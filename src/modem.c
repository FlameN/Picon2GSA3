/*
 * modem.c
 *
 *  Created on: Jan 18, 2010
 *      Author: albert
 */
#include <stdlib.h>
#include <stdarg.h>
#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dbg.h"
#include "modem.h"
#include "usart.h"

#define AT_AMOUNT             5

const AT_COMMAND atset[AT_AMOUNT] =
    {
	    "Call Ready",
	    MA_CALLREADY,
	    "OK",
	    MA_OK,
	    "CONNECT",
	    MA_CONNECT,
	    "NO CARRIER",
	    MA_NOCARRIER,
	    "ERROR",
	    MA_ERROR
    };

extern bool ReadString(void *dev, char *, int);

#define MODEM_ENABLE { AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, MODEM_RST);}
#define  MODEM_DISABLE { AT91F_PIO_SetOutput(AT91C_BASE_PIOA, MODEM_RST);}

#define STRBUFLEN 32
uint8 strbuf[STRBUFLEN];

bool strcmp(char* s1, char* s2)
    {
    while (*s1 && *s2)
	{
	if (*s1 != *s2)
	    {
	    return false;
	    } // if strings not much
	s1++;
	s2++;
	}
    if (*s1 != *s2)
	{
	return false;
	}
    return true;
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
	if (strcmp(atset[i].str, s) == true)
	    {
	    return (atset[i].ma);
	    }
	}

    return MA_UNKNOWN;
    }

MDEM_ANSWER ReadModem()
    {
//    if (ReadString(&COM1, strbuf, STRBUFLEN - 1) == false)
	{
	dbgmessage(".");
	return MA_UNKNOWN;
	}
    dbgmessage("\n");
    dbgmessage("modem>");
    dbgmessage(strbuf);
    dbgmessage("\n");
    return GetModemAnswer(strbuf);
    }
/* ----------------------- Start implementation -----------------------------*/
void ModemWrite(char *str)
    {
    int i = 0;
    char *tmpstr = str;
    while (*str)
	{
	str++;
	i++;
	if (i == 62)
	    break;
	}
    AT91F_USART_SEND(&COM1, tmpstr, i);
    }
bool WaitAnsver(MDEM_ANSWER ma)
    {
    int i = 4;
    while (ReadModem() != ma)
	{
	if (i == 0)
	    {

	    return false;
	    }
	i--;
	}
    dbgmessage("Ok\n");
    return true;
    }

bool WaitCallReady()
    {
    dbgmessage("Waiting for modem.");
    while (ReadModem() != MA_CALLREADY)
	{
	vTaskDelay(200);
	};
    dbgmessage("...ok");
    return true;
    }
bool WaitOk()
    {
    dbgmessage("AT");
    ModemWrite("AT\r");
    return WaitAnsver(MA_OK);
    }

bool WaitAts()
    {
    dbgmessage("ATS=0 .. ");
    ModemWrite("ATS0=0\r");
    return WaitAnsver(MA_OK);
    }
bool WaitAtd()
    {
    dbgmessage("AT&D0 .. ");
    ModemWrite("AT&D0\r");
    return WaitAnsver(MA_OK);
    }

bool WaitAte()
    {
    dbgmessage("ATE0V1 .. ");
    ModemWrite("ATE0V1\r");
    return WaitAnsver(MA_OK);
    }

bool WaitAddSet()
    {
    dbgmessage("Addition Settings");
    ModemWrite(PPP_ADDSTR);
    return WaitAnsver(MA_OK);
    }
bool CallNumber()
    {
    int j = 3;
    dbgmessage("Call Number ");
    while (--j)
	{
	ModemWrite("ATDT*99***1#\r");
	if (WaitAnsver(MA_CONNECT) == true)
	    {
	    //    dbgmessage("...Connected");
	    return true;
	    }
	}
    return false;
    }
bool InitModem()
    {
    int i;
    dbgmessage("Modem ... ");
    MODEM_ENABLE;
    dbgmessage("Enabled ....  ");
    vTaskDelay(1000);
    MODEM_DISABLE;
    dbgmessage("Disable ....  ");
    vTaskDelay(10000);
    MODEM_ENABLE;
    dbgmessage("Enabled ....  ");
    vTaskDelay(5000);
    i = 0;
    while (1)
	{
	i++;
	if (!WaitOk())
	    {
	    if (i > 5)
		return false;
	    }
	else
	    break;

	}
    vTaskDelay(50);
    if (!WaitAtd())
	return false;
    vTaskDelay(30);

    if (!WaitAte())
	return false;
    vTaskDelay(50);

    if (!WaitOk())
	return false;
    vTaskDelay(50);

    if (!WaitAddSet())
	return false;
    vTaskDelay(50);

    if (!WaitAts())
	return false;
    vTaskDelay(50);

    if (!WaitOk())
	return false;

    if (!WaitAte())
	return false;
    vTaskDelay(50);

    if (!WaitOk())
	return false;
    vTaskDelay(200);

    if (!WaitAddSet())
	return false;
    vTaskDelay(50);

    /*

     if (!WaitOk())
     return false;
     vTaskDelay(30);
     */

    //  if (!WaitAddSet())
    //  return false;
    vTaskDelay(30);
    if (!CallNumber())
	return false;
    //  vTaskDelay(2000);
    return true;
    }

