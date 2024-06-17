#ifndef OSLABS_H
#define OSLABS_H


struct RCB {
    int request_id;
    int arrival_timestamp;
    int cylinder;
    int address;
    int process_id;
};

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp);
struct RCB handle_request_completion_fcfs(struct RCB request_queue[], int *queue_cnt);

struct RCB handle_request_arrival_sstf(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp);
struct RCB handle_request_completion_sstf(struct RCB request_queue[], int *queue_cnt, int current_cylinder);

struct RCB handle_request_arrival_look(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp);
struct RCB handle_request_completion_look(struct RCB request_queue[], int *queue_cnt, int current_cylinder, int scan_direction);

#endif
