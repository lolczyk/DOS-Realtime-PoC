// Simple Real Time code for DOS (Open Watcom C). Includes scheduling.
// License: GNU GPL v.3
// by Lukasz Olczyk

#include <stdio.h>
#include <dos.h>
#include <conio.h>

// Constants for task periods in ticks (assuming each tick is ~55ms in DOS)
#define TASK1_PERIOD 18  // Approximately 1 second
#define TASK2_PERIOD 36  // Approximately 2 seconds

// Global tick count
volatile unsigned long tick_count = 0;

// Task control block
typedef struct {
    void (*task_func)();
    unsigned long period;
    unsigned long next_run;
} task_t;

// Task 1: Runs every second
void task1() {
    printf("Task 1 executed at tick %lu\n", tick_count);
}

// Task 2: Runs every two seconds
void task2() {
    printf("Task 2 executed at tick %lu\n", tick_count);
}

// Array of tasks
task_t tasks[] = {
    { task1, TASK1_PERIOD, 0 },
    { task2, TASK2_PERIOD, 0 },
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

    // Wait for key press to exit
    while (!kbhit()) {
        // Idle loop
    }

    // Restore the old ISR
    _dos_setvect(0x08, old_isr);
    printf("Real-Time System stopped.\n");
    
    return 0;
}
