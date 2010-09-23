#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "typedef.h"
#include "board.h"

#define ADDRETURN 1;
#include "dbgupdc.h"
char strbuffer[128];

xSemaphoreHandle xdbgMutex;

void dbgInit()
{
	xdbgMutex = xSemaphoreCreateMutex();
	Dbgu_init(AT91C_DBGU_BAUD);
}

void dbgmessage(char *str)
{
	if (xdbgMutex != NULL)
	{
		if (xSemaphoreTake(xdbgMutex, (portTickType) 100) == pdTRUE)
		{

				AT91F_DBGU_TRANSMIT((unsigned char *) "Hello ", 6);
			AT91F_DBGU_TRANSMIT((unsigned char *) str, strlen(str));
			while (AT91F_DBGU_TCR() != 0)
			{
				vTaskDelay(1);/*wait for transmittion*/
			};
#ifdef ADDRETURN
			AT91F_DBGU_TRANSMIT((unsigned char *) "\n\r", 2);
			while (AT91F_DBGU_TCR() != 0)
			{
				vTaskDelay(1);/*wait for transmittion*/
			};
#endif
			xSemaphoreGive(xdbgMutex);
		}

	}

}
void dbgPrintArray(int bstart, int bend, unsigned char buff[])
{
	char *pxstrbuff = strbuffer;
	int len = bend - bstart;
	int i;
	int tmp;
	vsprintf(pxstrbuff, "\n Array %04d bytes:", &len);
	dbgmessage(strbuffer);

	for (i = bstart; i < bend; i++)
	{
		tmp = buff[i];
		if (i * 5 > 120)
			break;
		snprintf(pxstrbuff + i * 5, 120, "0x%02X,", tmp);

	}
	dbgmessage(strbuffer);

}

void strace(const char *tstr, __const char *__restrict format, ...)
{
	//char *pxstrbuff = strbuffer;
	//    va_list ap;
	//  va_start(ap, format);
	//  vsnprintf(pxstrbuff, 200, format, ap);
	//  va_end(ap);
	//  dbgmessage(strbuffer);
	dbgmessage(format);
}

void trace(__const char *__restrict format, ...)
{
	char *pxstrbuff = strbuffer;
	va_list ap;
	va_start(ap, format);
	vsnprintf(pxstrbuff, 120, format, ap);
	va_end(ap);
	dbgmessage(strbuffer);
}

void ppp_trace(int level, __const char *__restrict format, ...)
{
	char *pxstrbuff = strbuffer;
	va_list ap;
	va_start(ap, format);
	vsnprintf(pxstrbuff, 120, format, ap);
	va_end(ap);
	dbgmessage(strbuffer);
	if (level != 6)
	{
		dbgmessage("\r");
	} //LOG_DETAIL
}

void lvip_trace(int level, __const char *__restrict format, ...)
{
	char *pxstrbuff = strbuffer;
	va_list ap;
	va_start(ap, format);
	vsnprintf(pxstrbuff, 120, format, ap);
	va_end(ap);
	dbgmessage(strbuffer);
	if (level != 6)
	{
		dbgmessage("\r");
	}
}
void dbgStackFreeSize(char *str)
{
	unsigned portBASE_TYPE uxHighWaterMark;
	uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
	dbgmessage(str);
	trace(" - Stack Rem: %d \n\r", uxHighWaterMark);
}
void dbgHeapFreeSize()
{
	int heapSize = 0;
	heapSize = xPortGetFreeHeapSize();
	trace("\n\rHeap free  %d. \r\n", heapSize);
}
