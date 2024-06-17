#include "oslabs.h"
#include <limits.h>

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int min_difference = INT_MAX;
    int best_fit_index = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            int size_difference = memory_map[i].segment_size - request_size;
            if (size_difference < min_difference) {
                min_difference = size_difference;
                best_fit_index = i;
            }
        }
    }

    if (best_fit_index == -1) {
        struct MEMORY_BLOCK null_block = {0, 0, 0, 0};
        return null_block;
    }

    if (min_difference == 0) {
        memory_map[best_fit_index].process_id = process_id;
        return memory_map[best_fit_index];
    } else {
        struct MEMORY_BLOCK allocated_block = {memory_map[best_fit_index].start_address,
                                               memory_map[best_fit_index].start_address + request_size - 1,
                                               request_size,
                                               process_id};

        struct MEMORY_BLOCK remaining_block = {allocated_block.end_address + 1,
                                               memory_map[best_fit_index].end_address,
                                               memory_map[best_fit_index].segment_size - request_size,
                                               0};

        memory_map[best_fit_index] = allocated_block;
        for (int i = *map_cnt; i > best_fit_index + 1; i--) {
            memory_map[i] = memory_map[i - 1];
        }
        memory_map[best_fit_index + 1] = remaining_block;
        (*map_cnt)++;
        
        return allocated_block;
    }
}

struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int first_fit_index = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            first_fit_index = i;
            break;
        }
    }

    if (first_fit_index == -1) {
        struct MEMORY_BLOCK null_block = {0, 0, 0, 0};
        return null_block;
    }

    int size_difference = memory_map[first_fit_index].segment_size - request_size;

    if (size_difference == 0) {
        memory_map[first_fit_index].process_id = process_id;
        return memory_map[first_fit_index];
    } else {
        struct MEMORY_BLOCK allocated_block = {memory_map[first_fit_index].start_address,
                                               memory_map[first_fit_index].start_address + request_size - 1,
                                               request_size,
                                               process_id};

        struct MEMORY_BLOCK remaining_block = {allocated_block.end_address + 1,
                                               memory_map[first_fit_index].end_address,
                                               memory_map[first_fit_index].segment_size - request_size,
                                               0};

        memory_map[first_fit_index] = allocated_block;
        for (int i = *map_cnt; i > first_fit_index + 1; i--) {
            memory_map[i] = memory_map[i - 1];
        }
        memory_map[first_fit_index + 1] = remaining_block;
        (*map_cnt)++;
        
        return allocated_block;
    }
}

struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id) {
    int max_difference = -1;
    int worst_fit_index = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
            int difference = memory_map[i].segment_size - request_size;
            if (difference > max_difference) {
                max_difference = difference;
                worst_fit_index = i;
            }
        }
    }

    if (worst_fit_index == -1) {
        struct MEMORY_BLOCK nullblock = {0, 0, 0, 0};
        return nullblock;
    }

    // Update memory_map
    struct MEMORY_BLOCK allocated_block = memory_map[worst_fit_index];
    if (memory_map[worst_fit_index].segment_size == request_size) {
        memory_map[worst_fit_index].process_id = process_id;
    } else {
        int original_end_address = memory_map[worst_fit_index].end_address;
        
        memory_map[worst_fit_index].end_address = memory_map[worst_fit_index].start_address + request_size - 1;
        memory_map[worst_fit_index].segment_size = request_size;
        memory_map[worst_fit_index].process_id = process_id;

        // Add the new free block to the memory_map
        struct MEMORY_BLOCK new_free_block = {
            memory_map[worst_fit_index].end_address + 1,
            original_end_address,
            original_end_address - memory_map[worst_fit_index].end_address,
            0
        };

        // Shift the memory_map elements to the right to insert the new_free_block
        for (int i = *map_cnt; i > worst_fit_index + 1; i--) {
            memory_map[i] = memory_map[i - 1];
        }
        memory_map[worst_fit_index + 1] = new_free_block;
        (*map_cnt)++;
    }

    // Return the allocated block
    return allocated_block;
}

struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt, int process_id, int last_address) {
    int next_fit_index = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size && memory_map[i].start_address >= last_address) {
            next_fit_index = i;
            break;
        }
    }

    if (next_fit_index == -1) {
        for (int i = 0; i < *map_cnt; i++) {
            if (memory_map[i].process_id == 0 && memory_map[i].segment_size >= request_size) {
                next_fit_index = i;
                break;
            }
        }
    }

    if (next_fit_index == -1) {
        struct MEMORY_BLOCK null_block = {0, 0, 0, 0};
        return null_block;
    }

    int size_difference = memory_map[next_fit_index].segment_size - request_size;

    if (size_difference == 0) {
        memory_map[next_fit_index].process_id = process_id;
        return memory_map[next_fit_index];
    } else {
        struct MEMORY_BLOCK allocated_block = {memory_map[next_fit_index].start_address,
                                               memory_map[next_fit_index].start_address + request_size - 1,
                                               request_size,
                                               process_id};

        struct MEMORY_BLOCK remaining_block = {allocated_block.end_address + 1,
                                               memory_map[next_fit_index].end_address,
                                               memory_map[next_fit_index].segment_size - request_size,
                                               0};

        memory_map[next_fit_index] = allocated_block;
        for (int i = *map_cnt; i > next_fit_index + 1; i--) {
            memory_map[i] = memory_map[i - 1];
        }
        memory_map[next_fit_index + 1] = remaining_block;
        (*map_cnt)++;
        
        return allocated_block;
    }
}

void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX], int *map_cnt) {
    int freed_block_index = -1;

    for (int i = 0; i < *map_cnt; i++) {
        if (memory_map[i].start_address == freed_block.start_address && memory_map[i].end_address == freed_block.end_address) {
            memory_map[i].process_id = 0;
            freed_block_index = i;
            break;
        }
    }

    if (freed_block_index == -1) {
        return; // Freed block not found in the memory_map
    }

    // Attempt to merge free blocks
    if (freed_block_index > 0 && memory_map[freed_block_index - 1].process_id == 0) {
        // Merge with the previous block if it's free
        memory_map[freed_block_index - 1].end_address = memory_map[freed_block_index].end_address;
        memory_map[freed_block_index - 1].segment_size += memory_map[freed_block_index].segment_size;

        for (int j = freed_block_index; j < *map_cnt - 1; j++) {
            memory_map[j] = memory_map[j + 1];
        }
        (*map_cnt)--;
        freed_block_index--;
    }

    if (freed_block_index < *map_cnt - 1 && memory_map[freed_block_index + 1].process_id == 0) {
        // Merge with the next block if it's free
        memory_map[freed_block_index].end_address = memory_map[freed_block_index + 1].end_address;
        memory_map[freed_block_index].segment_size += memory_map[freed_block_index + 1].segment_size;

        for (int j = freed_block_index + 1; j < *map_cnt - 1; j++) {
            memory_map[j] = memory_map[j + 1];
        }
        (*map_cnt)--;
    }
}

