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


#define  ManufacturerBlockAccess_REG    0x44


int bq40z50_get_Battery_Temperature(void);
int bq40z50_get_Battery_Voltage(void);
int bq40z50_get_Battery_Current(void);
int bq40z50_get_RelativeStateOfCharge(void);
int bq40z50_get_AbsoluteStateOfCharge(void);

#endif
