// SPI_BUS
#include <stdlib.h>
/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "spibus.h"
#include "spi.h"
#include "relay.h"
#include "board.h"
#include "modules.h"
#include "usermemory.h"

#define SPI_DTATA_SIZE  (4*2+1*2)
SPI_PCD_DATA SPIData[SPI_DTATA_SIZE];

void vSpibusTask(void *pvParameters )
{
  portTickType xSpiRequestRate = 2, xLastSpiRequestTime;
  ( void ) pvParameters;

  //-------------------------------------
  //   AT91F_PIO_ClearOutput( AT91C_BASE_PIOB,MODENABLE); //
  //-----------------------------------------------------------------------------------------------------
  ProcessOutputs(SPIData);
  AT91F_SPI_TRANSACTION_BLOCK (&SPIData[0],SPI_DTATA_SIZE);
  vTaskDelay(2);
  ProcessInputs(SPIData); //
  vTaskDelay(2);
  xLastSpiRequestTime = xTaskGetTickCount();
  while (1)
      {
        ProcessOutputs(SPIData);//
        AT91F_SPI_TRANSACTION_BLOCK (&SPIData[0],SPI_DTATA_SIZE);
       // vTaskDelayUntil( &xSpiRequestRate, xLastSpiRequestTime );
        vTaskDelay(2);
        ProcessInputs(SPIData); //
        //PICONRAM[0] = (SPIData[0].TDH<<8) + SPIData[0].TDL;
        //PICONRAM[1] = (SPIData[1].TDH<<8) + SPIData[1].TDL;
      }
};
