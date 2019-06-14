#ifndef __LCD_H_
#define __LCD_H_

#include<reg52.h>
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint 
#define uint unsigned int
#endif
#define LCD1602_DATAPINS P0
sbit LCD1602_E=P0^5;
sbit LCD1602_RW=P0^4;
sbit LCD1602_RS=P0^3;
void Lcd1602_Delay1ms(uint c);   //误差 0us
void LcdWriteCom(uchar com);//LCD1602写入8位命令子函数
void LcdWriteData(uchar dat);//LCD1602写入8位数据子函数*
void Send74HC595(unsigned char dat);//74HC595写入函数	
void LcdInit();//LCD1602初始化子程序							  

#endif
