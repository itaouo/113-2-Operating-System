import java.util.concurrent.*;
import java.util.*;

public class SudokuValidator {
    private static final int SIZE = 9;
    private static final int[][] sudoku = {
        {5,3,4,6,7,8,9,1,2},
        {6,7,2,1,9,5,3,4,8},
        {1,9,8,3,4,2,5,6,7},
        {8,5,9,7,6,1,4,2,3},
        {4,2,6,8,5,3,7,9,1},
        {7,1,3,9,2,4,8,5,6},
        {9,6,1,5,3,7,2,8,4},
        {2,8,7,4,1,9,6,3,5},
        {3,4,5,2,8,6,1,7,9}
    };

    public static void main(String[] args) throws InterruptedException, ExecutionException {
        System.out.println("Sudoku Puzzle to Validate:");
        printSudoku();

        ExecutorService executor = Executors.newFixedThreadPool(11);
        List<Future<Boolean>> futures = new ArrayList<>();

        futures.add(executor.submit(new RowValidator(sudoku)));
        futures.add(executor.submit(new ColumnValidator(sudoku)));

        for (int row = 0; row < SIZE; row += 3) {
            for (int col = 0; col < SIZE; col += 3) {
                futures.add(executor.submit(new SubgridValidator(sudoku, row, col)));
            }
        }

        boolean isValid = true;
        for (Future<Boolean> future : futures) {
            if (!future.get()) {
                isValid = false;
                break;
            }
        }

        executor.shutdown();
        System.out.println(isValid ? "Sudoku solution is valid!" : "Sudoku solution is invalid!");
    }

    private static void printSudoku() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                System.out.print(sudoku[i][j] + " ");
            }
            System.out.println();
        }
    }
}

class RowValidator implements Callable<Boolean> {
    private final int[][] sudoku;

    public RowValidator(int[][] sudoku) {
        this.sudoku = sudoku;
    }

    @Override
    public Boolean call() {
        for (int i = 0; i < 9; i++) {
            boolean[] seen = new boolean[9];
            for (int j = 0; j < 9; j++) {
                int num = sudoku[i][j];
                if (num < 1 || num > 9 || seen[num-1]) return false;
                seen[num-1] = true;
            }
        }
        return true;
    }
}

class ColumnValidator implements Callable<Boolean> {
    private final int[][] sudoku;

    public ColumnValidator(int[][] sudoku) {
        this.sudoku = sudoku;
    }

    @Override
    public Boolean call() {
        for (int j = 0; j < 9; j++) {
            boolean[] seen = new boolean[9];
            for (int i = 0; i < 9; i++) {
                int num = sudoku[i][j];
                if (num < 1 || num > 9 || seen[num-1]) return false;
                seen[num-1] = true;
            }
        }
        return true;
    }
}

class SubgridValidator implements Callable<Boolean> {
    private final int[][] sudoku;
    private final int startRow;
    private final int startCol;

    public SubgridValidator(int[][] sudoku, int startRow, int startCol) {
        this.sudoku = sudoku;
        this.startRow = startRow;
        this.startCol = startCol;
    }

    @Override
    public Boolean call() {
        boolean[] seen = new boolean[9];
        for (int i = startRow; i < startRow + 3; i++) {
            for (int j = startCol; j < startCol + 3; j++) {
                int num = sudoku[i][j];
                if (num < 1 || num > 9 || seen[num-1]) return false;
                seen[num-1] = true;
            }
        }
        return true;
    }
}
