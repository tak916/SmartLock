#ifndef __I2C_H//���I2C.Hδ����
#define __I2C_H//����i2c.h
#include<reg52.h> //����52��Ƭ��ͷ�ļ�
sbit SCL=P3^4;
sbit SDA=P3^5;
void I2cStart();//I2C��д����ʱ����
void I2cStop();//I2Cֹͣ��дʱ����
unsigned char I2cSendByte(unsigned char dat);//I2Cд��һλʱ����
unsigned char I2cReadByte();//��ȡһλʱ����
void At24c02Write(unsigned char addr,unsigned char dat);//д��һ�ֽں���
unsigned char At24c02Read(unsigned char addr);//I2C��ȡһ�ֽں���
#endif