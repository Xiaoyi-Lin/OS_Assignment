#ifndef OSLABS_H
#define OSLABS_H


// Define the Process Control Block (PCB) structure
struct PCB {
    int process_id;
    int arrival_timestamp;
    int total_bursttime;
    int execution_starttime;
    int execution_endtime;
    int remaining_bursttime;
    int process_priority;
};

// Define NULLPCB
#define NULLPCB (struct PCB) {0, 0, 0, 0, 0, 0, 0}

// Function prototypes
struct PCB handle_process_arrival_pp(struct PCB ready_queue[], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp);
struct PCB handle_process_completion_pp(struct PCB ready_queue[], int *queue_cnt, int timestamp);
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp);
struct PCB handle_process_completion_srtp(struct PCB ready_queue[], int *queue_cnt, int timestamp);
struct PCB handle_process_arrival_rr(struct PCB ready_queue[], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum);
struct PCB handle_process_completion_rr(struct PCB ready_queue[], int *queue_cnt, int timestamp, int time_quantum);
// Add function prototypes for SRTP and RR methods here

#endif /* OSLABS_H */

