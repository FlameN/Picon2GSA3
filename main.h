//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : main.h
//* Object              : main application written in C
//* Creation            :
//* 1.0 05/Jun/04 JOO   : Creation
//*----------------------------------------------------------------------------

#ifndef Main_h
#define Main_h

#include <stdio.h>

extern volatile char message[80];


//* ----------------------- External Function Prototype -----------------------

extern void Usart_init (void);
extern void AT91F_US_Printk( char *buffer);
extern void AT91F_US_Print_frame(char *buffer, unsigned short counter);
extern void AT91F_US_Print_2_frame(char *buffer, unsigned short counter,char *buffer2,unsigned short counter2);
extern  unsigned int AT91F_US_PDC_status (void);
extern void AT91F_US_PDC (char *buffer, unsigned int size, unsigned short block_size);
extern void AT91F_US_STT0 (void);


extern void AT91F_DBGU_Init(void);
extern unsigned int AT91F_DBGU_Get( char *val);
extern void AT91F_DBGU_scanf(char * type,unsigned int * val);
extern void AT91F_DBGU_Printk(char *);

#endif /* Main_h */
