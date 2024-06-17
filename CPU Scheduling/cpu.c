// Include the necessary libraries
#include <stdio.h>
#include <stdlib.h>
#include "oslabs.h"
#define MAX_PROCESSES 100
#define QUEUEMAX 100
// Define NULLPCB
#define NULLPCB (struct PCB) {0, 0, 0, 0, 0, 0, 0}

// Method to handle the arrival of a process for PP
struct PCB handle_process_arrival_pp(struct PCB ready_queue[], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else if (current_process.process_priority > new_process.process_priority) {
        current_process.remaining_bursttime = current_process.execution_endtime - timestamp;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        (*queue_cnt)++;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    } else {
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }
}

// Method to handle the completion of process execution for PP
struct PCB handle_process_completion_pp(struct PCB ready_queue[], int *queue_cnt, int timestamp) {
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    int next_process_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].process_priority < ready_queue[next_process_index].process_priority) {
            next_process_index = i;
        }
    }

    struct PCB next_process = ready_queue[next_process_index];
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.total_bursttime;
    next_process.remaining_bursttime = next_process.total_bursttime;

    for (int i = next_process_index; i < *queue_cnt - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }

    (*queue_cnt)--;
    return next_process;
}

// Method to handle the arrival of a process for SRTP
struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    // If there is no currently-running process
    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.total_bursttime;
        new_process.remaining_bursttime = new_process.total_bursttime;
        return new_process;
    }

    // If the new process does not have a shorter burst time
    if (new_process.total_bursttime >= current_process.remaining_bursttime) {
        new_process.execution_starttime = 0;
        new_process.execution_endtime = 0;
        new_process.remaining_bursttime = new_process.total_bursttime;
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;
        return current_process;
    }

    // If the new process has a shorter burst time
    current_process.remaining_bursttime -= (timestamp - current_process.execution_starttime);
    current_process.execution_starttime = 0;
    current_process.execution_endtime = 0;
    ready_queue[*queue_cnt] = current_process;
    (*queue_cnt)++;

    new_process.execution_starttime = timestamp;
    new_process.execution_endtime = timestamp + new_process.total_bursttime;
    new_process.remaining_bursttime = new_process.total_bursttime;

    return new_process;
}


// Method to handle the completion of process execution for SRTP
struct PCB handle_process_completion_srtp(struct PCB ready_queue[], int *queue_cnt, int timestamp) {
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    int next_process_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].remaining_bursttime < ready_queue[next_process_index].remaining_bursttime) {
            next_process_index = i;
        }
    }

    struct PCB next_process = ready_queue[next_process_index];
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    for (int i = next_process_index; i < *queue_cnt - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }

    (*queue_cnt)--;
    return next_process;
}

// Method to handle the arrival of a process for RR
struct PCB handle_process_arrival_rr(struct PCB ready_queue[], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {
    new_process.remaining_bursttime = new_process.total_bursttime;

    if (current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + ((new_process.remaining_bursttime > time_quantum) ? time_quantum : new_process.remaining_bursttime);
        return new_process;
    } else {
        // Add the new process to the end of the ready queue
        ready_queue[*queue_cnt] = new_process;
        (*queue_cnt)++;

        return current_process;
    }
}



// Method to handle the completion of process execution for RR
struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    if (*queue_cnt == 0) {
        return NULLPCB;
    }

    int earliest_arrival_index = 0;
    for (int i = 1; i < *queue_cnt; i++) {
        if (ready_queue[i].arrival_timestamp < ready_queue[earliest_arrival_index].arrival_timestamp) {
            earliest_arrival_index = i;
        }
    }

    struct PCB next_process = ready_queue[earliest_arrival_index];
    next_process.execution_starttime = timestamp;
    int execution_time = time_quantum < next_process.remaining_bursttime ? time_quantum : next_process.remaining_bursttime;
    next_process.execution_endtime = timestamp + execution_time;

    // Remove the selected process from the ready queue
    for (int i = earliest_arrival_index; i < *queue_cnt - 1; i++) {
        ready_queue[i] = ready_queue[i + 1];
    }
    (*queue_cnt)--;

    return next_process;
}

