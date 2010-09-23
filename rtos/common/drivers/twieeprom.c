
#include "twi.h"
#include "twieeprom.h"
#include "crc.h"
#include "error.h"

#define EXEEPROM_SAFE_PAGE   16
/////////////////////////////
//--------------------------------------------
//
//==========================================================================================
bool TwiEepromRead(uint16 addr, uint8 *buf, uint16 size)
  {
    uint8 tBuf[(EXEEPROM_SAFE_PAGE + 2)];
    int i;
    bool Noerror = true;
    while (size)
      {
        AT91F_TWI_Read(EEPROM_DEV_ADDRESS,((addr/EXEEPROM_SAFE_PAGE)*(EXEEPROM_SAFE_PAGE+2)),tBuf,(EXEEPROM_SAFE_PAGE+2));
        i = EXEEPROM_SAFE_PAGE - (addr%EXEEPROM_SAFE_PAGE);
        if (i > size)
          {
            i = size;
            size = 0;
          }
        else
          {
            size -= i;
          }
        while (i--)
          {
            *buf = tBuf[(addr % EXEEPROM_SAFE_PAGE)];
            buf++;
            addr++;
          }
        if (!CheckCRC(tBuf, (EXEEPROM_SAFE_PAGE + 2)))
          {
            Noerror = false;
          }
      }
    return Noerror;
  }
//==========================
bool TwiEepromWrite(uint16 addr, uint8 *buf, uint16 size)
  {

    uint8 tBuf[(EXEEPROM_SAFE_PAGE + 2)];
    int i;
    while (size)
      {
        AT91F_TWI_Read(EEPROM_DEV_ADDRESS,((addr/EXEEPROM_SAFE_PAGE)*(EXEEPROM_SAFE_PAGE+2)),
            tBuf,(EXEEPROM_SAFE_PAGE+2));
        i = EXEEPROM_SAFE_PAGE - (addr % EXEEPROM_SAFE_PAGE);
        if (i > size)
          {
            i = size;
            size = 0;
          }
        else
          {
            size -= i;
          }
        while (i--)
          {
            tBuf[(addr % EXEEPROM_SAFE_PAGE)] = *buf;
            buf++;
            addr++;
          }
        SetCRC(tBuf, EXEEPROM_SAFE_PAGE);
        AT91F_TWI_Read      (EEPROM_DEV_ADDRESS,((addr - 1) / EXEEPROM_SAFE_PAGE) * (EXEEPROM_SAFE_PAGE+ 2),
          tBuf,(EXEEPROM_SAFE_PAGE+2));
    }
  return true;
}


