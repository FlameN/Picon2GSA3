#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "usart1.h"
#include "rvnet.h"
#include "board.h"

RVNET_DATATYPE us1size;
uint8 us1buf[256];



void vUsart1Task(void *pvParameters)
{
	/* The parameters are not used. */
	(void) pvParameters;

	AT91F_USART_OPEN(AT91_USART_COM1_ID, 115200, AT91C_US_ASYNC_MODE);
	while (1)
	{
		us1size = AT91F_USART_RECEIVE(&COM1, us1buf, 256, 5000);
		if (us1size != 0)
		{

			us1size = RVnetSlaveProcess(us1buf, us1size, 1);

			AT91F_USART_SEND(&COM1, us1buf, us1size);
		}
		vTaskDelay(1);
	}
}
;

