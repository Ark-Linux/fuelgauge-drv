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

int bq40z50_i2c_write(unsigned char dev_addr, unsigned char reg, unsigned char *val, unsigned char data_len);
int bq40z50_i2c_read(unsigned char addr, unsigned char *reg_w_list, unsigned char reg_w_len, unsigned char *val, unsigned char data_len);
int bq40z50_exec_4CC_Cmd(unsigned char *cmd_ptr, unsigned char *cmd_data_in_ptr, unsigned char cmd_data_in_length, unsigned char *cmd_data_out_ptr, unsigned char cmd_data_out_length);


#endif
