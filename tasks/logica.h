#ifndef _LOGICATASK
#define _LOGICATASK

#define vLogicaTask_STACK_SIZE	500
#define vLogicaTask_PRIORITY            ( tskIDLE_PRIORITY + 2 )
extern void vLogicaTask(void *pvParameters )  __attribute__((naked));

#endif
