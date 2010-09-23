#ifndef MODEM_H
#define MODEM_H
#include "typedef.h"
typedef enum
  {
  MA_UNKNOWN, MA_CALLREADY, MA_OK, MA_CONNECT, MA_NOCARRIER, MA_ERROR
  } MDEM_ANSWER;

typedef struct
  {
    char *str;
    MDEM_ANSWER ma;
  } AT_COMMAND;

#define PPP_USER                "bemn_7650654"
#define PPP_PASS                "7650654"
#define PPP_ADDSTR              "AT+CGDCONT=1,\"IP\",\"web1.velcom.by\"\r"

//#define PPP_USER                ""
//#define PPP_PASS                ""
//#define PPP_ADDSTR              "AT+CGDCONT=1,\"IP\",\"lidazhkh.velcom.by\"\r"

 extern  bool InitModem();
 extern void ModemWrite(char *);


#endif
