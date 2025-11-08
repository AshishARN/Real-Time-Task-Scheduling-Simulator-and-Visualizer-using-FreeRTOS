#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Handle for the mutex */
SemaphoreHandle_t xUartMutex;

/* The memory-mapped address for the UART0 data register */
volatile unsigned int * const UART0_DR = (unsigned int *)0x4000C000;

/* A simple, non-blocking print function */
void print_uart0(const char *s) {
    while(*s != '\0') {
        *UART0_DR = (unsigned int)(*s);
        s++;
    }
}

/*
 * --- TASKS ---
 */

/* High-Priority Task H */
void vTaskHighPriority(void *pvParameters) {
    print_uart0("H: Task started, trying to get mutex...\r\n");
    if (xSemaphoreTake(xUartMutex, portMAX_DELAY) == pdTRUE) {
        print_uart0("H: >>> Task got the mutex! <<<\r\n");
        xSemaphoreGive(xUartMutex);
    }
    print_uart0("H: Task finished and is deleting itself.\r\n");
    vTaskDelete(NULL);
}

/* Medium-Priority Task M */
void vTaskMediumPriority(void *pvParameters) {
    print_uart0("M: Task started, now running in a loop.\r\n");
    for (;;) {
       // This loop will starve Task L if priority inversion occurs.
    }
}

/* Low-Priority Task L */
void vTaskLowPriority(void *pvParameters) {
    print_uart0("L: Task started, trying to get mutex...\r\n");
    if (xSemaphoreTake(xUartMutex, portMAX_DELAY) == pdTRUE) {
        print_uart0("L: >>> Task got the mutex! <<<\r\n");

        // --- THE FIX IS HERE ---
        // Suspend the scheduler to ensure both tasks are created
        // before any context switch can occur.
        vTaskSuspendAll();
        print_uart0("L: Scheduler suspended. Creating M and H...\r\n");
        xTaskCreate(vTaskMediumPriority, "Task M", configMINIMAL_STACK_SIZE,
                    NULL, 2, NULL); // Priority 2 (Medium)

        xTaskCreate(vTaskHighPriority, "Task H", configMINIMAL_STACK_SIZE,
                    NULL, 3, NULL); // Priority 3 (High)
        print_uart0("L: Resuming scheduler...\r\n");
        xTaskResumeAll();
        // The scheduler will now resume. Since Task H (P3) is now ready,
        // it will immediately preempt this task.


        // This code will only run AFTER the priority inversion scenario has
        // played out.
        print_uart0("L: Task is now doing some 'work' while holding the mutex...\r\n");

        for (long i = 0; i < 2000000; i++) { }

        print_uart0("L: Task finished work and is releasing the mutex.\r\n");
        xSemaphoreGive(xUartMutex);
    }
    print_uart0("L: Task finished and is deleting itself.\r\n");
    vTaskDelete(NULL);
}


/*
 * --- MAIN ---
 */
int main(void) {
    xUartMutex = xSemaphoreCreateMutex();

    if (xUartMutex != NULL) {
        xTaskCreate(vTaskLowPriority, "Task L", configMINIMAL_STACK_SIZE,
                    NULL, 1, NULL); // Priority 1 (Low)

        print_uart0("Scheduler starting... Task L will run first.\r\n");
        vTaskStartScheduler();
    }

    for (;;);
    return 0;
}