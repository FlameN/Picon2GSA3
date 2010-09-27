/*
 * modem.c
 *
 *  Created on: Jan 18, 2010
 *      Author: albert
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dbg.h"
#include "modem.h"
#include "usart.h"
#include "typedef.h"

#define MODEM_SIO_BUFF_SIZE 1024

uint8 sio_buf[MODEM_SIO_BUFF_SIZE];

#define SIOFIFOSIZE 1024
uint8 sio_fifo[SIOFIFOSIZE];
//uint8 sio_buf[64];
volatile int bufpoint, bufsize;

#define STRBUFLEN 256

char strbuf[STRBUFLEN];
#define MODEM_PORT (&COM0)

const AT_COMMAND atset[AT_AMOUNT] =
{ "Call Ready", MA_CALLREADY, "OK", MA_OK, "SERVER OK", MA_SERVEROK,
		"NORMAL POWER DOWN", MA_NORMALPOWERDOWN, "STATE: IP STATUS",
		MA_STATEIPSTATUS, "STATE: IP CLOSE", MA_STATEIPCLOSE,
		"STATE: IP INITIAL", MA_STATEIPINITIAL, "STATE: IP GPRSACT",
		MA_STATEIPGPRSACT, "STATE: PDP DEACT", MA_STATEPDPDEACT, "> ",
		MA_CIPSENDREADY, "CONNECT", MA_CONNECT, "NO CARRIER", MA_NOCARRIER,
		"ERROR", MA_ERROR };

bool IsLiteral(char ch)
{
	if ((ch >= ' ') && (ch <= '~'))
	{
		return true;
	}
	return false;
}

void InitSioBuf()
{
	bufpoint = 0;
	bufsize = 0;
}

bool PopFromBuf(char *ch)
{
	if (bufpoint >= bufsize)
	{
		return false;
	}
	*ch = sio_buf[bufpoint];
	bufpoint++;
	return true;
}

void FillBuf()
{
	InitSioBuf();
	bufsize = AT91F_USART_RECEIVE(MODEM_PORT, (uint8*) sio_buf, SIOFIFOSIZE,
			4000);

}

bool ReadString(char *str, int size)
{
	char ch;
	if (bufpoint >= bufsize)
	{
		FillBuf();
	}
	do // remove non symbolic data
	{
		if (PopFromBuf(&ch) == false)
		{
			return false;
		}
	} while (!IsLiteral(ch));
	do
	{
		if (IsLiteral(ch))
		{
			*str = ch;
			str++;
		}
		else // packet end
		{
			*str = 0; //  add end of string
			return true;
		}
	} while (PopFromBuf(&ch) == true);
	return false;
}

MDEM_ANSWER GetModemAnswer(char *s)
{
	int i;
	if (*s == 0)
	{
		return MA_UNKNOWN;
	}
	for (i = 0; i < AT_AMOUNT; i++)
	{
		if (strcmp(atset[i].str, s) == 0)
		{
			return (atset[i].ma);
		}
	}
	return MA_UNKNOWN;
}

MDEM_ANSWER ReadModem()
{
	if (ReadString(strbuf, STRBUFLEN) == false)
	{
		return MA_UNKNOWN;
	}
	dbgmessage("from modem>");
	dbgmessage(strbuf);
	return GetModemAnswer((char*) strbuf);
}
/* ----------------------- Start implementation -----------------------------*/
void ModemWrite(char *str)
{
	dbgmessage("to modem>");
	dbgmessage(str);
	uint32 i = 0;
	char *tmpstr = str;
	while (*str)
	{
		str++;
		i++;
		if (i == 62)
			break;
	}
	AT91F_USART_SEND(MODEM_PORT, (uint8*) tmpstr, i);
}
bool WaitAnsver(MDEM_ANSWER ma)
{
	int i = 5;

	while (ReadModem() != ma)
	{
		if (i == 0)
		{
			return false;
		}
		i--;
	}
	//	dbgmessage("Ok\n");
	return true;
}

bool WaitCallReady()
{
	int i = 0;
	dbgmessage("Waiting for modem.");
	while (ReadModem() != MA_CALLREADY)
	{
		vTaskDelay(200);
		if (i > 7)
		{
			return false;
		}
		i++;
	};
	return true;
}
bool WaitOk()
{
	ModemWrite("AT\r");
	return WaitAnsver(MA_OK);
}

bool WaitAtd()
{
	ModemWrite("AT&D0\r");
	return WaitAnsver(MA_OK);
}

bool WaitAte()
{
	ModemWrite("ATE0V1\r");
	return WaitAnsver(MA_OK);
}

bool WaitGprsConnect()
{
	char myIp[17];
	char strsipstart[256];

	char header[] = "+IPD";
	int packSize;
	int headerSize;
	int i;

	ModemWrite("AT+CIPMODE=0\r");
	if (WaitAnsver(MA_OK) == false)
		return false;
	//	ModemWrite("AT+CSQ\r");
	//	WaitAnsver(MA_OK); // TODO replace wait answer with parsing level string
	//	if (WaitAnsver(MA_OK) == false)
	//	return false;

	//	ModemWrite("AT+CREG?\r");
	//	WaitAnsver(MA_OK);// TODO replace wait answer with parsing status
	//	if (WaitAnsver(MA_OK) == false)
	//	return false;

	ModemWrite(PPP_ADDSTR);
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite(PPP_CSTT);
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CMGF=1\r");
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CIPSPRT=2\r");
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CIPHEAD=1\r");
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CIICR\r");
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CIPCSGP=1,\"CMNET\"\r");
	if (WaitAnsver(MA_OK) == false)
		return false;

	ModemWrite("AT+CLPORT=\"TCP\",\"4444\"\r");
	if (WaitAnsver(MA_OK) == false)
		return false;
	ModemWrite("AT+CIFSR\r");
	WaitAnsver(MA_UNKNOWN);

	for (i = 0; i < 5; i++)
	{
		ModemWrite("AT+CIPSERVER\r");
		if (WaitAnsver(MA_OK) == true)
			break;
	}
	if (i > 4)
		return false;

	ModemWrite("AT+CIPCCON=2\r");
	if (WaitAnsver(MA_OK) == false)
		return false;
	dbgmessage("GPRS Connected");
	return true;
}

void ModemInit()
{
	AT91F_USART_OPEN(AT91_USART_COM0_ID, 115200, AT91C_US_ASYNC_MODE);

	while (1)
	{
		dbgmessage("Init Modem.\r\n");
		dbgmessage("Modem ... ");
		MODEM_ENABLE;
		dbgmessage("Enabled ....  ");
		vTaskDelay(3000);
		MODEM_DISABLE;
		dbgmessage("Disable ....  ");
		vTaskDelay(10000);
		MODEM_ENABLE;

		vTaskDelay(1000);
		if (!WaitCallReady())
			continue;
		if (!WaitOk())
		{
			continue;
		}
		vTaskDelay(50);
		if (!WaitAtd())
			continue;
		vTaskDelay(30);

		if (!WaitAte())
			continue;
		vTaskDelay(50);

		if (!WaitOk())
			continue;
		vTaskDelay(50);

		if (!WaitGprsConnect())
			continue;
		vTaskDelay(50);
		return;
	}
}
int ModemReceiveData(uint8 *buf, int maxsize)
{
	char header[] = "+IPD";
	int packSize;
	int headerSize;
	int i;
	bufsize = AT91F_USART_RECEIVE(MODEM_PORT, (uint8*) sio_buf, SIOFIFOSIZE,
			4000);
	for (i = 0; i < 4; i++)
	{
		if (sio_buf[i] != header[i])
		{
			return 0;
		}
	};
	sscanf((char*) sio_buf, "+IPD%d:", &packSize);
	headerSize = 0;
	while (sio_buf[headerSize] != ':')
	{
		headerSize++;
	}
	headerSize++;
	while (bufsize < (packSize + headerSize))
	{
		int tmpsize;
		tmpsize = AT91F_USART_RECEIVE(MODEM_PORT, (uint8*) (&sio_buf[bufsize]),
				SIOFIFOSIZE - bufsize, 5000);
		if (tmpsize == 0)
		{
			break;
		}
		bufsize += tmpsize;
	}
	i = 0;
	while (bufsize)
	{
		if (i >= maxsize)
		{
			return 0;
		}
		*buf = sio_buf[i + headerSize];
		buf++;
		i++;
		bufsize--;
	}
	return packSize;
}
void ModemSendData(uint8 *buf, uint8 size)
{
	char strCipsend[20];
	sprintf(strCipsend, "AT+CIPSEND=%d\r", size);
	ModemWrite(strCipsend);
	AT91F_USART_SEND(MODEM_PORT, buf, size);
	//	WaitAnsver(MA_UNKNOWN); // listen here send ok or error ... no matter//
}

