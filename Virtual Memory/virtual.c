#include "oslabs.h"
#include <limits.h>

#include <stdbool.h>
#include <stdio.h>

int find_fifo_replacement(struct PTE page_table[TABLEMAX], int table_cnt) {
    int replacement_index = -1;
    int min_arrival_timestamp = INT_MAX;

    for (int i = 0; i < table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].arrival_timestamp < min_arrival_timestamp) {
            min_arrival_timestamp = page_table[i].arrival_timestamp;
            replacement_index = i;
        }
    }

    return replacement_index;
}

int process_page_access_fifo(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    // Check if the page is already in memory
    if (page_table[page_number].is_valid) {
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count++;
        return page_table[page_number].frame_number;
    }

    // If the page is not in memory and there are free frames
    if (*frame_cnt > 0) {
        int frame_number = frame_pool[--(*frame_cnt)];
        page_table[page_number].is_valid = 1;
        page_table[page_number].frame_number = frame_number;
        page_table[page_number].arrival_timestamp = current_timestamp;
        page_table[page_number].last_access_timestamp = current_timestamp;
        page_table[page_number].reference_count = 1;
        return frame_number;
    }

    // If the page is not in memory and there are no free frames, replace a page using FIFO policy
    int replacement_index = find_fifo_replacement(page_table, *table_cnt);
    int replaced_frame = page_table[replacement_index].frame_number;

    page_table[replacement_index].is_valid = 0;
    page_table[replacement_index].frame_number = -1;
    page_table[replacement_index].arrival_timestamp = -1;
    page_table[replacement_index].last_access_timestamp = -1;
    page_table[replacement_index].reference_count = -1;

    page_table[page_number].is_valid = 1;
    page_table[page_number].frame_number = replaced_frame;
    page_table[page_number].arrival_timestamp = current_timestamp;
    page_table[page_number].last_access_timestamp = current_timestamp;
    page_table[page_number].reference_count = 1;

    return replaced_frame;
}

int count_page_faults_fifo(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;
    int current_timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = reference_string[i];

        // Check if the page is already in memory
        if (page_table[page_number].is_valid) {
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count++;
        } else {
            // If the page is not in memory and there are free frames
            if (frame_cnt > 0) {
                int frame_number = frame_pool[--frame_cnt];
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;
            } else {
                // If the page is not in memory and there are no free frames, replace a page using FIFO policy
                int replacement_index = find_fifo_replacement(page_table, table_cnt);
                int replaced_frame = page_table[replacement_index].frame_number;

                page_table[replacement_index].is_valid = 0;
                page_table[replacement_index].frame_number = -1;
                page_table[replacement_index].arrival_timestamp = -1;
                page_table[replacement_index].last_access_timestamp = -1;
                page_table[replacement_index].reference_count = -1;

                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = replaced_frame;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;
            }
            page_faults++;
        }
        current_timestamp++;
    }

    return page_faults;
}

int process_page_access_lru(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    struct PTE current_page = page_table[page_number];

    // Check if the page is already in memory
    if (current_page.is_valid) {
        current_page.last_access_timestamp = current_timestamp;
        current_page.reference_count++;
        page_table[page_number] = current_page;
        return current_page.frame_number;
    }

    // Check if there is a free frame in the pool
    if (*frame_cnt > 0) {
        int frame_number = frame_pool[--(*frame_cnt)];
        current_page.is_valid = 1;
        current_page.frame_number = frame_number;
        current_page.arrival_timestamp = current_timestamp;
        current_page.last_access_timestamp = current_timestamp;
        current_page.reference_count = 1;
        page_table[page_number] = current_page;
        return frame_number;
    }

    // Replace a page using LRU policy
    int lru_index = -1;
    int min_last_access_timestamp = INT_MAX;
    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid && page_table[i].last_access_timestamp < min_last_access_timestamp) {
            min_last_access_timestamp = page_table[i].last_access_timestamp;
            lru_index = i;
        }
    }

    int replaced_frame_number = page_table[lru_index].frame_number;
    page_table[lru_index].is_valid = 0;
    page_table[lru_index].frame_number = -1;
    page_table[lru_index].arrival_timestamp = -1;
    page_table[lru_index].last_access_timestamp = -1;
    page_table[lru_index].reference_count = -1;

    current_page.is_valid = 1;
    current_page.frame_number = replaced_frame_number;
    current_page.arrival_timestamp = current_timestamp;
    current_page.last_access_timestamp = current_timestamp;
    current_page.reference_count = 1;
    page_table[page_number] = current_page;

    return replaced_frame_number;
}

int count_page_faults_lru(struct PTE page_table[TABLEMAX], int table_cnt, int refrence_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int faults = 0;
    int current_timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int page_number = refrence_string[i];

        // Check if the page is already in memory
        if (page_table[page_number].is_valid) {
            // Update the page's last access timestamp and reference count
            page_table[page_number].last_access_timestamp = current_timestamp;
            page_table[page_number].reference_count++;

        } else {
            // Check if there are any free frames
            if (frame_cnt > 0) {
                // Get a frame from the free frame pool
                int frame_number = frame_pool[--frame_cnt];

                // Update the page table entry for the new page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                // Increment the number of page faults
                faults++;

            } else {
                // Find the page with the smallest last access timestamp
                int lru_page_number = -1;
                int min_last_access_timestamp = INT_MAX;
                int min_arrival_timestamp = INT_MAX;

                for (int j = 0; j < table_cnt; j++) {
                    if (page_table[j].is_valid && page_table[j].last_access_timestamp < min_last_access_timestamp) {
                        lru_page_number = j;
                        min_last_access_timestamp = page_table[j].last_access_timestamp;
                        min_arrival_timestamp = page_table[j].arrival_timestamp;
                    } else if (page_table[j].is_valid && page_table[j].last_access_timestamp == min_last_access_timestamp && page_table[j].arrival_timestamp < min_arrival_timestamp) {
                        lru_page_number = j;
                        min_arrival_timestamp = page_table[j].arrival_timestamp;
                    }
                }

                // Invalidate the page table entry for the LRU page
                int frame_number = page_table[lru_page_number].frame_number;
                page_table[lru_page_number].is_valid = 0;
                page_table[lru_page_number].frame_number = -1;
                page_table[lru_page_number].arrival_timestamp = 0;
                page_table[lru_page_number].last_access_timestamp = 0;
                page_table[lru_page_number].reference_count = 0;

                // Update the page table entry for the new page
                page_table[page_number].is_valid = 1;
                page_table[page_number].frame_number = frame_number;
                page_table[page_number].arrival_timestamp = current_timestamp;
                page_table[page_number].last_access_timestamp = current_timestamp;
                page_table[page_number].reference_count = 1;

                // Increment the number of page faults
                faults++;
            }
        }

        // Increment the current timestamp
        current_timestamp++;
    }

    return faults;
}

int process_page_access_lfu(struct PTE page_table[TABLEMAX], int *table_cnt, int page_number, int frame_pool[POOLMAX], int *frame_cnt, int current_timestamp) {
    struct PTE current_page = page_table[page_number];

    // Check if the page is already in memory
    if (current_page.is_valid) {
        current_page.last_access_timestamp = current_timestamp;
        current_page.reference_count++;
        page_table[page_number] = current_page;
        return current_page.frame_number;
    }

    // Check if there is a free frame in the pool
    if (*frame_cnt > 0) {
        int frame_number = frame_pool[--(*frame_cnt)];
        current_page.is_valid = 1;
        current_page.frame_number = frame_number;
        current_page.arrival_timestamp = current_timestamp;
        current_page.last_access_timestamp = current_timestamp;
        current_page.reference_count = 1;
        page_table[page_number] = current_page;
        return frame_number;
    }

    // Replace a page using LFU policy
    int lfu_index = -1;
    int min_reference_count = INT_MAX;
    int min_arrival_timestamp = INT_MAX;
    for (int i = 0; i < *table_cnt; i++) {
        if (page_table[i].is_valid) {
            if (page_table[i].reference_count < min_reference_count) {
                min_reference_count = page_table[i].reference_count;
                min_arrival_timestamp = page_table[i].arrival_timestamp;
                lfu_index = i;
            } else if (page_table[i].reference_count == min_reference_count && page_table[i].arrival_timestamp < min_arrival_timestamp) {
                min_arrival_timestamp = page_table[i].arrival_timestamp;
                lfu_index = i;
            }
        }
    }

    int replaced_frame_number = page_table[lfu_index].frame_number;
    page_table[lfu_index].is_valid = 0;
    page_table[lfu_index].frame_number = -1;
    page_table[lfu_index].arrival_timestamp = -1;
    page_table[lfu_index].last_access_timestamp = -1;
    page_table[lfu_index].reference_count = -1;

    current_page.is_valid = 1;
    current_page.frame_number = replaced_frame_number;
    current_page.arrival_timestamp = current_timestamp;
    current_page.last_access_timestamp = current_timestamp;
    current_page.reference_count = 1;
    page_table[page_number] = current_page;

    return replaced_frame_number;
}

int count_page_faults_lfu(struct PTE page_table[TABLEMAX], int table_cnt, int reference_string[REFERENCEMAX], int reference_cnt, int frame_pool[POOLMAX], int frame_cnt) {
    int page_faults = 0;
    int current_timestamp = 1;

    for (int i = 0; i < reference_cnt; i++) {
        int logical_page_number = reference_string[i];
        struct PTE current_page = page_table[logical_page_number];

        // Check if the page is already in memory
        if (current_page.is_valid) {
            current_page.last_access_timestamp = current_timestamp;
            current_page.reference_count++;
            page_table[logical_page_number] = current_page;
        } else {
            // Process the page access using LFU policy
            int frame_number = process_page_access_lfu(page_table, &table_cnt, logical_page_number, frame_pool, &frame_cnt, current_timestamp);
            page_faults++;
        }
        current_timestamp++;
    }

    return page_faults;
}
