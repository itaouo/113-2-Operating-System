c:\Users\Tim\Downloads\img.pngimport java.util.*;

public class SchedulingAlgorithms {
    public static void fcfs(List<Process> processes) {
        processes.sort(Comparator.comparingInt(p -> p.arrivalTime));
        int currentTime = 0;

        for (Process p : processes) {
            currentTime = Math.max(currentTime, p.arrivalTime);
            p.waitingTime = currentTime - p.arrivalTime;
            currentTime += p.burstTime;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
        }

        printResults("FCFS", processes);
    }

    public static void sjf(List<Process> processes) {
        List<Process> queue = new ArrayList<>();
        int currentTime = 0;
        List<Process> completed = new ArrayList<>();

        while (completed.size() < processes.size()) {
            for (Process p : processes) {
                if (p.arrivalTime <= currentTime && !queue.contains(p) && !completed.contains(p)) {
                    queue.add(p);
                }
            }
            if (queue.isEmpty()) {
                currentTime++;
                continue;
            }
            queue.sort(Comparator.comparingInt(p -> p.burstTime));
            Process p = queue.remove(0);
            p.waitingTime = currentTime - p.arrivalTime;
            currentTime += p.burstTime;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            completed.add(p);
        }

        printResults("SJF", completed);
    }

    public static void priority(List<Process> processes) {
        List<Process> queue = new ArrayList<>();
        int currentTime = 0;
        List<Process> completed = new ArrayList<>();

        while (completed.size() < processes.size()) {
            for (Process p : processes) {
                if (p.arrivalTime <= currentTime && !queue.contains(p) && !completed.contains(p)) {
                    queue.add(p);
                }
            }
            if (queue.isEmpty()) {
                currentTime++;
                continue;
            }
            queue.sort(Comparator.comparingInt(p -> p.priority));
            Process p = queue.remove(0);
            p.waitingTime = currentTime - p.arrivalTime;
            currentTime += p.burstTime;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            completed.add(p);
        }

        printResults("Priority", completed);
    }

    public static void roundRobin(List<Process> processes, int timeQuantum) {
        Queue<Process> queue = new LinkedList<>();
        List<Process> processList = new ArrayList<>(processes);
        int currentTime = 0;

        processList.sort(Comparator.comparingInt(p -> p.arrivalTime));
        int index = 0;
        List<Process> finished = new ArrayList<>();

        while (!queue.isEmpty() || index < processList.size()) {
            while (index < processList.size() && processList.get(index).arrivalTime <= currentTime) {
                queue.add(processList.get(index++));
            }

            if (queue.isEmpty()) {
                currentTime++;
                continue;
            }

            Process p = queue.poll();
            int execTime = Math.min(timeQuantum, p.remainingTime);
            currentTime += execTime;
            p.remainingTime -= execTime;

            while (index < processList.size() && processList.get(index).arrivalTime <= currentTime) {
                queue.add(processList.get(index++));
            }

            if (p.remainingTime > 0) {
                queue.add(p);
            } else {
                p.completionTime = currentTime;
                p.turnaroundTime = p.completionTime - p.arrivalTime;
                p.waitingTime = p.turnaroundTime - p.burstTime;
                finished.add(p);
            }
        }

        printResults("Round Robin", finished);
    }

    public static void priorityRoundRobin(List<Process> processes, int timeQuantum) {
        Map<Integer, Queue<Process>> priorityQueues = new TreeMap<>();
        List<Process> processList = new ArrayList<>(processes);
        int currentTime = 0;
        int index = 0;
        List<Process> finished = new ArrayList<>();

        processList.sort(Comparator.comparingInt(p -> p.arrivalTime));

        while (finished.size() < processes.size()) {
            while (index < processList.size() && processList.get(index).arrivalTime <= currentTime) {
                Process p = processList.get(index++);
                priorityQueues.computeIfAbsent(p.priority, k -> new LinkedList<>()).add(p);
            }

            boolean found = false;
            for (int prio : priorityQueues.keySet()) {
                Queue<Process> queue = priorityQueues.get(prio);
                if (queue.isEmpty()) continue;

                Process p = queue.poll();
                int execTime = Math.min(timeQuantum, p.remainingTime);
                currentTime += execTime;
                p.remainingTime -= execTime;

                while (index < processList.size() && processList.get(index).arrivalTime <= currentTime) {
                    Process arriving = processList.get(index++);
                    priorityQueues.computeIfAbsent(arriving.priority, k -> new LinkedList<>()).add(arriving);
                }

                if (p.remainingTime > 0) {
                    queue.add(p);
                } else {
                    p.completionTime = currentTime;
                    p.turnaroundTime = p.completionTime - p.arrivalTime;
                    p.waitingTime = p.turnaroundTime - p.burstTime;
                    finished.add(p);
                }
                found = true;
                break;
            }

            if (!found) currentTime++;
        }

        printResults("Priority Round Robin", finished);
    }

    private static void printResults(String algo, List<Process> processes) {
        System.out.println("\n--- " + algo + " Scheduling ---");
        System.out.printf("%-5s %-12s %-12s %-12s %-12s\n",
                "PID", "WaitingTime", "Turnaround", "Completion", "Priority");
        for (Process p : processes) {
            System.out.printf("%-5s %-12d %-12d %-12d %-12d\n",
                    p.pid, p.waitingTime, p.turnaroundTime, p.completionTime, p.priority);
        }
    }

    public static void main(String[] args) {
        List<Process> processes = Arrays.asList(
                new Process("P1", 0, 5, 2),
                new Process("P2", 1, 3, 1),
                new Process("P3", 2, 8, 3),
                new Process("P4", 3, 6, 2)
        );

        fcfs(cloneList(processes));
        sjf(cloneList(processes));
        priority(cloneList(processes));
        roundRobin(cloneList(processes), 2);
        priorityRoundRobin(cloneList(processes), 2);
    }

    private static List<Process> cloneList(List<Process> list) {
        List<Process> newList = new ArrayList<>();
        for (Process p : list) {
            newList.add(new Process(p.pid, p.arrivalTime, p.burstTime, p.priority));
        }
        return newList;
    }
}
