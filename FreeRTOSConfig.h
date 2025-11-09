#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 *----------------------------------------------------------*/

/* Essential hardware-independent settings */
#define configUSE_PREEMPTION                    1
#define configUSE_IDLE_HOOK                     0  // Set to 1 if you want to use the idle hook
#define configUSE_TICK_HOOK                     0  // Set to 1 if you want to use the tick hook
#define configUSE_MUTEXES                       1
#define configCPU_CLOCK_HZ                      ( ( unsigned long ) 25000000 )
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES                    ( 5 )
#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 15 * 1024 ) ) // Increased heap size for more tasks
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_16_BIT_TICKS                  0

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		            0
#define configMAX_CO_ROUTINE_PRIORITIES         ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a library function that
might create a critical section. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values). */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS standard names. */
#define vPortSVCHandler    SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler


/* ----------------------------------------------------------
 * TRACE HOOKS AND IMPLEMENTATION
 * ----------------------------------------------------------*/
#define configUSE_TRACE_FACILITY        1 // Enable the trace facility

// We need a forward declaration for the print function used in the macro
void print_uart0(const char *s);

// Simple integer to string conversion
static inline void itoa(int val, char* buf) {
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    int i = 0;
    int is_negative = val < 0;
    if (is_negative) val = -val;

    while (val != 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    if (is_negative) buf[i++] = '-';
    
    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char temp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = temp;
    }
    buf[i] = '\0';
}

// The implementation of our trace macro
#define TRACE_TASK_SWITCHED_IN_MACRO() \
    do { \
        char buffer[16]; \
        itoa(xTaskGetTickCount(), buffer); \
        print_uart0(buffer); \
        print_uart0(","); \
        print_uart0(pcTaskGetName(NULL)); \
        print_uart0(",IN\r\n"); \
    } while (0)

#define traceTASK_SWITCHED_IN()         TRACE_TASK_SWITCHED_IN_MACRO()


#endif /* FREERTOS_CONFIG_H */