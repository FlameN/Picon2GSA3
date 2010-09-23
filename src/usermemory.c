//********************************************************
//    ������      :
//    ������      : 1.0.
//    ����        : 29.03.2009
//    �����       : ����������� ���� ��������.
//    ��������    : ����
//    ����        :
//    ����������� :
//    ����� ���������� : IAR
//*********************************************************
//    ���������           :
//    ������� ����������  :
//    ��� ���������       : ����������
//    ������ ������       :
//*********************************************************

#include "usermemory.h"
#include "twi.h"
#include "rtclock.h"
#include "flash.h"

#define __at(x)

#define EEPROM_A1A0   0
#define EEPROM_DADR   (0x50|EEPROM_A1A0)

RAMMEM RAM;

//DEVICEGFG *device = (void*) DEVICE_CFG_LOCATION;

bool GetRam(uint16, uint16*, uint16);
bool SetRam(uint16, uint16*, uint16);
bool GetEeprom(uint16, uint16*, uint16);
bool SetEeprom(uint16, uint16*, uint16);
bool GetRtc(uint16, uint16*, uint16);
bool SetRtc(uint16, uint16*, uint16);
bool GetFlash(uint16, uint16*, uint16);
bool SetFlash(uint16, uint16*, uint16);

bool GetSignature(uint16, uint16*, uint16);

//==============================================================================
void MemInit()
    {
    Flash_Init();
    int i;
    MemManInit();
    for (i = 0; i < DEVICE_RAMMEM_WORDS; i++)
	{
	RAM.dwords[i] = 0;
	}

    memunit[0].startaddr = 0x0000;
    memunit[0].endaddr = DEVICE_RAMMEM_WORDS;
    memunit[0].GetMem = GetRam;
    memunit[0].SetMem = SetRam;

#if(DEVICE_RAMMEM_WORDS!=0x400)
#error Check useer memory address table !!!
#endif

    memunit[4].startaddr = 0x0400;
    memunit[4].endaddr = 0x480;
    memunit[4].GetMem = GetSignature;
    //	memunit[1].SetMem = SetRam;

    memunit[1].startaddr = 0x1000;
    memunit[1].endaddr = 0x1031;
    memunit[1].GetMem = GetRtc;
    memunit[1].SetMem = SetRtc;

    memunit[2].startaddr = 0x2000;
    memunit[2].endaddr = 0x6000;
    memunit[2].GetMem = GetEeprom;
    memunit[2].SetMem = SetEeprom;

    memunit[3].startaddr = 0x8000;
    memunit[3].endaddr = 0x10000;
    memunit[3].GetMem = GetFlash;
    memunit[3].SetMem = SetFlash;

    }

//==============================================================================
bool GetRam(uint16 addr, uint16* pxDsn, uint16 len)
    {
    atcResetCounter();
    CopyDataBytes((uint8*) &RAM.dwords[addr], (uint8*) pxDsn, len * 2);
    return true;
    }
//==============================================================================
bool SetRam(uint16 addr, uint16 *pxSrc, uint16 len)
    {
    atcResetCounter();
    CopyDataBytes((uint8*) pxSrc, (uint8*) &RAM.dwords[addr], len * 2);
    return true;
    }
//==============================================================================
bool GetSignature(uint16 addr, uint16* pxDsn, uint16 len)
    {
    void *pxFlash = (void*) (SIGNATURE_LOCATION);
    pxFlash = (void*) ((int) pxFlash + addr * 2);
    CopyDataBytes((uint8*) pxFlash, (uint8*) pxDsn, len * 2);
    return true;

    }
//==============================================================================
bool GetEeprom(uint16 addr, uint16* pxDsn, uint16 len)
    {
    atcResetCounter();
    if (AT91F_TWI_Read(EEPROM_DADR, addr * 2, (uint8*) pxDsn, len * 2) == 0)
	{
	return true;
	}
    return false;
    }
//==============================================================================
bool SetEeprom(uint16 addr, uint16 *pxSrc, uint16 len)
    {
    atcResetCounter();
    if (AT91F_TWI_Write(EEPROM_DADR, addr * 2, (uint8*) pxSrc, len * 2) == 0)
	{
	return true;
	}

    return false;
    }

//==============================================================================
bool GetRtc(uint16 addr, uint16* pxDsn, uint16 len)
    {
    DATATIME temprtc[2];
    char iFixstr[16];

    if (addr >= 0x20)
	{
	rtcGetiFixDateTime(iFixstr);
	CopyDataBytes((uint8*) ((uint8*) (iFixstr)), (uint8*) pxDsn, len * 2);
	}
    else
	{
	rtcGetDataTime(temprtc);
	CopyDataBytes((uint8*) ((uint8*) (temprtc) + addr * 2), (uint8*) pxDsn,
		len * 2);
	}
    atcResetCounter();
    return true;
    }
//==============================================================================
bool SetRtc(uint16 addr, uint16 *pxSrc, uint16 len)
    {
    DATATIME temprtc[2];
    char iFixstr[16];

    if (addr >= 0x20)
	{
	rtcGetiFixDateTime(iFixstr);
	CopyDataBytes((uint8*) pxSrc, (uint8*)iFixstr,len * 2);
	rtcSetiFixDateTime(iFixstr);
	}
    else
	{
	rtcGetDataTime(temprtc);
	CopyDataBytes((uint8*) pxSrc, (uint8*) ((uint8*) (temprtc) + addr * 2),
		len * 2);
	rtcSetDataTime(temprtc);
	}
    atcResetCounter();
    return true;
    }
//==============================================================================


//==============================================================================
bool GetFlash(uint16 addr, uint16* pxDsn, uint16 len)
    {
    void *pxFlash = (void*) (DEVICE_FLASHMEM_LOCATION);
    pxFlash = (void*) ((int) pxFlash + addr * 2);
    CopyDataBytes((uint8*) pxFlash, (uint8*) pxDsn, len * 2);
    atcResetCounter();
    return true;
    }
//==============================================================================
bool SetFlash(uint16 addr, uint16 *pxSrc, uint16 len)
    {
    return Flash_WriteBuffer(DEVICE_FLASHMEM_LOCATION + addr * 2,
	    (uint8 *) pxSrc, len * 2);
    atcResetCounter();

    }

