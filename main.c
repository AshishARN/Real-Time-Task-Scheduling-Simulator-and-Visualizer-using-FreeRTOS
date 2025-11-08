#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h" // The header file for Semaphores and Mutexes

/* Handle for the mutex that protects the UART */
SemaphoreHandle_t xUartMutex;

/* The memory-mapped address for the UART0 data register */
volatile unsigned int * const UART0_DR = (unsigned int *)0x4000C000;

/*
 * A printing function that is "task safe" because it uses the mutex.
 */
void safe_print(const char *s) {
    // Attempt to take the mutex. Block indefinitely until it's available.
    if (xSemaphoreTake(xUartMutex, portMAX_DELAY) == pdTRUE) {
        // We now have exclusive access to the UART.
        while(*s != '\0') {
            *UART0_DR = (unsigned int)(*s);
            s++;
        }
        // Release the mutex so another task can use the UART.
        xSemaphoreGive(xUartMutex);
    }
}

/*
 * --- TASKS ---
 */

/* Worker Task 1 */
void vWorkerTask1(void *pvParameters) {
    const char *pcTaskMessage = "Worker 1 says: part 1, part 2, part 3.\r\n";
    // Pre-calculate the delay in ticks to avoid 64-bit division.
    // (250ms * 1000 ticks/sec) / 1000 ms/sec = 250 ticks
    const TickType_t xDelay = 250;

    for (;;) {
        // Print out the complete message.
        safe_print(pcTaskMessage);

        // Wait for a fixed period.
        vTaskDelay(xDelay);
    }
}

/* Worker Task 2 */
void vWorkerTask2(void *pvParameters) {
    const char *pcTaskMessage = "Worker 2 says: part 1, part 2, part 3.\r\n";
    // Pre-calculate the delay in ticks.
    // (300ms * 1000 ticks/sec) / 1000 ms/sec = 300 ticks
    const TickType_t xDelay = 300;

    for (;;) {
        // Print out the complete message.
        safe_print(pcTaskMessage);

        // Wait for a fixed period.
        vTaskDelay(xDelay);
    }
}


/*
 * --- MAIN ---
 */
int main(void) {
    // Create the mutex before any tasks that might use it.
    xUartMutex = xSemaphoreCreateMutex();

    // Check the mutex was created successfully.
    if (xUartMutex != NULL) {
        // Create the two worker tasks with the same priority.
        xTaskCreate(vWorkerTask1, "Worker 1", configMINIMAL_STACK_SIZE,
                    NULL, 1, NULL);

        xTaskCreate(vWorkerTask2, "Worker 2", configMINIMAL_STACK_SIZE,
                    NULL, 1, NULL);

        // Start the scheduler.
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached. */
    for (;;);
    return 0;
}