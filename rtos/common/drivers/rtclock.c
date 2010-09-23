#include "rtclock.h"
#include "twi.h"
#include "journal.h"

#define RTCCLOCK_CORRECTOR 14975
#define RTC_DADR      (0x68)

DATATIME RTClock[2];
bool iFixLocal;

unsigned int RTCCorrector = 0;

uint32 atcTickCounter;

bool rtcGetHardClock(DATATIME *dt);
bool rtcSetHardClock(DATATIME *dt);
void rtcUpdateLocalDT();

unsigned char month_day_table[] =
    {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
unsigned char leap_year(unsigned int test_year);
//���������� 1, ���� ��� test_year - ����������
unsigned char leap_year(unsigned int test_year)
    {
    if (((test_year % 4 == 0) && (test_year % 100)) || (test_year % 400 == 0))
	return 1;
    else
	return 0;
    }
unsigned int at91_bcd_int(unsigned char bcd)
    {
    return ((bcd & 0x0F) + (((bcd & 0xF0) >> 4) * 10));

    }
unsigned char at91_int_bcd(unsigned int value)
    {
    char tmp[2];
    tmp[1] = (value % 10) + 0;
    value /= 10;
    tmp[0] = (value % 10) + 0;
    return ((tmp[1] & 0x0F) | ((tmp[0] & 0x0F) << 4));
    }

void rtcInit(void)
    {

    rtcGetHardClock(&RTClock[0]);
    if (RTClock[0].MSec > 999)
	{
	RTClock[0].MSec = 0;
	}
    if (RTClock[0].Sec > 59)
	{
	RTClock[0].Sec = 0;
	}
    if (RTClock[0].Min > 59)
	{
	RTClock[0].Min = 0;
	}
    if (RTClock[0].Hour > 23)
	{
	RTClock[0].Hour = 0;
	}
    if (RTClock[0].Day > 7)
	{
	RTClock[0].Day = 1;
	}
    if (RTClock[0].Data > 31)
	{
	RTClock[0].Data = 1;
	}
    rtcUpdateLocalDT();
    }
unsigned int rtcResetConfig(void)
    {
    unsigned char cmd;
    cmd = 0x00;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x01, &cmd, 1);
    cmd = 0x1F;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x0A, &cmd, 1);
    cmd = 0x00;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x0B, &cmd, 1);
    cmd = 0x01;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x00, &cmd, 1);
    cmd = 0x00;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x00, &cmd, 1);

    return 0;
    }
bool rtcSetHardClock(DATATIME *dt)
    {
    unsigned char RTCBUF[16];
    RTCBUF[0] = at91_int_bcd(dt->Sec);
    RTCBUF[1] = at91_int_bcd(dt->Min);
    RTCBUF[2] = at91_int_bcd(dt->Hour);
    RTCBUF[3] = at91_int_bcd(dt->Day);
    RTCBUF[4] = at91_int_bcd(dt->Data);
    RTCBUF[5] = at91_int_bcd(dt->Month);
    RTCBUF[6] = at91_int_bcd(dt->Years);
    if (AT91F_TWI_WriteByteAddress(RTC_DADR, 0, RTCBUF, 7) != 0)
	{
	return false;
	};
    //AT91F_TWI_WriteByteAddress(RTC_DADR, 0x08, (uint8*) &dt->ram[0], 56);
    return true;
    }

bool rtcGetHardClock(DATATIME *dt)
    {
    unsigned char cmd;
    unsigned char RTCBUF[16];
    cmd = 1;
    if (AT91F_TWI_ReadByteAddress(RTC_DADR, 0, RTCBUF, 7) != 0)
	{
	return false;
	}
    dt->MSec = 0;
    dt->Sec = at91_bcd_int(RTCBUF[0]);
    dt->Min = at91_bcd_int(RTCBUF[1]);
    dt->Hour = at91_bcd_int(RTCBUF[2]);
    dt->Day = at91_bcd_int(RTCBUF[3]);
    dt->Data = at91_bcd_int(RTCBUF[4]);
    dt->Month = at91_bcd_int(RTCBUF[5]);
    dt->Years = at91_bcd_int(RTCBUF[6]);
    //AT91F_TWI_ReadByteAddress(RTC_DADR, 0x08, (uint8*) &dt->ram[0], 56);
    return true;
    }
void rtcGetDataTime(DATATIME *dt)
    {
    rtcUpdateLocalDT();
    dt[0].MSec = RTClock[0].MSec;
    dt[0].Sec = RTClock[0].Sec;
    dt[0].Min = RTClock[0].Min;
    dt[0].Hour = RTClock[0].Hour;
    dt[0].Day = RTClock[0].Day;
    dt[0].Data = RTClock[0].Data;
    dt[0].Month = RTClock[0].Month;
    dt[0].Years = RTClock[0].Years;

    dt[1].MSec = RTClock[1].MSec;
    dt[1].Sec = RTClock[1].Sec;
    dt[1].Min = RTClock[1].Min;
    dt[1].Hour = RTClock[1].Hour;
    dt[1].Day = RTClock[1].Day;
    dt[1].Data = RTClock[1].Data;
    dt[1].Month = RTClock[1].Month;
    dt[1].Years = RTClock[1].Years;
    /*for(i=0;i<28;i++)
     {
     dt->ram[i] = RTClock.ram[i];
     }*/
    }
void rtcGetLocalDataTime(DATATIME *dt)
    {
    rtcUpdateLocalDT();
    dt->MSec = RTClock[1].MSec;
    dt->Sec = RTClock[1].Sec;
    dt->Min = RTClock[1].Min;
    dt->Hour = RTClock[1].Hour;
    dt->Day = RTClock[1].Day;
    dt->Data = RTClock[1].Data;
    dt->Month = RTClock[1].Month;
    dt->Years = RTClock[1].Years;
    }

void rtcUpdateLocalDT()
    {
    int shift = 0;//our shift amount
    if (RTClock[0].Month > 3 && RTClock[0].Month < 10)
	{
	shift = 1;
	}
    if (RTClock[0].Month == 3 && RTClock[0].Data > 24 && RTClock[0].Data < 31
	    - RTClock[0].Day)
	{
	shift = 1;
	}
    if (RTClock[0].Month == 10 && !(RTClock[0].Data > 24 && RTClock[0].Data
	    < 31 - RTClock[0].Day))
	{
	shift = 1;
	}

    RTClock[1].MSec = RTClock[0].MSec;
    RTClock[1].Sec = RTClock[0].Sec;
    RTClock[1].Min = RTClock[0].Min;
    RTClock[1].Hour = RTClock[0].Hour + shift;
    if (RTClock[0].Hour > 23)
	{
	RTClock[1].Hour = 0;
	}
    else
	{
	shift = 0;
	}

    RTClock[1].Day = RTClock[0].Day + shift;
    if (RTClock[1].Day > 7)
	{
	RTClock[1].Day = 1;
	}
    RTClock[1].Data = RTClock[0].Data + shift;
    if (RTClock[1].Data > month_day_table[RTClock[0].Month])
	{
	RTClock[1].Data = 1;
	}
    else
	{
	shift = 0;
	}
    RTClock[1].Month = RTClock[0].Month + shift;
    RTClock[1].Years = RTClock[0].Years;
    /*for (i = 0; i < 28; i++)
     {
     RTClock[1].ram[i] = RTClock[0].ram[i];
     }*/

    }

void rtcSetDataTime(DATATIME *dt)
    {
    rtcSetHardClock(dt);
    RTClock[0].MSec = dt->MSec;
    RTClock[0].Sec = dt->Sec;
    RTClock[0].Min = dt->Min;
    RTClock[0].Hour = dt->Hour;
    RTClock[0].Day = dt->Day;
    RTClock[0].Data = dt->Data;
    RTClock[0].Month = dt->Month;
    RTClock[0].Years = dt->Years;
    /*for(i=0;i<28;i++)
     {
     RTClock.ram[i] = dt->ram[i];
     }*/
    }

void rtcSetLocalDataTime(DATATIME *dt)
    {

    int shift = 0;//our shift amount
    if (RTClock[0].Month > 3 && RTClock[0].Month < 10)
	{
	shift = 1;
	}
    if (RTClock[0].Month == 3 && RTClock[0].Data > 24 && RTClock[0].Data < 31
	    - RTClock[0].Day)
	{
	shift = 1;
	}
    if (RTClock[0].Month == 10 && !(RTClock[0].Data > 24 && RTClock[0].Data
	    < 31 - RTClock[0].Day))
	{
	shift = 1;
	}

    RTClock[0].MSec = dt->MSec;
    RTClock[0].Sec = dt->Sec;
    RTClock[0].Min = dt->Min;
    RTClock[0].Hour = dt->Hour - shift;
    if (dt->Hour == 0)
	{
	RTClock[0].Hour = 23;
	}
    else
	{
	shift = 0;
	}

    RTClock[0].Day = dt->Day - shift;
    if (RTClock[0].Day == 1)
	{
	RTClock[0].Day = 7;
	}
    RTClock[0].Data = dt->Data - shift;
    if (RTClock[0].Data == 1)
	{
	RTClock[0].Data = month_day_table[dt->Month - 1];
	}
    else
	{
	shift = 0;
	}
    RTClock[0].Month = dt->Month - shift;
    RTClock[0].Years = dt->Years;
    /*for (i = 0; i < 28; i++)
     {
     RTClock[0].ram[i] = dt->ram[i];
     }*/
    rtcSetHardClock(&RTClock[0]);
    }

void rtcGetRam(uint8 *pxBuf, int len)
    {
    if (len > 56)
	return;
    AT91F_TWI_ReadByteAddress(RTC_DADR, 0x08, pxBuf, len);
    }
void rtcSetRam(uint8 *pxBuf, int len)
    {
    if (len > 56)
	return;
    AT91F_TWI_WriteByteAddress(RTC_DADR, 0x08, pxBuf, len);
    }

void rtcSync(void)
    {
    rtcSetHardClock(&RTClock[0]);
    }

void rtcIncrementSoftClockMs(void)
    {
    unsigned char day_tmp;
    RTCCorrector++;
    if (RTCCorrector == RTCCLOCK_CORRECTOR)
	{
	RTCCorrector = 0;
	}
    else
	{
	RTClock[0].MSec++;
	if (RTClock[0].MSec > 999)
	    {
	    RTClock[0].MSec = 0;
	    RTClock[0].Sec++;
	    if (RTClock[0].Sec > 59)
		{
		RTClock[0].Sec = 0;
		RTClock[0].Min++;
		if (RTClock[0].Min > 59)
		    {
		    RTClock[0].Min = 0;
		    RTClock[0].Hour++;
		    if (RTClock[0].Hour > 23)
			{

			RTClock[0].Hour = 0;
			RTClock[0].Day++;
			if (RTClock[0].Day > 7)
			    {
			    RTClock[0].Day = 1;
			    }
			RTClock[0].Data++;

			if (leap_year(RTClock[0].Years) && (RTClock[0].Month
				== 2))
			    {
			    day_tmp = 29;
			    }
			else
			    {
			    day_tmp = month_day_table[RTClock[0].Month - 1];
			    }
			if (RTClock[0].Data > day_tmp)
			    {
			    RTClock[0].Data = 1;
			    RTClock[0].Month++;
			    if (RTClock[0].Month > 12)
				{
				RTClock[0].Month = 1;
				RTClock[0].Years++;
				}
			    }
			}
		    }
		}
	    }

	}
    }

//-----------------------------------------------------------

void itoaFix(int n, char s[])
    {
    n = n % 100;

    s[0] = n / 10+48;
    s[1] = n % 10+48;
    }

void rtcGetiFixDateTime(char *dt)
    {
    char buffer[2];
    DATATIME dtC[2];

    dt[0] = 48;
    if (iFixLocal)
	dt[1] = 49;
    else
	dt[1] = 48;

    //if (iFixLocal)
//	rtcGetLocalDataTime(&dtC);
 //   else
	rtcGetDataTime(dtC);

    itoaFix(dtC->Years, buffer);
    dt[2] = buffer[0];
    dt[3] = buffer[1];
    itoaFix(dtC->Month, buffer);
    dt[4] = buffer[0];
    dt[5] = buffer[1];
    itoaFix(dtC->Data, buffer);
    dt[6] = buffer[0];
    dt[7] = buffer[1];
    itoaFix(dtC->Hour, buffer);
    dt[8] = buffer[0];
    dt[9] = buffer[1];
    itoaFix(dtC->Min, buffer);
    dt[10] = buffer[0];
    dt[11] = buffer[1];
    itoaFix(dtC->Sec, buffer);
    dt[12] = buffer[0];
    dt[13] = buffer[1];
    itoaFix(dtC->MSec, buffer);
    dt[14] = buffer[0];
    dt[15] = buffer[1];
    }

void rtcSetiFixDateTime(char *dt)
    {
    char buffer[3];
    buffer[2] = 0;
    DATATIME dtC;

    if (dt[1] == 49)
	iFixLocal = true;
    else
	iFixLocal = false;

    buffer[0] = dt[2];
    buffer[1] = dt[3];
    dtC.Years = atoi(buffer);

    buffer[0] = dt[4];
    buffer[1] = dt[5];
    dtC.Month = atoi(buffer);

    buffer[0] = dt[6];
    buffer[1] = dt[7];
    dtC.Data = atoi(buffer);

    buffer[0] = dt[8];
    buffer[1] = dt[9];
    dtC.Hour = atoi(buffer);

    buffer[0] = dt[10];
    buffer[1] = dt[11];
    dtC.Min = atoi(buffer);

    buffer[0] = dt[12];
    buffer[1] = dt[13];
    dtC.Sec = atoi(buffer);

    buffer[0] = dt[14];
    buffer[1] = dt[15];
    dtC.MSec = atoi(buffer);
    if (iFixLocal)
	rtcSetLocalDataTime(&dtC);
    else
	rtcSetDataTime(&dtC);
    }

//-------------------------------------------------------------
void atcIncrementCounter()
    {
    atcTickCounter++;
    }

void atcResetCounter()
    {
    atcTickCounter = 0;
    }

uint32 atcGetCounter()
    {
    return atcTickCounter;
    }
