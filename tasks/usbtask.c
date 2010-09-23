#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "usbtask.h"
#include "rvnet.h"
#include "usb_cdc.h"
#include "board.h"

struct _AT91S_CDC pCDC;
uint8 usbbuf[256];
int usbsize;

//*----------------------------------------------------------------------------
//* \fn    AT91F_USB_Open
//* \brief This function Open the USB device
//*----------------------------------------------------------------------------
void vUsbTask(void *pvParameters)
{
	(void) pvParameters;
	// Init USB device
	AT91F_USB_Open();
	while (1)
	{
		//  Check enumeration
		if (pCDC.IsConfigured(&pCDC))
		{
			usbsize = pCDC.Read(&pCDC, (char*) usbbuf, 256);
			usbsize = RVnetSlaveProcess(usbbuf, usbsize, 1); //2.Do quest
			pCDC.Write(&pCDC, (char*) usbbuf, usbsize);
		}
		vTaskDelay(10);
	}
}
;

