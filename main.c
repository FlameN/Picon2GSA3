
//********************************************************
//    Project      :
//    Version      :
//    Date         :
//    Author       : Kopachevsky Yuri
//    Company      : BEMN
//    Discription  :
//    Tools        : GNUARM compiller
//*********************************************************

/* Standard includes. */
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "logica.h"
#include "rtclock.h"
#include "board.h"

//uint16 testbyte[1024*7];
//=============================================
void prvSetupHardware(void);
extern void mapInit();
int main(void)
    {
	//for(int i=0;i<1024*10;i++){testbyte[i]=i;i=testbyte[i];}
    prvSetupHardware();
    xTaskCreate(vLogicaTask, (const signed portCHAR * const ) "Logica",
    		    vLogicaTask_STACK_SIZE, NULL, vLogicaTask_PRIORITY,
    		    (xTaskHandle *) NULL);
    /* Start the scheduler. */
    vTaskStartScheduler();
    while(1);
    return 0;
    }
//=================================================================================================
//
//=================================================================================================
void prvSetupHardware(void)
    {
    /* When using the JTAG debugger the hardware is not always initialised to
     the correct default state.  This line just ensures that this does not
     cause all interrupts to be masked at the start. */
    AT91C_BASE_AIC->AIC_EOICR = 0;

    /* Most setup is performed by the low level init function called from the
     startup asm file. */

    /* Configure the PIO Lines corresponding to LED1 to LED4 to be outputs as
     */
    //AT91F_PIO_CfgInput(AT91C_BASE_PIOB, DISIN_MASK );
    /* Enable the peripheral clock. */
    //     AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOA;
    AT91C_BASE_PMC->PMC_PCER = 1 << AT91C_ID_PIOB;
    AT91F_PIO_CfgOutput(AT91C_BASE_PIOB, OUTPIN_MASK);
    AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, MODEM_RST);

    }
void vApplicationIdleHook(void)
    {

    }
extern void GprsIdleIncMSec();
void vApplicationTickHook(void)
    {
    rtcIncrementSoftClockMs();
    atcIncrementCounter();
    GprsIdleIncMSec();
    }
void vApplicationMallocFailedHook()
{

}
void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
    {
	while(1);
    }
