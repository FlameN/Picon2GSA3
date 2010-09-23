//
#ifndef _MODULES
#define _MODULES
#include"spi.h"
#include "typedef.h"

#define CHECK_CODE 0x3344


#define MODULES_COUNT 5

#define MODULE_NON         0
#define MODULE_DISKRET11   1
#define MODULE_RELAY8      2


extern void ProcessInputs(SPI_PCD_DATA *pxSPIData);
extern void ProcessOutputs(SPI_PCD_DATA *pxSPIData);
extern void ModulesInit(void);
extern int GetModulErrors(int i);
#endif


