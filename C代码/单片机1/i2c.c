#include "i2c.h"
void Delay10us()
{
	unsigned char a,b;
	for(b=1;b>0;b--)
		for(a=2;a>0;a--);

}
void I2cStart()
{
	SDA=1;
	Delay10us();
	SCL=1;
	Delay10us();//����ʱ����SDA����ʱ��>4.7us
	SDA=0;
	Delay10us();//����ʱ����>4us
	SCL=0;			
	Delay10us();		
}
void I2cStop()
{
	SDA=0;
	Delay10us();
	SCL=1;
	Delay10us();//����ʱ�����4.7us
	SDA=1;
	Delay10us();		
}
unsigned char I2cSendByte(unsigned char dat)
{
	unsigned char a=0,b=0;//���255��һ����������Ϊ1us�������ʱ255us��		
	for(a=0;a<8;a++)//Ҫ����8λ�������λ��ʼ
	{
		SDA=dat>>7;	 //��ʼ�ź�֮��SCL=0�����Կ���ֱ�Ӹı�SDA�ź�
		dat=dat<<1;
		Delay10us();
		SCL=1;
		Delay10us();//����ʱ��>4.7us
		SCL=0;
		Delay10us();//ʱ�����4us		
	}
	SDA=1;
	Delay10us();
	SCL=1;
	while(SDA)//�ȴ�Ӧ��Ҳ���ǵȴ����豸��SDA����
	{
		b++;
		if(b>200)	 //�������2000usû��Ӧ����ʧ�ܣ�����Ϊ��Ӧ�𣬱�ʾ���ս���
		{
			SCL=0;
			Delay10us();
			return 0;
		}
	}
	SCL=0;
	Delay10us();
 	return 1;		
}
unsigned char I2cReadByte()
{
	unsigned char a=0,dat=0;
	SDA=1;			//��ʼ�ͷ���һ���ֽ�֮��SCL����0
	Delay10us();
	for(a=0;a<8;a++)//����8���ֽ�
	{
		SCL=1;
		Delay10us();
		dat<<=1;
		dat|=SDA;
		Delay10us();
		SCL=0;
		Delay10us();
	}
	return dat;		
}
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa0);//����д������ַ
	I2cSendByte(addr);//����Ҫд���ڴ��ַ
	I2cSendByte(dat);	//��������
	I2cStop();
}
unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2cStart();
	I2cSendByte(0xa0); //����д������ַ
	I2cSendByte(addr); //����Ҫ��ȡ�ĵ�ַ
	I2cStart();
	I2cSendByte(0xa1); //���Ͷ�������ַ
	num=I2cReadByte(); //��ȡ����
	I2cStop();
	return num;	
}
