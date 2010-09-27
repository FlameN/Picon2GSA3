#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "rs485.h"
#include "rvnet.h"
#include "iec61107.h"
#include "board.h"
#include "usermemory.h"

char rs485buf[256];
char tmpbuff[128];
RVNET_DATATYPE rs485size;
IDENTIFIER id;

#define AT91C_US_ASYNC_IEC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_EVEN + \
                        AT91C_US_CHRL_7_BITS + \
                        AT91C_US_CLKS_CLOCK )

bool AskConunter();

uint16 ConvertUIP(char* string)
{

	return (atof(string) / 400) * 65536;
}

void MakeiFixUIP()
{
	//Voltage
	RAM.UIPparams[0] = (uint16) (atof(&RAM.strings[8][0]) * 65535 / 400);
	RAM.UIPparams[1] = (uint16) (atof(&RAM.strings[9][0]) * 65535 / 400);
	RAM.UIPparams[2] = (uint16) (atof(&RAM.strings[10][0]) * 65535 / 400);
	//current
	RAM.UIPparams[3] = (uint16) (atof(&RAM.strings[11][0]) * 65535 / 100);
	RAM.UIPparams[4] = (uint16) (atof(&RAM.strings[12][0]) * 65535 / 100);
	RAM.UIPparams[5] = (uint16) (atof(&RAM.strings[13][0]) * 65535 / 100);
	//Power
	RAM.UIPparams[6] = (uint16) (atof(&RAM.strings[5][0]) * 65535 / 40);
	RAM.UIPparams[7] = (uint16) (atof(&RAM.strings[6][0]) * 65535 / 40);
	RAM.UIPparams[8] = (uint16) (atof(&RAM.strings[7][0]) * 65535 / 40);

	RAM.UIPparams[9] = ((uint32) (atof(&RAM.strings[4][0]) * 65536 / 40))
			& 0xFF;
	RAM.UIPparams[10] = (((uint32) (atof(&RAM.strings[4][0]) * 65536 / 40))
			>> 8) & 0xFF;
	/*
	 RAM.CounterReqParams[2] = ((uint32)(atof(&RAM.strings[15][0])*65536/40))&0xFF;
	 RAM.CounterReqParams[3] = (((uint32)(atof(&RAM.strings[15][0])*65536/40))>>8)&0xFF;

	 RAM.CounterReqParams[4] = ((uint32)(atof(&RAM.strings[16][0])*65536/40))&0xFF;
	 RAM.CounterReqParams[5] = (((uint32)(atof(&RAM.strings[16][0])*65536/40))>>8)&0xFF;

	 RAM.CounterReqParams[6] = ((uint32)(atof(&RAM.strings[17][0])*65536/40))&0xFF;
	 RAM.CounterReqParams[7] = (((uint32)(atof(&RAM.strings[17][0])*65536/40))>>8)&0xFF;
	 */
}

void vRs485Task(void *pvParameters)
{

	/* The parameters are not used. */
	(void) pvParameters;

#ifdef TESTBOARD
	AT91F_USART_OPEN(AT91_USART_COM2_ID, 115200, AT91C_US_ASYNC_MODE);
	while (1)
	{
		RS485_RECEIVE_MODE();

		rs485size = AT91F_USART_RECEIVE(&COM2, rs485buf, 256, 5000);
		if (rs485size != 0)
		{

			LED_MODE_GREEN(1);

			rs485size = RVnetSlaveProcess(rs485buf, rs485size, 1);

			RS485_SEND_MODE();
			vTaskDelay(5);
			AT91F_USART_SEND(&COM2, rs485buf, rs485size);

			LED_MODE_GREEN(0);
		}
		vTaskDelay(1);
	}
#else

	AT91F_USART_OPEN(AT91_USART_COM2_ID, 9600, AT91C_US_ASYNC_IEC_MODE);

	while (1)
	{
		if (AskConunter())
		{
			vTaskDelay(10);

		}
		else
		{

			rs485size = iecProcExitPacket(rs485buf);
			RS485_SEND_MODE();
			AT91F_USART_SEND(&COM2, (uint8*) rs485buf, rs485size);
			vTaskDelay(300);
			vTaskDelay(20);
		}
	}
#endif
}
;

bool SendReseive()
{
	//  vTaskDelay(50);
	LED_WORK_GREEN(1);
	RS485_SEND_MODE();
	vTaskDelay(50);
	AT91F_USART_SEND(&COM2, (uint8*) rs485buf, rs485size);

	RS485_RECEIVE_MODE();

	rs485size = AT91F_USART_RECEIVE(&COM2, (uint8*) rs485buf, 256, 3000);

	if (rs485size == 0)
		return false;
	vTaskDelay(50);
	LED_WORK_GREEN(0);
	vTaskDelay(100);
	return true;
}
bool AskParam(char *paramName, char *value)
{
	value[0] = 0;
	rs485size = iecProcVarReqPacket(rs485buf, paramName, 32);
	if (!SendReseive())
		return false;
	if (!iecProcVarAnswerPacket(rs485buf, value, 128))
		return false;
	return true;
}
char* ParseAnswer(char *buf, char *dsn)
{
	while (*buf && *buf != '(')
		buf++;
	buf++;
	while (*buf && *buf != ')')
	{
		*dsn = *buf;
		dsn++;
		buf++;
	}
	*dsn = 0;
	return buf;
}
bool AskConunter()
{

	char *pxBuf;
	// Find Device
	rs485size = iecProcReqPacket(rs485buf, "", 10);
	if (!SendReseive())
		return false;
	if (!iecIndProc(rs485buf, &id))
		return false;

	// Password & security
	rs485size = iecOptionsAckPacket(rs485buf, '0', '5', '1');
	if (!SendReseive())
		return false;
	if (!iecProcAddrAnswer(rs485buf, &RAM.strings[0][0], 128))
		return false;
	//#define IEC_PASS "777777"
	char* pass = ((FLASHMEM*) DEVICE_FLASHMEM_LOCATION)->devcfg.logica.reserv;
	if (strlen((char*) pass) > 0 && strlen((char*) pass) < 15)
	{
		rs485size
				= iecProcPassPacket(
						rs485buf,
						(char*) (((FLASHMEM*) DEVICE_FLASHMEM_LOCATION)->devcfg.logica.reserv),
						16);
		if (!SendReseive())
			return false;
		if (rs485buf[0] != ACK)
			return false;
	}

	if (!AskParam("DATE_", tmpbuff))
		return false;
	ParseAnswer(tmpbuff, &RAM.strings[1][0]);

	if (!AskParam("TIME_", tmpbuff))
		return false;
	ParseAnswer(tmpbuff, &RAM.strings[2][0]);

	if (!AskParam("SNUMB", tmpbuff))
		return false;
	ParseAnswer(tmpbuff, &RAM.strings[3][0]);

	if (!AskParam("ET0PE", tmpbuff))
		return false;
	ParseAnswer(tmpbuff, &RAM.strings[4][0]);

	if (!AskParam("POWPP", tmpbuff))
		return false;
	pxBuf = tmpbuff;
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[5][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[6][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[7][0]);

	if (!AskParam("VOLTA", tmpbuff))
		return false;
	pxBuf = tmpbuff;
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[8][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[9][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[10][0]);

	if (!AskParam("CURRE", tmpbuff))
		return false;
	pxBuf = tmpbuff;
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[11][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[12][0]);
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[13][0]);

	if (!AskParam("FREQU", tmpbuff))
		return false;
	pxBuf = tmpbuff;
	pxBuf = ParseAnswer(pxBuf, &RAM.strings[14][0]);
	/*
	 if (!AskParam("EAMPE", tmpbuff))
	 return false;
	 ParseAnswer(tmpbuff, &RAM.strings[15][0]);

	 if (!AskParam("ENMPE", tmpbuff))
	 return false;
	 ParseAnswer(tmpbuff, &RAM.strings[16][0]);

	 if (!AskParam("ENDPE", tmpbuff))
	 return false;
	 ParseAnswer(tmpbuff, &RAM.strings[17][0]);
	 */
	MakeiFixUIP();

	rs485size = iecProcExitPacket(rs485buf);
	RS485_SEND_MODE();
	AT91F_USART_SEND(&COM2, (uint8*) rs485buf, rs485size);
	vTaskDelay(300);
	return true;
}

