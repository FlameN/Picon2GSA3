// Modules
#include"modules.h"

#include"error.h"
#include "usermemory.h"
#include"relay.h"
#include"diskrets.h"

ErrorHandle Modules_Error;
int ModulesErrorsCounter[MODULES_COUNT];

int GetModulErrors(int i)
{
	if (i >= MODULES_COUNT)
		return (-1);
	return ModulesErrorsCounter[i];
}
//----------------------------------------------------------------
void AddModulErrors(int i)
{
	if (i >= MODULES_COUNT)
		return;
	if (ModulesErrorsCounter[i] > 10000)
		return;
	ModulesErrorsCounter[i]++;
}
//----------------------------------------------------------------
void ResetModulErrors(int i)
{
	if (i >= MODULES_COUNT)
		return;
	ModulesErrorsCounter[i] = 0;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void ModulesInit()
{
	int i = MODULES_COUNT;
	while (i--)
	{
		ResetModulErrors(i);
	}
	Modules_Error = ErrorCreate("ModulesError");
	DiskretsClear();
}
//----------------------------------------------------------------
void ProcessInputs(SPI_PCD_DATA *pxSPIData)
{
	uint16 spidata;
	uint16 checkCode;
	for (int i = 0; i < 4; i++)
	{
		// 4 ������ ��������� �� 10 ������
		spidata = ((unsigned int)pxSPIData->TDL) + ((unsigned int)pxSPIData->TDH << 8);
		checkCode = ((unsigned int)(pxSPIData+1)->TDL) + ((unsigned int)(pxSPIData+1)->TDH << 8);
		//if (checkCode != CHECK_CODE) // It chek fail on low speed diskrets
		if ( (spidata & 0x1F00) != 0) // use for all types of diskretds modules
		{
			AddModulErrors(1 + i);
			SYSTEM_ERROR(Modules_Error);
			for (int j = 0; j < 11; j++)
			{
				DiskretSet(i * 11 + j, DISKRET_STATE_MAILFUNCTION);
			}
		}
		else
		{
			ResetModulErrors(1+i);
			DiskretSet(i * 11 + 0, ((~spidata) >> 13) & 0x1);
			DiskretSet(i * 11 + 1, ((~spidata) >> 14) & 0x1);
			DiskretSet(i * 11 + 2, ((~spidata) >> 15) & 0x1);
			DiskretSet(i * 11 + 3, ((~spidata) >> 0 ) & 0x1);
			DiskretSet(i * 11 + 4, ((~spidata) >> 1 ) & 0x1);
			DiskretSet(i * 11 + 5, ((~spidata) >> 2 ) & 0x1);
			DiskretSet(i * 11 + 6, ((~spidata) >> 3 ) & 0x1);
			DiskretSet(i * 11 + 7, ((~spidata) >> 4 ) & 0x1);
			DiskretSet(i * 11 + 8, ((~spidata) >> 5 ) & 0x01);
			DiskretSet(i * 11 + 9, ((~spidata) >> 6 ) & 0x01);
			DiskretSet(i * 11 + 10, ((~spidata) >> 7) & 0x01);
		};
		pxSPIData += 2;
	}
	DiskretsProcess();
	spidata = ((unsigned int)pxSPIData->TDL) + ((unsigned int)pxSPIData->TDH << 8);
	RAM.dwords[0x130]=spidata;
	checkCode = ((unsigned int)(pxSPIData+1)->TDL) + ((unsigned int)(pxSPIData+1)->TDH << 8);
	RAM.dwords[0x131]=checkCode;
	if(((uint8)spidata&0xFF == 0xFF) && (checkCode&0xFF== 0xFF)){AddModulErrors(0);}else
	    ResetModulErrors(0);
}
//----------------------------------------------------------------
void ProcessOutputs(SPI_PCD_DATA *pxSPIData)
{
	uint8 tReley;
	RelaysProcess();
	//-- 4 diskrets modules
	for (int i = 0; i < 4; i++)
	{
		pxSPIData->LASTXFER = 0;
		pxSPIData->PCS = 5 - i;
		pxSPIData->TDL = CHECK_CODE & 0xFF;
		pxSPIData->TDH = (CHECK_CODE>>8)& 0xFF;
		pxSPIData++;
		pxSPIData->LASTXFER = 0;
		pxSPIData->PCS = 5 - i;
		pxSPIData->TDL = 0xFF;
		pxSPIData->TDH = 0xFF;
		pxSPIData++;
	}
	// 1 �������� ...
	tReley = 0;
	for (int i = 0; i < RELAYS_AMOUNT; i++)
	{
		if (RelayGet(i) == RELAY_STATE_ON)
		{
			tReley |= (0x01) << i;
		}
	};
	tReley = ~tReley;
	pxSPIData->LASTXFER = 0;
	pxSPIData->PCS = 6;
	pxSPIData->TDL = 0x55;
	pxSPIData->TDH = 0x55;
	pxSPIData++;
	pxSPIData->LASTXFER = 0;
	pxSPIData->PCS = 6;
	pxSPIData->TDL = 0x55;
	pxSPIData->TDH = tReley;
	pxSPIData++;

}
