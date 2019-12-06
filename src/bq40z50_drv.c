/**
*  @file      bq40z50-drv.c
*  @brief     bq40z50 SMBUS(i2c) drv
*  @author    Link Lin
*  @date      12 -2019
*  @copyright
*/

#include<stdio.h>
#include<fcntl.h>
#include <error.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<string.h>
#include<stdlib.h>

#include<linux/i2c.h>
#include<linux/i2c-dev.h>

#include "bq40z50_drv.h"

#define I2C_FILE_NAME   "/dev/i2c-5"
//#define I2C_ADDR        0x38
//#define I2C_ADDR        0x20

//the I2C addr will change, 0x38 or 0x20
unsigned int I2C_ADDR = 0x16;

static int fd;


//supposed to be Little-endian
static int check_endian(void)
{
    unsigned int x;
    unsigned char x0,x1,x2,x3;

    x=0x12345678;

    x0=((unsigned char *)&x)[0];  //low byte
    x1=((unsigned char *)&x)[1];
    x2=((unsigned char *)&x)[2];
    x3=((unsigned char *)&x)[3];  //high byte

    if(x0 == 0x12)
    {
        printf("Big-endian, x0=0x%x,x1=0x%x,x2=0x%x,x3=0x%x\n",x0,x1,x2,x3);
    }

    if(x0 == 0x78)
    {
        printf("Little-endian, x0=0x%x,x1=0x%x,x2=0x%x,x3=0x%x\n",x0,x1,x2,x3);
    }

    return 0;
}


static int i2c_open(unsigned char i2c_addr)
{
    int ret;

    int val;

    fd = open(I2C_FILE_NAME, O_RDWR);

    if(fd < 0)
    {
        perror("Unable to open i2c control file");

        return -1;
    }

    printf("open i2c file success %d\n",fd);

    ret = ioctl(fd, I2C_SLAVE_FORCE, i2c_addr);
    if (ret < 0)
    {
        printf("i2c: Failed to set i2c device address 0x%x\n",i2c_addr);
        return -1;
    }

    printf("i2c: set i2c device address success\n");

    val = 3;
    ret = ioctl(fd, I2C_RETRIES, val);
    if(ret < 0)
    {
        printf("i2c: set i2c retry times err\n");
    }

    printf("i2c: set i2c retry times %d\n",val);

    /*
    * use I2C_TIMEOUT default setting, which is HZ, that means 1 second
    */

    return 0;
}

static int i2c_write(int fd, unsigned char dev_addr, unsigned char *val, unsigned char len)
{
    int ret;
    int i;

    struct i2c_rdwr_ioctl_data data;

    struct i2c_msg messages;


    messages.addr = dev_addr;  //device address
    messages.flags = 0;    //write
    messages.len = len;
    messages.buf = val;  //data

    data.msgs = &messages;
    data.nmsgs = 1;

    ret = ioctl(fd, I2C_RDWR, &data);

    if(ret < 0)
    {
        printf("write ioctl err %d\n",ret);
        return ret;
    }

    printf("i2c write buf = ");
    for(i=0; i< len; i++)
    {
        printf("%02x ",val[i]);
    }
    printf("\n");

    return 0;
}


static int i2c_read(int fd, unsigned char addr, unsigned char *reg_w_list, unsigned char reg_w_len, unsigned char *val, unsigned char len)
{
    int ret;
    int i;

    struct i2c_rdwr_ioctl_data data;
    struct i2c_msg messages[2];

    messages[0].addr = addr;  //device address
    messages[0].flags = 0;    //write
    messages[0].len = reg_w_len;
    messages[0].buf = reg_w_list;  //reg address

    messages[1].addr = addr;       //device address
    messages[1].flags = I2C_M_RD;  //read
    messages[1].len = len;
    messages[1].buf = val;

    data.msgs = messages;
    data.nmsgs = 2;

    ret = ioctl(fd, I2C_RDWR, &data);

    if(ret < 0)
    {
        printf("read ioctl err %d\n",ret);

        return ret;
    }

    printf("i2c read buf = ");
    for(i = 0; i < len; i++)
    {
        printf("%02x ",val[i]);
    }
    printf("\n");

    return 0;
}


int bq40z50_i2c_write(unsigned char dev_addr, unsigned char reg, unsigned char *val, unsigned char data_len)
{
    unsigned char buf[80] = {0};
    int i;

    int ret;

    if(data_len + 2 >= 80)
    {
        printf("data_len_exceed\n");
        return -1;
    }

    buf[0] = reg;

    for(i = 0; i<data_len; i++)
    {
        buf[1+i] = val[i];
    }

    if(i2c_write(fd, dev_addr, buf, data_len+1) == 0)
    {
        return 0;
    }

    return -1;
}


int bq40z50_i2c_read(unsigned char addr, unsigned char *reg_w_list, unsigned char reg_w_len, unsigned char *val, unsigned char data_len)
{
    unsigned char buf[80] = {0};
    int i;

    if(data_len + 1 >= 80)
    {
        printf("data_len_exceed\n");
        return -1;
    }

    if(i2c_read(fd, addr, reg_w_list, reg_w_len, buf, data_len) == 0)
    {
        return 0;
    }

    return -1;
}


static int bq40z50_ManufacturerBlockAccess_Send(unsigned short w_data)
{
    int i;

    unsigned short Data = w_data;

    printf("bq40z50 ManufacturerBlockAccess Send 0x%04x\n", Data);

    return bq40z50_i2c_write(I2C_ADDR, ManufacturerBlockAccess_REG, (unsigned char*)(&Data), 2);
}


static int bq40z50_ManufacturerBlockAccess_Read(unsigned short r_data_reg)
{
    unsigned char write_val[3] = {0};
    unsigned char read_val[4] = {0};

    unsigned short r_data = 0;

    write_val[0] = ManufacturerBlockAccess_REG;
    write_val[1] = (unsigned char)(r_data_reg & 0xff);
    write_val[2] = (unsigned char)(r_data_reg >> 8);

    if(bq40z50_i2c_read(I2C_ADDR, write_val, 3, read_val, 4) == 0)
    {
        r_data = (read_val[3] << 8) | read_val[2];

        printf("bq40z50 ManufacturerBlockAccess Read Reg:0x%04x, Data:0x%04x\n", r_data_reg, r_data);
        return r_data;
    }

    printf("bq40z50 ManufacturerBlockAccess Read err\n");
    return -1;
}


int main(int argc, char* argv[])
{
    int i;

    unsigned short read_data;
    unsigned char buf[64];

    unsigned char reg;

    check_endian();

    if(argc > 1)
    {
        for(i = 0; i < argc; i++)
        {
            printf("Argument %d is %s\n", i, argv[i]);
        }

        if(strcmp(argv[1],"0x38") == 0)
        {
            //I2C_ADDR = 0x38;
        }
        else if(strcmp(argv[1],"0x20") == 0)
        {
            //I2C_ADDR = 0x20;
        }
    }

    printf("used i2c address is 0x%x\n",I2C_ADDR);


    if(i2c_open(I2C_ADDR) != 0)
    {
        return -1;
    }

    //test write
    bq40z50_ManufacturerBlockAccess_Send(0x0021);

    //test read
    read_data = bq40z50_ManufacturerBlockAccess_Read(0x0001);
    read_data = bq40z50_ManufacturerBlockAccess_Read(0x0006);

    //Temperature
    reg = 0x08;
    bq40z50_i2c_read(I2C_ADDR, &reg, 1, buf, 2);

    //Voltage
    reg = 0x09;
    bq40z50_i2c_read(I2C_ADDR, &reg, 1, buf, 2);

    //Current
    reg = 0x0A;
    bq40z50_i2c_read(I2C_ADDR, &reg, 1, buf, 2);


    close(fd);

    return 0;
}



