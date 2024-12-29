

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <climits> 
using namespace std;

struct Process {
    int pid;          // Process ID
    int arrivalTime;  // Arrival time
    int burstTime;    // Burst time
    int remainingTime; // Remaining time (used for SRTF and RR)
    int completionTime; // Completion time
    int waitingTime;   // Waiting time
    int turnAroundTime; // Turnaround time
};

// Function to print the Gantt chart
void printGanttChart(const vector<pair<int, int>>& gantt) {
    cout << "\nGantt Chart:\n";
    for (const auto& step : gantt) {
        cout << "| P" << step.first << " ";
    }
    cout << "|\n";
    for (const auto& step : gantt) {
        cout << setw(4) << step.second;
    }
    cout << endl;
}

// FCFS Scheduling
void FCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrivalTime < b.arrivalTime;
        });

    int time = 0;
    vector<pair<int, int>> gantt;

    for (auto& p : processes) {
        if (time < p.arrivalTime) time = p.arrivalTime;
        gantt.emplace_back(p.pid, time);
        time += p.burstTime;
        p.completionTime = time;
        p.turnAroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnAroundTime - p.burstTime;
    }

    printGanttChart(gantt);
    cout << "P\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto& p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t"
            << p.completionTime << "\t" << p.turnAroundTime << "\t" << p.waitingTime << endl;
    }
}

// SRTF Scheduling
void SRTF(vector<Process> processes) {
    sort(processes.begin(), processes.end(), [](Process a, Process b) {
        return a.arrivalTime < b.arrivalTime;
        });

    int time = 0, completed = 0, n = processes.size();
    vector<pair<int, int>> gantt;
    while (completed < n) {
        int idx = -1, minRemainingTime = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0 &&
                processes[i].remainingTime < minRemainingTime) {
                idx = i;
                minRemainingTime = processes[i].remainingTime;
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        gantt.emplace_back(processes[idx].pid, time);
        time++;
        processes[idx].remainingTime--;

        if (processes[idx].remainingTime == 0) {
            completed++;
            processes[idx].completionTime = time;
            processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
        }
    }

    printGanttChart(gantt);
    cout << "P\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto& p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t"
            << p.completionTime << "\t" << p.turnAroundTime << "\t" << p.waitingTime << endl;
    }
}

// Round-Robin Scheduling
void RoundRobin(vector<Process> processes, int quantum) {
    int time = 0, completed = 0, n = processes.size();
    vector<pair<int, int>> gantt;
    vector<int> readyQueue;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= time && processes[i].remainingTime > 0) {
                readyQueue.push_back(i);
            }
        }

        if (!readyQueue.empty()) {
            int idx = readyQueue.front();
            readyQueue.erase(readyQueue.begin());

            gantt.emplace_back(processes[idx].pid, time);

            if (processes[idx].remainingTime <= quantum) {
                time += processes[idx].remainingTime;
                processes[idx].remainingTime = 0;
                completed++;
                processes[idx].completionTime = time;
                processes[idx].turnAroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].burstTime;
            }
            else {
                time += quantum;
                processes[idx].remainingTime -= quantum;
                readyQueue.push_back(idx);
            }
        }
        else {
            time++;
        }
    }

    printGanttChart(gantt);
    cout << "P\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto& p : processes) {
        cout << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t"
            << p.completionTime << "\t" << p.turnAroundTime << "\t" << p.waitingTime << endl;
    }
}

int main() {
    ifstream file("processes.txt");
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    int n, quantum;
    file >> n;
    vector<Process> processes(n);

    for (int i = 0; i < n; i++) {
        file >> processes[i].pid >> processes[i].arrivalTime >> processes[i].burstTime;
        processes[i].remainingTime = processes[i].burstTime;
    }
    file >> quantum;

    cout << "Choose a scheduling algorithm:\n1. FCFS\n2. SRTF\n3. Round Robin\n";
    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        FCFS(processes);
        break;
    case 2:
        SRTF(processes);
        break;
    case 3:
        RoundRobin(processes, quantum);
        break;
    default:
        cout << "Invalid choice!" << endl;
        break;
    }

    return 0;
}
 
