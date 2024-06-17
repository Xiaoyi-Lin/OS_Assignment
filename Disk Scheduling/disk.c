#include "oslabs.h"
#include <stdlib.h>
#define INT_MAX 100000
#define NULLRCB (struct RCB){0, 0, 0, 0, 0}

struct RCB handle_request_arrival_fcfs(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == NULLRCB.request_id) {
        return new_request; // Disk is free, return the new_request as the current request
    } else {
        request_queue[*queue_cnt] = new_request; // Add new_request to the request_queue
        (*queue_cnt)++;
        return current_request; // Return the currently-serviced request
    }
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[], int *queue_cnt) {
    if (*queue_cnt == 0) {
        return NULLRCB; // Request queue is empty, return NULLRCB
    } else {
        int earliest_index = 0;

        // Find the request with the earliest arrival time
        for (int i = 1; i < *queue_cnt; i++) {
            if (request_queue[i].arrival_timestamp < request_queue[earliest_index].arrival_timestamp) {
                earliest_index = i;
            }
        }

        struct RCB next_request = request_queue[earliest_index];

        // Remove the selected request from the request_queue and shift remaining requests
        for (int i = earliest_index; i < *queue_cnt - 1; i++) {
            request_queue[i] = request_queue[i + 1];
        }

        (*queue_cnt)--;

        return next_request; // Return the request with the earliest arrival time
    }
}

struct RCB handle_request_arrival_sstf(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == NULLRCB.request_id) {
        return new_request; // Disk is free, return the newly-arriving request
    } else {
        // Add the newly-arriving request to the request queue
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;

        return current_request; // Disk is busy, return the currently-serviced request
    }
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[], int *queue_cnt, int current_cylinder) {
    if (*queue_cnt == 0) {
        return NULLRCB; // Request queue is empty, return NULLRCB
    }

    int closest_distance = INT_MAX;
    int closest_index = -1;

    for (int i = 0; i < *queue_cnt; i++) {
        int distance = abs(current_cylinder - request_queue[i].cylinder);
        if (distance < closest_distance) {
            closest_distance = distance;
            closest_index = i;
        } else if (distance == closest_distance && request_queue[i].arrival_timestamp < request_queue[closest_index].arrival_timestamp) {
            closest_index = i;
        }
    }

    struct RCB next_request = request_queue[closest_index];

    // Remove the selected request from the request queue
    for (int i = closest_index; i < *queue_cnt - 1; i++) {
        request_queue[i] = request_queue[i + 1];
    }
    (*queue_cnt)--;

    return next_request;
}

struct RCB handle_request_arrival_look(struct RCB request_queue[], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.process_id == NULLRCB.process_id) {
        return new_request; // Disk is free, return newly-arriving request
    } else {
        request_queue[*queue_cnt] = new_request;
        (*queue_cnt)++;
        return current_request; // Disk is busy, return currently-serviced request after adding new request to the queue
    }
}

struct RCB handle_request_completion_look(struct RCB request_queue[], int *queue_cnt, int current_cylinder, int scan_direction) {
    if (*queue_cnt == 0) {
        return NULLRCB; // Queue is empty, return NULLRCB
    }

    int next_request_index = -1;
    int min_difference = INT_MAX;

    for (int i = 0; i < *queue_cnt; i++) {
        int cylinder_diff = request_queue[i].cylinder - current_cylinder;

        if (request_queue[i].cylinder == current_cylinder) {
            if (next_request_index == -1 || request_queue[i].arrival_timestamp < request_queue[next_request_index].arrival_timestamp) {
                next_request_index = i;
            }
        } else if (scan_direction == 1 && cylinder_diff > 0 && cylinder_diff < min_difference) {
            min_difference = cylinder_diff;
            next_request_index = i;
        } else if (scan_direction == 0 && cylinder_diff < 0 && -cylinder_diff < min_difference) {
            min_difference = -cylinder_diff;
            next_request_index = i;
        }
    }

    if (next_request_index == -1) {
        min_difference = INT_MAX;

        for (int i = 0; i < *queue_cnt; i++) {
            int cylinder_diff = abs(request_queue[i].cylinder - current_cylinder);

            if (cylinder_diff < min_difference) {
                min_difference = cylinder_diff;
                next_request_index = i;
            }
        }
    }

    struct RCB next_request = request_queue[next_request_index];

    for (int i = next_request_index; i < *queue_cnt - 1; i++) {
        request_queue[i] = request_queue[i + 1];
    }

    (*queue_cnt)--;
    return next_request;
}
