#include <stddef.h> // Required for size_t

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/*
 * These are our own implementations of the basic C library functions
 * that FreeRTOS requires. By providing them ourselves, we remove the

 * need to link against any standard C libraries.
 */
// void *memcpy(void *dest, const void *src, size_t n) {
//     char *dp = dest;
//     const char *sp = src;
//     while (n--)
//         *dp++ = *sp++;
//     return dest;
// }

// void *memset(void *s, int c, size_t n) {
//     unsigned char *p = s;
//     while (n--)
//         *p++ = (unsigned char)c;
//     return s;
// }

/* The memory-mapped address for the UART0 data register */
volatile unsigned int * const UART0_DR = (unsigned int *)0x4000C000;

/* Function to print a string to the UART */
void print_uart0(const char *s) {
    while(*s != '\0') {
        *UART0_DR = (unsigned int)(*s);
        s++;
    }
}

/*
 * --- TASKS ---
 */

/* Task A (High Priority) */
void vTaskA(void *pvParameters) {
    const char *pcTaskName = "Task A is running\r\n";
    const TickType_t xDelay = pdMS_TO_TICKS(500); // Delay for 500 milliseconds

    for (;;) {
        print_uart0(pcTaskName);
        vTaskDelay(xDelay); // Block this task for a fixed period
    }
}

/* Task B (Low Priority) */
void vTaskB(void *pvParameters) {
    const char *pcTaskName = "Task B is running\r\n";
    //const TickType_t xDelay = pdMS_TO_TICKS(300);

    for (;;) {
        print_uart0(pcTaskName);
        //vTaskDelay(xDelay);
        // This task does not block, it just runs in a tight loop.
    }
}


/*
 * --- MAIN ---
 */
int main(void) {
    /* Create Task A */
    xTaskCreate(vTaskA,               /* Function that implements the task. */
                "Task A",             /* Text name for the task. */
                configMINIMAL_STACK_SIZE, /* Stack size in words, not bytes. */
                NULL,                 /* Parameter passed into the task. */
                2,                    /* Priority at which the task is created. */
                NULL);                /* Used to pass out the created task's handle. */

    /* Create Task B */
    xTaskCreate(vTaskB,
                "Task B",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,                    /* Lower priority than Task A */
                NULL);

    print_uart0("Scheduler starting...\r\n");

    /* Start the scheduler. */
    vTaskStartScheduler();

    /*
     * The scheduler has been started, so the program should never reach this
     * point. If it does, it's likely due to insufficient heap memory for
     * the idle task to be created.
     */
    for (;;) {
        // Loop forever.
    }

    return 0;
}

// #include <stddef.h>

// // void *memcpy(void *dest, const void *src, size_t n) {
// //     char *dp = dest;
// //     const char *sp = src;
// //     while (n--)
// //         *dp++ = *sp++;
// //     return dest;
// // }

// // void *memset(void *s, int c, size_t n) {
// //     unsigned char *p = s;
// //     while (n--)
// //         *p++ = (unsigned char)c;
// //     return s;
// // }
// // Define the memory-mapped address for the UART0 data register
// volatile unsigned int * const UART0_DR = (unsigned int *)0x4000C000;

// // Function to print a string to the UART
// void print_uart0(const char *s) {
//     while(*s != '\0') { // Loop until the end of the string
//         *UART0_DR = (unsigned int)(*s); // Write the character to the UART
//         s++;
//     }
// }

// // Entry point of our application
// int main(void) {
//     print_uart0("Hello from QEMU!\n");
//     // while(1) {
//     //     *UART0_DR = (unsigned int)('X');
//     // }

//     // Infinite loop to halt the processor
//     while(1);

//     return 0;
// }