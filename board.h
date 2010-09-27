/*----------------------------------------------------------------------------
*         ATMEL Microcontroller Software Support  -  ROUSSET  -
*----------------------------------------------------------------------------
* The software is delivered "AS IS" without warranty or condition of any
* kind, either express, implied or statutory. This includes without
* limitation any warranty or condition with respect to merchantability or
* fitness for any particular purpose, or against the infringements of
* intellectual property rights of others.
*----------------------------------------------------------------------------
* File Name           : Board.h
* Object              : AT91SAM7X Evaluation Board Features Definition File.
*
* Creation            : JG   20/Jun/2005
*----------------------------------------------------------------------------
*/
#ifndef Board_h
#define Board_h

#include "AT91SAM7A3.h"
#include "ioat91sam7a3.h"
#include "lib_AT91SAM7A3.h"



#define true	-1
#define false	0


#define TESTBOARD

/*-------------------------------*/
/* SAM7Board Memories Definition */
/*-------------------------------*/
// The AT91SAM7A3 embeds a 32-Kbyte SRAM bank, and 256 K-Byte Flash

#define  INT_SARM           0x00200000
#define  INT_SARM_REMAP	    0x00000000

#define  INT_FLASH          0x00000000
#define  INT_FLASH_REMAP    0x01000000

#define  FLASH_PAGE_NB		1024
#define  FLASH_PAGE_LOCK    64
#define  FLASH_PAGE_SIZE	256

/*-----------------*/
/* Leds Definition */
/*-----------------*/
/*                                 PIO */
#define LED1            (1<<0)	/* PB0 */
#define LED2            (1<<1)	/* PB1 */
#define LED3            (1<<3)  /* PB3 */
#define LED4            (1<<4)  /* PB4 */
#define LED_485            (1<<4)  /* PB4 */

#define LED_WORK_RED(v)		{if(v != 0){AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED1)} \
							else {AT91F_PIO_CleanOutput(AT91C_BASE_PIOB, LED1)};
#define LED_WORK_GREEN(v)	{if(v != 0){AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED2)} \
							else{AT91F_PIO_CleanOutput(AT91C_BASE_PIOB, LED2)};

#define LED_MODE_RED(v)		{if(v != 0){AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED3)} \
							else {AT91F_PIO_CleanOutput(AT91C_BASE_PIOB, LED3)};
#define LED_MODE_GREEN(v)	{if(v != 0){AT91F_PIO_SetOutput(AT91C_BASE_PIOB, LED4)} \
							else{AT91F_PIO_CleanOutput(AT91C_BASE_PIOB, LED4)};

#define LED_MASK        (LED1|LED2|LED3|LED_485)

#define MODENABLE            AT91C_PIO_PB13

#define MODEM_RST            AT91C_PIO_PA5

#define MODEM_ENABLE { AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, MODEM_RST);}
#define  MODEM_DISABLE { AT91F_PIO_SetOutput(AT91C_BASE_PIOA, MODEM_RST);}

#define USART2_RTS           AT91C_PIO_PB27


#define OUTPIN_MASK      (MODENABLE|LED_MASK|USART2_RTS|MODEM_RST)
//#define DISIN_MASK       (DISKRET1)

/*------------------*/
/* USART Definition */
/*------------------*/
/* SUB-D 9 points DBGU*/
#define DBGU_RXD		AT91C_PA30_DRXD
#define DBGU_TXD		AT91C_PA31_DTXD
#define AT91C_DBGU_BAUD	   230400   // Baud rate


/*--------------*/
/* Master Clock */
/*--------------*/

#define EXT_OC          18432000   // Exetrnal ocilator MAINCK
#define MCK             47923200   // MCK (PLLRC div by 2)
#define MCKKHz          (MCK/1000) //

#endif /* Board_h */
