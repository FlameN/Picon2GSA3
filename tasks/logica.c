// SPI_BUS
#include <stdlib.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
//System
#include "board.h"
#include "memman.h"
#include "twi.h"
#include "spi.h"
#include "rtclock.h"
#include "usermemory.h"
#include "dbg.h"
// Tasks
#include "logica.h"
#include "gprscon.h"
#include "rs485.h"
#include "spibus.h"
#include "usbtask.h"
#include "usart1.h"

#include "translator.h"

#define vSpibusTask_PRIORITY            ( tskIDLE_PRIORITY + 4 )
#define vRs485Task_PRIORITY             ( tskIDLE_PRIORITY + 3 )
#define vUsart1Task_PRIORITY             ( tskIDLE_PRIORITY + 2 )
#define vUsbTask_PRIORITY               ( tskIDLE_PRIORITY + 1 )

void SwitchLeds();

//sio_fifo
void DoTestProgramm();
/*
int GetDiskretFromRam(int i)
{
	return ((RAM.dwords[i / 16] >> (i % 16)) & 0x1);
}

void SetRelayToRam(int i, int vol)
{
	if (vol != 0)
	{
		RAM.dwords[i / 16 + 4] |= (1 << (i % 16));
	}
	else
	{
		RAM.dwords[i / 16 + 4] &= ~(1 << (i % 16));
	}
}
*/
#define GETANDDISKRET(i) ((((GetDiskretFromRam(i) & GetDiskretFromRam(i + 11))) \
							& GetDiskretFromRam(i + 22) & GetDiskretFromRam(i + 33)))

void vLogicaTask(void *pvParameters)
{
	FLASHMEM *pxFlash;
	(void) pvParameters;
	dbgInit();
	dbgmessage(	"\n\r=====================================\n\r= Albert shklovich DBG WINDOW ==\n\r=====================================");
	dbgStackFreeSize("\nLogica Task");
	dbgmessage("DbgInit ...ok ");
	dbgmessage("Mem Init ...");
	MemInit();

	dbgmessage("ok ");
	dbgmessage("TWI Init ...");
	AT91F_TWI_Init();
	dbgmessage("ok ");
	dbgmessage("SPI Init ...");
	SPI_Init();
	dbgmessage("ok ");
	dbgmessage("RTC Init ...");
	rtcInit();
	dbgmessage("ok ");
	dbgmessage("Starting tasks...");

	xTaskCreate(vGprsConTask, (const signed portCHAR * const ) "vGprsCon",
			vGprsConTask_STACK_SIZE, NULL, vGprsConTask_PRIORITY, (xTaskHandle *) NULL);

	xTaskCreate(vRs485Task, (const signed portCHAR * const ) "vRs485",
			vRs485Task_STACK_SIZE, NULL, vRs485Task_PRIORITY,
			(xTaskHandle *) NULL);
#ifndef COM1_AS_DEBUGPORT
	xTaskCreate(vUsart1Task, (const signed portCHAR * const ) "vUsart1",
			vUsart1Task_STACK_SIZE, NULL, vUsart1Task_PRIORITY, (xTaskHandle *) NULL);
#endif

	xTaskCreate(vUsbTask, (const signed portCHAR * const ) "vUsb",
			vUsbTask_STACK_SIZE, NULL, vUsbTask_PRIORITY, (xTaskHandle *) NULL);

	xTaskCreate(vSpibusTask, (const signed portCHAR * const ) "vSpibus",
			vSpibusTask_STACK_SIZE, NULL, vSpibusTask_PRIORITY,
			(xTaskHandle *) NULL);

	dbgHeapFreeSize();
	dbgStackFreeSize("Logica Task");
	dbgmessage("Starting Translator ");

#ifdef TESTBOARD
	SetRelayToRam(3, 1); // initial state of rele
#endif
	while (1)
	{

        AT91C_BASE_WDTC->WDTC_WDCR = 0xA5000001;
#ifdef TESTBOARD
		SwitchLeds();
		vTaskDelay(900);
		DoTestProgramm();

#else
		AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED2);
		DoProgram();
#endif
		vTaskDelay(100);
	}

}
void DoTestProgramm()
{
	for (int i = 0; i < 4; i++)
	{
		if (GETANDDISKRET(i) != 0)
		{
			SetRelayToRam(i, 1);
		}
		else
		{
			SetRelayToRam(i, 0);
		}
	}

}
void SwitchLeds()
{
	static int cnt = 0;

	switch (cnt % 4)
	{
	case 0:
		AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED1);
		break;
	case 1:
		AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED2);
		break;
	case 2:
		AT91F_PIO_ClearOutput(AT91C_BASE_PIOB, LED1);
		break;
	case 3:
		AT91F_PIO_ClearOutput(AT91C_BASE_PIOB, LED2);
		break;
	}
	cnt++;
}
