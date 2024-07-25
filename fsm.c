/* Simple Real Time code for DOS (Open Watcom C). 
Includes rudimentary finite state machines for each task.
Can (and should) easily be modified to proper control of task states with signals (not manually).
License: GNU GPL v.3
by Lukasz Olczyk
*/

#include <stdio.h>
#include <dos.h>
#include <conio.h>

// Constants for task periods in ticks (assuming each tick is ~55ms in DOS)
#define TASK1_PERIOD 18  // Approximately 1 second
#define TASK2_PERIOD 36  // Approximately 2 seconds

// States for Task 1
typedef enum {
    TASK1_STATE_INIT,
    TASK1_STATE_RUNNING,
    TASK1_STATE_COMPLETE
} task1_state_t;

// States for Task 2
typedef enum {
    TASK2_STATE_INIT,
    TASK2_STATE_RUNNING,
    TASK2_STATE_COMPLETE
} task2_state_t;

// Initial states for tasks
task1_state_t task1_state = TASK1_STATE_INIT;
task2_state_t task2_state = TASK2_STATE_INIT;

// Global tick count
volatile unsigned long tick_count = 0;

// Task control block
typedef struct {
    void (*task_func)();
    unsigned long period;
    unsigned long next_run;
    void *state; // Pointer to task-specific state
} task_t;

// Task 1: Runs every second
void task1(task1_state_t *state) {
    switch (*state) {
        case TASK1_STATE_INIT:
            printf("Task 1 INIT at tick %lu\n", tick_count);
            //*state = TASK1_STATE_RUNNING;
            break;
        case TASK1_STATE_RUNNING:
            printf("Task 1 RUNNING at tick %lu\n", tick_count);
            *state = TASK1_STATE_COMPLETE;
            break;
        case TASK1_STATE_COMPLETE:
            printf("Task 1 COMPLETE at tick %lu\n", tick_count);
            *state = TASK1_STATE_INIT;  // Reset state for the example
            break;
    }
}

// Task 2: Runs every two seconds
void task2(task2_state_t *state) {
    switch (*state) {
        case TASK2_STATE_INIT:
            printf("                              Task 2 INIT at tick %lu\n", tick_count);
            *state = TASK2_STATE_RUNNING;
            break;
        case TASK2_STATE_RUNNING:
            printf("                              Task 2 RUNNING at tick %lu\n", tick_count);
            *state = TASK2_STATE_COMPLETE;
			task1_state = TASK1_STATE_RUNNING;	// Directly change state of task1 - better way to achieve it would be to send signal to it.
            break;
        case TASK2_STATE_COMPLETE:
            printf("                              Task 2 COMPLETE at tick %lu\n", tick_count);
            *state = TASK2_STATE_INIT;  // Reset state for the example
            break;
    }
}



// Wrapper functions for tasks
void task1_wrapper() {
    task1(&task1_state);
}

void task2_wrapper() {
    task2(&task2_state);
}

// Array of tasks
task_t tasks[] = {
    { task1_wrapper, TASK1_PERIOD, 0, &task1_state },
    { task2_wrapper, TASK2_PERIOD, 0, &task2_state },
};

// Number of tasks
#define NUM_TASKS (sizeof(tasks) / sizeof(task_t))

// Interrupt Service Routine (ISR) for the timer
void interrupt (*old_isr)();

void interrupt new_isr() {
    int i;
    tick_count++;
    for (i = 0; i < NUM_TASKS; i++) {
        if (tick_count >= tasks[i].next_run) {
            tasks[i].task_func();
            tasks[i].next_run += tasks[i].period;
        }
    }
    // Call the original timer ISR to maintain system clock
    old_isr();
}

int main() {
    // Save the old ISR
    old_isr = _dos_getvect(0x08);
    // Set the new ISR
    _dos_setvect(0x08, new_isr);

    printf("Real-Time System started. Press any key to exit...\n");

    // Main loop
    while (!kbhit()) {
        // Idle loop
    }

    // Restore the old ISR
    _dos_setvect(0x08, old_isr);
    printf("Real-Time System stopped.\n");

    return 0;
}
