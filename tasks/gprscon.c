#include <stdlib.h>
#include <stdarg.h>

#include "typedef.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "gprscon.h"
#include "dbg.h"
#include "rvnet.h"

#include "board.h"
#include "modem.h"

#define GATESERVER_PORT            ( 4444)

#define PPP_AUTH_ENABLED        1


void vGprsConTask(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(10);
	}

}

