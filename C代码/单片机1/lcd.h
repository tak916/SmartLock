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
void Lcd1602_Delay1ms(uint c);   //��� 0us
void LcdWriteCom(uchar com);//LCD1602д��8λ�����Ӻ���
void LcdWriteData(uchar dat);//LCD1602д��8λ�����Ӻ���*
void Send74HC595(unsigned char dat);//74HC595д�뺯��	
void LcdInit();//LCD1602��ʼ���ӳ���							  

#endif
