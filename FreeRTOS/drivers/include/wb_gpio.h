/****************************************************************************
*                                                                           *
* Copyright (c) 2009 Nuvoton Tech. Corp. All rights reserved.               *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   nuc930_gpio.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   GPIO library header file
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*
* HISTORY
*
* REMARK
*   None
****************************************************************************/
#ifndef _WB_GPIO_H_H
#define _WB_GPIO_H_H
#include "wb_include.h"


extern int gpio_open(unsigned char port);
extern int gpio_readport(unsigned char port, unsigned short *val);
extern int gpio_setportdir(unsigned char port, unsigned short mask, unsigned short dir);
extern int gpio_setportval(unsigned char port, unsigned short mask, unsigned short val);
extern int gpio_setportpull(unsigned char port, unsigned short mask, unsigned short pull);
extern int gpio_setdebounce(unsigned int clk, unsigned char src);
extern void gpio_getdebounce(unsigned int *clk, unsigned char *src);
extern int gpio_setsrcgrp(unsigned char port, unsigned short mask, unsigned char irq);
extern int gpio_getsrcgrp(unsigned char port, unsigned int *val);
extern int gpio_setintmode(unsigned char port, unsigned short mask, unsigned short falling, unsigned short rising);
extern int gpio_getintmode(unsigned char port, unsigned short *falling, unsigned short *rising);
extern int gpio_setlatchtrigger(unsigned char src);
extern void gpio_getlatchtrigger(unsigned char *src);
extern int gpio_getlatchval(unsigned char port, unsigned short *val);
extern int gpio_gettriggersrc(unsigned char port, unsigned short *src);
extern int gpio_cleartriggersrc(unsigned char port);

#endif
