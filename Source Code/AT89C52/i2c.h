#ifndef __I2C_H//如果I2C.H未定义
#define __I2C_H//定义i2c.h
#include<reg52.h> //引用52单片机头文件
sbit SCL=P3^4;
sbit SDA=P3^5;
void I2cStart();//I2C读写启动时序函数
void I2cStop();//I2C停止读写时序函数
unsigned char I2cSendByte(unsigned char dat);//I2C写入一位时序函数
unsigned char I2cReadByte();//读取一位时序函数
void At24c02Write(unsigned char addr,unsigned char dat);//写入一字节函数
unsigned char At24c02Read(unsigned char addr);//I2C读取一字节函数
#endif