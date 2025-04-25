class Process {
    String pid;
    int arrivalTime;
    int burstTime;
    int priority;

    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;

    public Process(String pid, int arrivalTime, int burstTime, int priority) {
        this.pid = pid;
        this.arrivalTime = arrivalTime;
        this.burstTime = burstTime;
        this.priority = priority;
        this.remainingTime = burstTime;
    }
}