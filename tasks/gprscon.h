#ifndef _GPRSCONTASK
#define _GPRSCONTASK

#define vGprsConTask_STACK_SIZE		530
#define vGprsConTask_PRIORITY           ( tskIDLE_PRIORITY + 2 )



extern void vGprsConTask(void *pvParameters )  __attribute__((naked));

#endif
