import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;

public class FibonacciMultiThread {
    static class FibonacciTask implements Callable<Integer> {
        private final int n;

        public FibonacciTask(int n) {
            this.n = n;
        }

        @Override
        public Integer call() throws Exception {
            if (n <= 1) {
                return n;
            }
            FutureTask<Integer> f1 = new FutureTask<>(new FibonacciTask(n - 1));
            FutureTask<Integer> f2 = new FutureTask<>(new FibonacciTask(n - 2));

            Thread t1 = new Thread(f1);
            Thread t2 = new Thread(f2);
            t1.start();
            t2.start();

            return f1.get() + f2.get();
        }
    }

    public static void main(String[] args) {
        int n = 20;
        FutureTask<Integer> task = new FutureTask<>(new FibonacciTask(n));
        Thread thread = new Thread(task);
        thread.start();
        try {
            int result = task.get();
            System.out.println("Fibonacci(" + n + ") = " + result);
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }
    }
}
