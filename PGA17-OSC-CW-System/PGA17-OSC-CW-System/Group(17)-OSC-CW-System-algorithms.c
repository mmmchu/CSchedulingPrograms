#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// Define a structure for multi-level process id, burst time, arrival time, total turnaround time, completion time, temp completion time, and waiting time
struct MLFQ
{
    int pid;
    int bt;
    int at;
    int tat;
    int ct;
    int temporary;
    int wt;
    struct MLFQ *next; // Pointer to the next node in the linked list
};
struct Node{
    int pnum;
    int at;
    int bt;
    int rt;
    struct Node* next;
};
struct Node1{
    int pnum;
    int at;
    int bt;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    struct Node1* next;
};
struct FCFSnode{
        int pnum;
        int at;
        int bt;
        int waitingTime;
        int completionTime;
        int turnaroundTime;
        struct FCFSnode* next;
};
// Function to Enqueue a Node at the end of the linked list
struct Node* Enqueue(struct Node* tail, struct Node* Node){
    // Link the current tail to the new Node and update the tail
    tail->next = Node;
    tail = tail->next;
    return tail;
}
// Function to Dequeue a Node from the front of the linked list
struct Node* Dequeue(struct Node** head){
    struct Node* node;
    struct Node* temp;

    // Retrieve the Node at the head and update the head pointer
    node = *head;
    temp = *head;
    temp = temp->next;
    *head = temp;
    node->next = NULL;

    return node;
}
// Function to create a new Node1 with specified arrival time, burst time, process number, and completion time
struct Node1* createNode1(int arrivaltime, int bursttime, int procNum, int completionT){
    struct Node1* newNode1 = (struct Node1*)malloc(sizeof(struct Node1));

    // Initialize Node1 attributes
    newNode1->pnum = procNum;
    newNode1->at = arrivaltime;
    newNode1->bt = bursttime;
    newNode1->completionTime = completionT;
    newNode1->turnaroundTime = completionT - arrivaltime;
    newNode1->waitingTime = completionT - arrivaltime - bursttime;
    newNode1->next = NULL;

    return newNode1;
}
// Function to create a new Node with specified arrival time, burst time, process number, and completion time
struct Node* createNode(int arrivaltime, int bursttime, int procNum){
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->pnum = procNum;
    newNode->at = arrivaltime;
    newNode->bt = bursttime;
    newNode->rt = bursttime;
    newNode->next = NULL;
    return newNode;
}
// Function to calculate a specific metric (Waiting Time, Turnaround Time, or Throughput) for a given linked list
float calculateMetric(struct Node1* head, int noofprocess, int metricType) {
    struct Node1* currentNode = head;
    int sum = 0;
    int count = 0;
    float metric = 0.0;

    // Initialize variables for calculating total time spanned by processes
    int minAT = head->at, maxCT = head->completionTime, totalTime;

    // Traverse the list and accumulate metric values
    while (currentNode != NULL) {
        count++;
        // Choose whether to accumulate Waiting Time or Turnaround Time based on metricType
        sum += (metricType == 0) ? currentNode->waitingTime : currentNode->turnaroundTime;

        // Update minAT and maxCT for calculating total time
        minAT = (currentNode->at < minAT) ? currentNode->at : minAT;
        maxCT = (currentNode->completionTime > maxCT) ? currentNode->completionTime : maxCT;

        currentNode = currentNode->next;
    }

    // Calculate total time spanned by processes
    totalTime = maxCT - minAT;

    // Calculate metric based on metricType (Throughput or Average Waiting Time/Turnaround Time)
    metric = (metricType == 2) ? (float)noofprocess / totalTime : (float)sum / count;

    return metric;
}
// Function to perform a bubble sort on a linked list based on arrival time
struct Node* sorting(struct Node* head){
    int isSwapped;
    struct Node* temp;
    do{
        isSwapped = 0;
        struct Node* tail2 = head;
        struct Node* previousNode = NULL;

        while(tail2 -> next != NULL) {
            struct Node* nextNode = tail2 -> next;

            // Traverse the list and swap nodes if the arrival time is out of order
            if(tail2->at > nextNode->at) {
                if(previousNode != NULL){
                    previousNode->next = nextNode;
                }else{
                    head = nextNode;
                }

                tail2->next = nextNode->next;
                nextNode->next = tail2;

                temp = tail2;
                tail2 = nextNode;
                nextNode = temp;

                isSwapped = 1;
            }
            previousNode = tail2;
            tail2 = tail2->next;
        }
    }while(isSwapped);

    return head;
}
// Function to perform Round Robin scheduling
struct Node1* RRbubble(struct Node1* head2) {
    int isSwapped;
    struct Node1* temp;

    // Repeat until no more swaps are needed
    do {
        isSwapped = 0;
        struct Node1* currentNode1 = head2;
        struct Node1* previousNode1 = NULL;

        // Traverse the linked list
        while (currentNode1->next != NULL) {
            struct Node1* nextNode1 = currentNode1->next;

            // If current process has higher priority than the next one, swap them
            if (currentNode1->pnum > nextNode1->pnum) {
                if (previousNode1 != NULL) {
                    previousNode1->next = nextNode1;
                } else {
                    head2 = nextNode1;
                }

                currentNode1->next = nextNode1->next;
                nextNode1->next = currentNode1;

                temp = currentNode1;
                currentNode1 = nextNode1;
                nextNode1 = temp;

                isSwapped = 1;
            }

            previousNode1 = currentNode1;
            currentNode1 = currentNode1->next;
        }
    } while (isSwapped);

    return head2;
}
struct Node1* rrlogic(struct Node* head, struct Node1* head2) {
    int quantum;
    int currentTime = 0;
    struct Node1* currentNode;
    struct Node* temp;
    struct Node* queueHead;
    struct Node* queueTail;

    // Get time quantum with a while loop for input validation
    printf("\nEnter the time quantum:\n");
    while (1) {
        scanf("%d", &quantum);

        if (quantum > 0) {
            break;  // Exit the loop if quantum is valid
        } else {
            printf("Invalid \nPlease input a valid number");
        }
    }

    currentTime = head->at;

    // Initialize the queue
    queueHead = createNode(head->at, head->bt, head->pnum);
    queueTail = queueHead;
    temp = head->next;
    free(head);
    head = temp;

    // Print Gantt chart header
    printf("\nGantt Chart:\n");
    printf("+-----------------------------------+\n");
    printf("| Current Time | Process             |\n");
    printf("+-----------------------------------+\n");

    // Main Round Robin loop
    while (queueHead != NULL) {
        // If remaining time is greater than quantum
        if (queueHead->rt > quantum) {
            // Enqueue new processes with arrival time <= current time
            while (head != NULL && head->at <= currentTime) {
                queueTail->next = createNode(head->at, head->bt, head->pnum);
                queueTail = queueTail->next;
                temp = head->next;
                free(head);
                head = temp;
            }

            // Print the Gantt chart
            printf("| %-13d | P%d                |\n", currentTime, queueHead->pnum);

            // Update remaining time and current time
            queueHead->rt -= quantum;
            currentTime += quantum;

            // Enqueue the current process back
            queueTail->next = queueHead;
            queueHead = queueHead->next;
            queueTail = queueTail->next;
            queueTail->next = NULL;

            if (queueHead == queueTail) {
                queueHead->next = NULL;
            }
        } else {  // If remaining time is less than or equal to quantum
            // Update current time and Enqueue new processes
            currentTime += queueHead->rt;

            // Print the Gantt chart
            printf("| %-13d | P%d                |\n", currentTime, queueHead->pnum);

            while (head != NULL && head->at <= currentTime) {
                queueTail->next = createNode(head->at, head->bt, head->pnum);
                queueTail = queueTail->next;
                temp = head->next;
                free(head);
                head = temp;
            }

            // Update Node1 and Dequeue the current process
            if (head2 == NULL) {
                head2 = createNode1(queueHead->at, queueHead->bt, queueHead->pnum, currentTime);
                currentNode = head2;
            } else {
                currentNode->next = createNode1(queueHead->at, queueHead->bt, queueHead->pnum, currentTime);
                currentNode = currentNode->next;
            }

            if (queueHead->next == NULL) {
                if (head != NULL) {
                    queueHead->next = createNode(head->at, head->bt, head->pnum);
                    queueTail = queueHead->next;
                }

                if (queueTail->at >= currentTime) {
                    currentTime = queueTail->at;
                }
            }

            temp = queueHead->next;
            free(queueHead);
            queueHead = temp;
        }
    }

    // Print Gantt chart footer
    printf("+-----------------------------------+\n");

    return head2;
}

// Function to calculate throughput for FCFS scheduling
float runfcfsTP(struct FCFSnode* head, int noofprocess) {
    float throughput;
    struct FCFSnode* currentNode = head;
    int minAT = head->at, maxCT = head->completionTime, totalTime;

    // Traverse the linked list to find minimum arrival time and maximum completion time
    while (currentNode != NULL) {
        if (currentNode->at < minAT) {
            minAT = currentNode->at;
        }
        if (currentNode->completionTime > maxCT) {
            maxCT = currentNode->completionTime;
        }

        currentNode = currentNode->next;
    }

    totalTime = maxCT - minAT;

    // Calculate throughput
    throughput = (float)noofprocess / totalTime;

    return throughput;
}
// Function to create a node for FCFS scheduling
struct FCFSnode* createFCFSnode(int arrivaltime, int bursttime, int procNum) {
    struct FCFSnode* newNode = (struct FCFSnode*)malloc(sizeof(struct FCFSnode));
    newNode->pnum = procNum;
    newNode->at = arrivaltime;
    newNode->bt = bursttime;
    newNode->next = NULL;
    return newNode;
}
// Function to perform Bubble Sort for FCFS scheduling based on arrival time
struct FCFSnode* FCFSbubble(struct FCFSnode* head) {
    int isSwapped;
    struct FCFSnode* temp;

    // Repeat until no more swaps are needed
    do {
        isSwapped = 0;
        struct FCFSnode* currentNode = head;
        struct FCFSnode* previousNode = NULL;

        // Traverse the linked list
        while (currentNode->next != NULL) {
            struct FCFSnode* nextNode = currentNode->next;

            // If current process has higher arrival time than the next one, swap them
            if (currentNode->at > nextNode->at) {
                if (previousNode != NULL) {
                    previousNode->next = nextNode;
                } else {
                    head = nextNode;
                }

                currentNode->next = nextNode->next;
                nextNode->next = currentNode;

                temp = currentNode;
                currentNode = nextNode;
                nextNode = temp;

                isSwapped = 1;
            }
            previousNode = currentNode;
            currentNode = currentNode->next;
        }
    } while (isSwapped);

    return head;
}
//To calculate TAT,WT,CT
void calculateMetricsfcfs(struct FCFSnode* head) {
    struct FCFSnode* currentNode = head;
    struct FCFSnode* previousNode = NULL;

    while (currentNode != NULL) {
        // Calculate Waiting Time (WT) based on arrival time and completion time of the previous process
        currentNode->waitingTime = (previousNode != NULL && currentNode->at < previousNode->completionTime)
                                        ? previousNode->completionTime - currentNode->at
                                        : 0;
        // Calculate Completion Time (CT) and Turnaround Time (TAT)
        currentNode->completionTime = currentNode->at + currentNode->bt + currentNode->waitingTime;
        currentNode->turnaroundTime = currentNode->waitingTime + currentNode->bt;

        previousNode = currentNode;
        currentNode = currentNode->next;
    }
}
// Function to calculate the average Turnaround Time (TAT) or Waiting Time (WT) for all processes in FCFS scheduling
float avgTrunfcfs(struct FCFSnode* head, int (*getMetric)(struct FCFSnode*, int), int waitingtime) {
    struct FCFSnode* currentNode = head;
    int sum = 0;
    int count = 0;
    float averageTime = 0.0;

    // Traverse the list and accumulate the metric values
    while (currentNode != NULL) {
        count++;
        sum += getMetric(currentNode, waitingtime);
        currentNode = currentNode->next;
    }

    // Calculate the average metric value
    if (count > 0) {
        averageTime = (float)sum / count;
    }
    return averageTime;
}
// Function to perform a bubble sort on the FCFS scheduling queue based on process number
struct FCFSnode* bb2(struct FCFSnode* head){
    int isSwapped;
    struct FCFSnode* temp;
    do {
        isSwapped = 0;
        struct FCFSnode* currentNode = head;
        struct FCFSnode* previousNode = NULL;

        // Traverse the list and swap nodes if the process number is out of order
        while(currentNode -> next != NULL) {
            struct FCFSnode* nextNode = currentNode -> next;

            if(currentNode->pnum > nextNode->pnum) {
                // Adjust pointers to swap nodes
                if(previousNode != NULL){
                    previousNode->next = nextNode;
                } else {
                    head = nextNode;
                }

                currentNode->next = nextNode->next;
                nextNode->next = currentNode;

                // Swap the nodes
                temp = currentNode;
                currentNode = nextNode;
                nextNode = temp;

                isSwapped = 1;
            }
            previousNode = currentNode;
            currentNode = currentNode->next;
        }
    } while(isSwapped);

    return head;
}
// Function to get a specific metric value (either WT or TAT) for a given FCFS node
int getMetric(struct FCFSnode* node, int waitingtime) {
    if (waitingtime) {
        return node->waitingTime;
    } else {
        return node->turnaroundTime;
    }
}
// Function to sort processes by process ID
struct Node* sortProcessesByID(struct Node* head) {
    struct Node *current, *index;
    int tempAt, tempBt, tempPnum;

    if (head == NULL) {
        return head;
    } else {
        for (current = head; current->next != NULL; current = current->next) {
            for (index = current->next; index != NULL; index = index->next) {
                if (current->pnum > index->pnum) {
                    tempAt = current->at;
                    tempBt = current->bt;
                    tempPnum = current->pnum;

                    current->at = index->at;
                    current->bt = index->bt;
                    current->pnum = index->pnum;

                    index->at = tempAt;
                    index->bt = tempBt;
                    index->pnum = tempPnum;
                }
            }
        }
    }
    return head;
}
// Function to swap two MLFQ structures
void swap(struct MLFQ *xp, struct MLFQ *yp)
{
    struct MLFQ temp = *xp;
    *xp = *yp;
    *yp = temp;
}
// Bubble sort algorithm to swap the processes based on arrival time
void bbsort3(struct MLFQ arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j].at > arr[j + 1].at)
            {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Sort processes based on process id
void sortPID(struct MLFQ arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j].pid > arr[j + 1].pid)
            {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

// Function to display the final table with results
void displayTable(struct MLFQ proc[], int n) {
    printf("\n+--------------------------------------- MLFQ TABLE -------------------------------------+\n");
    printf("| Process | Arrival Time | Burst Time | Turnaround Time | Waiting Time | Completion Time |\n");


    for (int i = 0; i < n; i++) {
        printf("+----------------------------------------------------------------------------------------+\n");
        printf("|   P%d    |      %2d      |     %2d     |        %2d       |      %2d      |        %2d       |\n",
               proc[i].pid, proc[i].at, proc[i].bt, proc[i].tat, proc[i].wt, proc[i].ct);
    }
    printf("+----------------------------------------------------------------------------------------+\n");
    printf("\n");
}

void displayGanttChart(struct MLFQ gantt[], int count) {
    // Display the Gantt Chart
    printf("\nGantt Chart\n");

    // Display the Gantt Chart header
    printf("|");


    for (int i = 0; i < count; i++) {
        printf("P%d\t|", gantt[i].pid);
    }

}

void runfcfs() {
    int num = 0;
    int arrivaltime, bursttime;
    struct FCFSnode* head = NULL;
    struct FCFSnode* currentNode = NULL;

    printf("Enter the number of processes: ");
    scanf("%d", &num);

    for (int i = 1; i <= num; i++) {
        printf("\nEnter arrival time for process %d: ", i);
        scanf("%d", &arrivaltime);

        printf("Enter burst time for process %d: ", i);
        scanf("%d", &bursttime);

        if (i == 1) {
            head = createFCFSnode(arrivaltime, bursttime, i);
            currentNode = head;
        } else {
            currentNode->next = createFCFSnode(arrivaltime, bursttime, i);
            currentNode = currentNode->next;
        }
    }

    head =FCFSbubble(head);
    calculateMetricsfcfs(head);
    head = bb2(head);

    printf("\n");
    printf("+------------------------------------------------------------------------------------------+\n");
    printf("| Process   | arrival time | Burst time | Completion time | Waiting time | Turnaround time |\n");
    printf("+------------------------------------------------------------------------------------------+\n");

    currentNode = head;
    while (currentNode != NULL) {
        printf("|%-11d|%-14d|%-12d|%-17d|%-14d|%-16d |\n", currentNode->pnum, currentNode->at, currentNode->bt, currentNode->completionTime, currentNode->waitingTime, currentNode->turnaroundTime);
         printf("+------------------------------------------------------------------------------------------+\n"); // Added line
        currentNode = currentNode->next;
    }

    printf("\n");

    printf("\nAverage Waiting Time: %.2f\n", avgTrunfcfs(head, getMetric, 1)); // Pass 1 for waiting time
    printf("Throughput: %.2f\n", runfcfsTP(head, num));
    printf("Average Turnaround Time: %.2f\n\n", avgTrunfcfs(head, getMetric, 0)); // Pass 0 for turnaround time
}
void runRR() {
    int num = 0;
    int arrivalT, bursttime;

    struct Node* head;
    struct Node* currentNode;

    struct Node1* head2 = NULL;

    // Get the number of processes
    do {

        printf("\nEnter the number of processes: ");
        scanf("%d", &num);

        if (num <= 0) {
            printf("Invalid number \nPlease input valid number\n");
        }
    } while (num <= 0);

    // Input process details
    for (int i = 1; i <= num; i++) {
        if (i == 1) {
            // First process
            printf("\nEnter arrival time for process %d: ", i);
            scanf("%d", &arrivalT);

            printf("Enter burst time for process %d: ", i);
            scanf("%d", &bursttime);

            head = createNode(arrivalT, bursttime, i);
            currentNode = head;
        } else {
            // Subsequent processes
            printf("\nEnter arrival time for process %d: ", i);
            scanf("%d", &arrivalT);

            printf("Enter burst time for process %d: ", i);
            scanf("%d", &bursttime);

            currentNode->next = createNode(arrivalT, bursttime, i);
            currentNode = currentNode->next;
        }
    }

     // Sort processes by arrival time
    head = sorting(head);

    // Run the Round Robin rrlogic
    head2 = rrlogic(head, head2);

    // Sort processes by process ID
    head2 = RRbubble(head2);

    struct Node1* currentNode1 = head2;
    int currentTime = 0;
    printf("\n");

    // Display results
    printf("+------------------------------------------------------------------------------------------+\n");
    printf("| Process   | Arrival time | Burst time | Completion Time | Turnaround Time | Waiting time |\n");
    printf("+------------------------------------------------------------------------------------------+\n");


    currentNode1 = head2;
    while (currentNode1 != NULL) {
        printf("|%-11d|%-14d|%-12d|%-17d|%-17d|%-14d|\n", currentNode1->pnum, currentNode1->at, currentNode1->bt, currentNode1->completionTime, currentNode1->turnaroundTime, currentNode1->waitingTime);
        printf("+------------------------------------------------------------------------------------------+\n"); // Added line
        currentNode1 = currentNode1->next;
    }

    printf("\n");

     printf("Average Waiting Time: %.2f\n", calculateMetric(head2, num, 0));  // 0 for waiting time
    printf("Throughput: %.2f\n", calculateMetric(head2, num, 2));  // 2 for throughput
    printf("Average Turnaround Time: %.2f\n\n", calculateMetric(head2, num, 1));  // 1 for turnaround time
}

int runMLFQ()
{
    // create array and variables needed
    struct MLFQ proc[20], remaining[20],gantt[20];
    int n, qt;
    float averageWT,averageTAT;
    float numofprocess=0,ct=0,tp=0;

    // Ask the user for the number of processes
    printf("Enter number of processes: ");
    scanf("%d", &n);

    //take the number of process as float to calculate throughput
    numofprocess = n;

    // ask user for processes waiting time and arrival time
    for (int i = 0; i < n; i++)
    {

        proc[i].pid = i + 1;

        printf("Enter arrival time of process %d : ", i + 1);
        scanf("%d", &proc[i].at);

        printf("Enter burst time of process %d: ", i + 1);
        scanf("%d", &proc[i].bt);

        remaining[i] = proc[i];
    }

    // let user enter the quantum
    printf("Enter time quantum:");
    scanf("%d", &qt);


    bbsort3(proc, n);
    bbsort3(remaining, n);

    //initialize average waiting time and average turnaround time as 0
    averageWT=0.0;
    averageTAT=0.0;



    // round robin (queue 1)
     if (proc[0].at!=0)
    {
        printf("\n+---------------------------------------------------------+\n");
        printf("|   CPU Idle/No Processes      |            0 to %d         |\n", proc[0].at);
        printf("+----------------------------------------------------------+\n");
    }
    int currenttime = proc[0].at;
    int done = 0;
    int count = 0;
    int fcfscount =0;
    int endrr[n];
    bool idleprocess = false;
    int currenttime_fcfs =0;

    // while value of variable done < n, continue loop
    while (done < n)
    {
        int currenttime_fcfs = currenttime;
        for (int i = 0; i < n; i++)
        {
            // if there is idle, run fcfs process

            if (currenttime < proc[i].at && fcfscount > 0)
            {
                idleprocess = true;

                printf("\n+---------------------------------FCFS queue--------------------------------------------+\n");
                printf("|Process\t|Start Time\t|Turnaround Time\t|Waiting Time\t|Completion Time|\n");
                printf("+--------------------------------------------------------------------------------------+\n");
                // FCFS (queue 2)
                currenttime_fcfs = currenttime;
                for (int i=0; i<n ; i++)
                {

                    // if remaining burst time larger than 0, increase current time by remaining burst time
                    if (remaining[i].bt > 0)
                    {
                        int temp=currenttime;
                        currenttime_fcfs += remaining[i].bt;
                        remaining[i].ct = currenttime_fcfs;

                        // calculate waiting time
                        int wt=temp-proc[remaining[i].pid].temporary;
                        remaining[i].tat = currenttime_fcfs - remaining[i].at;
                        remaining[i].wt = remaining[i].tat - remaining[i].bt - qt;
                        printf("P%d\t%dms\t\t%dms\t\t%dms\t\t%dms\t\t\n", remaining[i].pid, temp, remaining[i].ct-currenttime + wt, currenttime-proc[remaining[i].pid].temporary,remaining[i].ct);
                        //Display completed in first come first served queue if process is completed in this queue
                        printf("\n+--------------------------Process %d completed in FCFS queue---------------------------+\n\n", remaining[i].pid);

                        proc[i].ct = remaining[i].ct;
                        proc[i].wt = remaining[i].wt;
                        proc[i].tat = remaining[i].tat;

                        currenttime = remaining[i].ct;
                        averageWT = averageWT + remaining[i].wt;
                        averageTAT = averageTAT + remaining[i].tat;
                        gantt[count].wt = remaining[i].ct;
                        gantt[count].at = remaining[i].at;
                        gantt[count].pid = remaining[i].pid;
                        gantt[count].ct = temp;
                        gantt[count].bt = remaining[i].bt;
                        gantt[count].temporary = 1;
                        count++;
                        fcfscount --;
                        remaining[i].bt = 0;

                        if ( idleprocess == true)
                        {
                            if (fcfscount == 0 || currenttime >= proc[i+2].at )
                            {
                            idleprocess = false;
                            break;
                            }


                        }


                    }
                    // set waiting time to zero if it is smaller than 0
                    if (proc[i].wt < 0 )
                    {
                        proc[i].wt = 0;
                    }
                }
                if (currenttime < proc[i].at && fcfscount == 0)
                {
                    int temp4=currenttime;
                    printf("\n              |CPU Idle/No Processes |%d to %d |           \n\n",temp4, proc[i].at);
                    // set current time as process arrival time
                    currenttime = proc[i].at;

                    gantt[count-1].temporary = 0;
                }


            }

            if (remaining[i].bt > 0)
            {
                printf("\n+-----------------------------Round robin queue-----------------------------------------+\n");
                printf("|Process\t|Start Time\t|Turnaround Time\t|Waiting Time\t|Completion Time|\n");
                 printf("+--------------------------------------------------------------------------------------+\n");
                // if remaining burst time > 0 and <= quantum,
                if (remaining[i].bt <= qt)
                {
                    // calculate current time, completion time, turnaround time, waiting time, burst time, average turnaround time and average waiting time
                    int temp = currenttime;
                    currenttime = currenttime + proc[i].bt;
                    proc[i].ct= currenttime;
                    proc[i].tat = currenttime - proc[i].at;
                    proc[i].wt = proc[i].tat - proc[i].bt;
                    averageWT = averageWT + proc[i].wt;
                    averageTAT = averageTAT + proc[i].tat;

                    remaining[i].bt = 0;
                    proc[proc[i].pid].temporary=currenttime;

                    done++;

                    printf("|\tP%d\t|\t%dms\t|\t%dms\t\t|\t%dms\t|\t%dms\t|", proc[i].pid, temp, proc[i].tat, proc[i].wt,proc[i].ct);
                    // Display completed if process completely runs its burst time
                    printf("\n+----------------------Process %d completed in round robin queue-------------------------+\n\n", proc[i].pid);

                    gantt[count].at = proc[i].at;
                    gantt[count].pid = proc[i].pid;
                    gantt[count].ct = temp;
                    gantt[count].bt = proc[i].bt;
                    gantt[count].wt = proc[i].ct;


                    count++;

                }
                else
                {
                    fcfscount++;
                    // if remaining burst time >quantum
                    int temp=currenttime;

                    // increase current time value by quantum
                    currenttime = currenttime + qt;

                    // calculate remaining burst time, turnaround time, waiting time and completion time
                    remaining[i].bt = remaining[i].bt - qt;
                    proc[i].tat = currenttime - proc[i].at;
                    proc[i].wt = proc[i].tat - proc[i].bt;
                    proc[i].ct= currenttime;

                    // set waiting time to 0 if waiting time < 0 is get from calculation
                    if (proc[i].wt < 0)
                    {
                        proc[i].wt = 0;
                    }

                    printf("|\tP%d\t|\t%dms\t|\t%dms\t\t|\t%dms\t|\t%dms\t|", proc[i].pid, temp,proc[i].tat, temp-proc[i].at,currenttime);
                    printf("\n+----------------------Process %d not completed, go to FCFS------------------------------+\n\n", proc[i].pid);
                    done++;
                    proc[proc[i].pid].temporary=currenttime;
                    gantt[count].wt = proc[i].ct;
                    gantt[count].at = proc[i].at;
                    gantt[count].pid = proc[i].pid;
                    gantt[count].ct = temp;
                    gantt[count].bt = proc[i].bt;
                    count ++;
              \
                }
            }
        }
    }

      printf("\n+---------------------------------FCFS queue--------------------------------------------+\n");
      printf("|Process\t|Start Time\t|Turnaround Time\t|Waiting Time\t|Completion Time|\n");
      printf("+--------------------------------------------------------------------------------------+\n");
    // FCFS (queue 2)
    currenttime_fcfs = currenttime;
    for (int i=0; i<n ; i++)
    {
        // if remaining burst time larger than 0, increase current time by remaining burst time
        if (remaining[i].bt > 0)
        {
            int temp=currenttime;
            currenttime_fcfs += remaining[i].bt;
            remaining[i].ct = currenttime_fcfs;

            // calculate waiting time
            int wt=temp-proc[remaining[i].pid].temporary;
            remaining[i].tat = currenttime_fcfs - remaining[i].at;
            remaining[i].wt = remaining[i].tat - remaining[i].bt - qt;
            printf("|\tP%d\t|\t%dms\t|\t%dms\t\t|\t%dms\t|\t%dms\t|", remaining[i].pid, temp, remaining[i].ct-currenttime + wt, currenttime-proc[remaining[i].pid].temporary,remaining[i].ct);
            //Display completed in first come first served queue if process is completed in this queue
            printf("\n+--------------------------Process %d completed in FCFS queue---------------------------+\n\n", remaining[i].pid);
            //take the completion time as float to calculate throughput
            ct=remaining[i].ct;

            currenttime = remaining[i].ct;
            averageWT = averageWT + remaining[i].wt;
            averageTAT = averageTAT + remaining[i].tat;
            gantt[count].wt = remaining[i].ct;
            gantt[count].at = remaining[i].at;
            gantt[count].pid = remaining[i].pid;
            gantt[count].ct = temp;
            gantt[count].bt = remaining[i].bt;
            count++;

        }
        // set waiting time to zero if it is smaller than 0
        if (proc[i].wt < 0 )
        {
            proc[i].wt = 0;
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (remaining[i].bt!=0)
        {
            for (int j = 0; j < n; j++)
                {
                    if (remaining[i].pid==proc[j].pid)
                    {
                        proc[j]=remaining[i];
                        proc[j].bt=remaining[i].bt+qt;
                    }
                }
        }
    }

    displayGanttChart(gantt, count);
    printf("\n");
    sortPID(proc,n);


    displayTable(proc, n);

    averageWT = averageWT/n;
    averageTAT= averageTAT/n;
    tp  = numofprocess/ct;

    printf("\nAverage waiting time: %.2f\n",averageWT);
    printf("Average turnaround time: %.2f",averageTAT);
    printf("\nThroughput: %.2f \n\n",tp);

    // multi-level feedback queue ended
    return 0;
}


int main(){
int choice;
    int num; // num represents the number of processes
    struct process* p = NULL;
    int tq;


while (1)
{
    printf("Please choose an algorithm:\n1.First Come First Serve \n2.Round Robin \n3.Multi-Level Feedback Queue \n4.Quit\n");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:

        runfcfs();
        break;

    case 2:
        runRR();
        break;

    case 3:
        runMLFQ();
        break;


    case 4:
        exit(1);
    default:
        break;

    }

}

}
