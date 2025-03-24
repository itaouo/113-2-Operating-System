#include <stdio.h>
#include <stdlib.h>

void copy_file(const char *source, const char *destination) {
    FILE *src_file = fopen(source, "rb");
    if (src_file == NULL) {
        perror("Error opening source file");
        exit(1);
    }

    FILE *dest_file = fopen(destination, "wb");
    if (dest_file == NULL) {
        perror("Error opening destination file");
        fclose(src_file);
        exit(1);
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }

    printf("File copied successfully!\n");

    fclose(src_file);
    fclose(dest_file);
}

int main() {
    char source_file[256];
    char destination_file[256];
    printf("Enter source file path: ");
    scanf("%255s", source_file);

    printf("Enter destination file path: ");
    scanf("%255s", destination_file);
    
    copy_file(source_file, destination_file);

    return 0;
}
