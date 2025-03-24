#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define HISTORY_SIZE 10

char history[HISTORY_SIZE][MAX_LINE];
int history_count = 0;

void add_to_history(const char *cmd) {
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count], cmd);
        history_count++;
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], cmd);
    }
}

void print_history() {
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s", i + 1, history[i]);
    }
}

// 執行帶有管道的命令
void execute_piped_command(char **args1, char **args2) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // 第一個子進程，寫入管道
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execvp(args1[0], args1);
        perror("exec failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // 第二個子進程，從管道讀取
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execvp(args2[0], args2);
        perror("exec failed");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

// 執行指令（支援輸入 `<` 與輸出 `>` 重定向）
void execute_command(char **args) {
    int fd_in = -1, fd_out = -1;
    int i = 0;

    // 檢查是否有 `<` 或 `>`
    while (args[i] != NULL) {
        if (strcmp(args[i], ">") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: Missing file name after '>'\n");
                return;
            }
            // 打開檔案作為輸出（覆寫模式）
            fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open failed");
                return;
            }
            args[i] = NULL;  // `>` 及其後參數不傳遞給 `execvp()`
        } 
        else if (strcmp(args[i], "<") == 0) {
            if (args[i + 1] == NULL) {
                fprintf(stderr, "Error: Missing file name after '<'\n");
                return;
            }
            // 打開檔案作為輸入
            fd_in = open(args[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("open failed");
                return;
            }
            args[i] = NULL;  // `<` 及其後參數不傳遞給 `execvp()`
        }
        i++;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子行程
        if (fd_in != -1) {
            dup2(fd_in, STDIN_FILENO); // 讓 stdin 指向檔案
            close(fd_in);
        }
        if (fd_out != -1) {
            dup2(fd_out, STDOUT_FILENO); // 讓 stdout 指向檔案
            close(fd_out);
        }
        execvp(args[0], args);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // 父行程
        wait(NULL);
        if (fd_in != -1) close(fd_in);
        if (fd_out != -1) close(fd_out);
    } else {
        perror("fork failed");
    }
}

void parse_and_execute(char *input) {
    add_to_history(input);

    char *args[MAX_ARGS];
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];
    char *saveptr;
    char *token = strtok_r(input, " \t\n", &saveptr);
    int i = 0, pipe_index = -1;

    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            pipe_index = i;
            args[i] = NULL;  // 分隔命令
        } else {
            args[i] = token;
        }
        token = strtok_r(NULL, " \t\n", &saveptr);
        i++;
    }
    args[i] = NULL;

    if (args[0] == NULL) return;

    if (strcmp(args[0], "exit") == 0) exit(0);

    if (strcmp(args[0], "history") == 0) {
        print_history();
        return;
    }

    if (pipe_index != -1) {
        // 分割 args1 和 args2
        for (int j = 0; j < pipe_index; j++) {
            args1[j] = args[j];
        }
        args1[pipe_index] = NULL;

        int k = 0;
        for (int j = pipe_index + 1; args[j] != NULL; j++) {
            args2[k++] = args[j];
        }
        args2[k] = NULL;

        execute_piped_command(args1, args2);
        return;
    }

    execute_command(args);
}


int main() {
    char input[MAX_LINE];

    while (1) {
        printf("shell> ");
        if (!fgets(input, MAX_LINE, stdin)) break;
        parse_and_execute(input);
    }

    return 0;
}
