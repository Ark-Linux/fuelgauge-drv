/**
*  @file      bq40z50-drv.h
*  @brief     bq40z50
*  @author    Link Lin
*  @date      12 -2019
*  @copyright
*/

#ifndef _BQ40Z50_DRV_H_
#define _BQ40Z50_DRV_H_

#include<stdio.h>
#include<stdlib.h>


int fuelgauge_i2c_open(unsigned char i2c_addr);

int fuelgauge_get_Battery_Temperature(void);
int fuelgauge_get_Battery_Voltage(void);
int fuelgauge_get_Battery_Current(void);
int fuelgauge_get_RelativeStateOfCharge(void);
int fuelgauge_get_AbsoluteStateOfCharge(void);

#endif
