/*
 * journal.c
 *
 *  Created on: June 16, 2010
 *      Author: Nick Bokhan
 *
 */
#include "journal.h"
#include "usermemory.h"
#include "rtclock.h"



extern size_t strlen(const char * str);
extern char * strcat(char * destination, const char * source);

void reverse(char *str)
    {
    char c;
    int size=strlen(str)/2;
    for(int i=0;i<size;i++)
	{
	c=str[i];
	str[i]=str[strlen(str)-i-1];
	str[strlen(str)-i-1]=c;
	}
    }

/* itoa:  конвертируем n в символы в s */
void itoa(int n, char s[])
    {
    int i, sign;

    if ((sign = n) < 0) /* записываем знак */
	n = -n; /* делаем n положительным числом */
    i = 0;
    do
	{ /* генерируем цифры в обратном порядке */
	s[i++] = n % 10 + '0'; /* берем следующую цифру */
	}
    while ((n /= 10) > 0); /* удаляем */
    if (sign < 0)
	s[i++] = '-';
    s[i] = '\0';
    reverse(s);
    }

void GetTimeString(char *aBuf)
    {
    DATATIME dtl[2];
    char tempBuf[16];
    char overalBuff[32];
    rtcGetDataTime(dtl);
    overalBuff[0] = '\0';

    itoa(dtl->Years, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), "/");
    itoa(dtl->Month, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), "/");
    itoa(dtl->Data, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), ": ");
    itoa(dtl->Hour, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), ".");
    itoa(dtl->Min, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), ".");
    itoa(dtl->Sec, tempBuf);
    strcat(strcat((char *) overalBuff, (char *) tempBuf), ".");
    CopyDataBytes((uint8*) overalBuff, (uint8*) aBuf, strlen(overalBuff)+1);
    }

bool JrnlClear()
    {
    uint16 journaLength;
    journaLength = 1;
    return MemSetWords(0x2000, &journaLength, 1);
    }

bool JrnlWrite(char *aMsg)
    {
    uint16 journaLength;

    char buffer[64];
    buffer[0]=0;
    GetTimeString(buffer);
    strcat(buffer, aMsg);
    MemGetWords(0x2000, &journaLength, 1);
    if (journaLength + strlen(buffer) / 2 > 0x8000)
	return false;
    MemSetWords(0x2000 + journaLength, (uint16*) buffer, (strlen(buffer)+1) / 2
	    + (strlen(buffer)+1) % 2);
    journaLength += (strlen(buffer)+1) / 2 + (strlen(buffer)+1) % 2;
    return MemSetWords(0x2000, &journaLength, 1);
    }

uint16 GetJrnlLength()
    {
    uint16 journaLength;
    MemGetWords(0x2000, &journaLength, 1);
    return journaLength;
    }


