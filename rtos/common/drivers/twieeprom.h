//********************************************************
//    ������      : Picon-T1.
//    ������      : 1.0.
//    ����        : 29.03.2005
//    �����       : ����������� ���� ��������.
//    ��������    : ����
//    ����        :
//    ����������� :
//    ����� ���������� ; IAR EWB Version :iarc320c
//*********************************************************
//    ���������           : ATmega128
//    ������� ����������  : 9,216000 MHz
//    ��� ���������       : ����������
//    ������ ������       :
//*********************************************************
//extern unsigned char   ExEEPROM_Read(unsigned int, unsigned char*,unsigned char);
//extern unsigned char   ExEEPROM_Write(unsigned int, unsigned char*,unsigned char);
#ifndef  _TWIEEPROM
#define  _TWIEEPROM
#include "typedef.h"

#define EEPROM_DEV_ADDRESS      (8)

extern bool TwiEepromRead(uint16, uint8 *, uint16);
extern bool TwiEepromWrite(uint16, uint8 *, uint16);

#endif










