#include "reg52.h"
#include "i2c.h"//i2c����ʱ����غ���
#include "lcd.h"//lcdʱ����غ���
#include "intrins.h"
typedef unsigned int u16;
typedef unsigned char u8;
#define GPIO_KEY P1//�궨��P1�ھ������
sbit shcp=P0^0;//λ������λ�Ĵ���ʱ��
sbit ds=P0^1;		//λ���崮����������
sbit stcp=P0^2;	//λ����洢�Ĵ���ʱ��
sbit rstpwd=P3^2;//λ�������ð���
sbit sounder=P1^7;//λ���������
sbit relay=P2^4;//λ����̵���
u8 code entpwd[]="Enter Password:";//����������ʾ����
u8 code pwdcorrect[]="Pwd Correct!";//������ȷ
u8 code resetpwd[]="Reseting Password:";//��������
u8 code pwdnew[]="New Password:";//������
u8 code cnfmpwd[]="Confirm Password:";//ȷ������
u8 code notequal[]="Input Error!";//�������
u8 code entrstmode[]="Reset Pwd";//��������
u8 code pwderror[]="Wrong Pwd!";//�������
u8 code rstscd[]="Reset Succeed!";//���óɹ�
u8 code excderrtime[]="Wait 15 mins";//�ȴ�15����
u8 code resetunavail[]="Please wait 15 mins!";//�ȴ�15����
u8 code pwdchart[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};	//�������
u8 code anode[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};//������������
u8 code bitselect[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//74HC138��ѡ����
u8 code convdigit[]={'0','1','2','3','4','5','6','7','8','9'};
u8 keyPos=0,keyValue=-1,insData[8],recPwd[8],newPwd[8],confPwd[8],intFlag=0,wrongtime;//keyPos�洢LED����ܶ�ѡλ��,keyValue�����������������ֵ,insData����8λ����ʱ�������룬recPwd�����AT24C02C��ȡ8λ����ֵ,newPwd��confPwd�ֱ𱣴���������ʱ�������ȷ���������������,intFlag���жϱ�־,Ϊ0ʱMCUδ�����ж�,Ϊ1ʱ�ж�����Ӧ
//��ʱ����
void delay(u16 i){
	while(i--);
}
void delay1s(void)   //��ʱ1�뺯�� ��� 0us
{
    unsigned char a,b,c;
    for(c=167;c>0;c--)
        for(b=171;b>0;b--)
            for(a=16;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}

void delay20ms(void)   //��ʱ20ms�������ɸ���������� ��� 0us
{
    unsigned char a,b;
    for(b=215;b>0;b--)
        for(a=45;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
    _nop_();  //if Keil,require use intrins.h
}
void delay5s(void)   //��ʱ5�� ��� 0us
{
    unsigned char a,b,c;
    for(c=165;c>0;c--)
        for(b=100;b>0;b--)
            for(a=150;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
    _nop_();  //if Keil,require use intrins.h
}
void delay15min(void)   //��� 0us
{
    unsigned char a,b,c,d;
    for(d=212;d>0;d--)
        for(c=104;c>0;c--)
            for(b=119;b>0;b--)
                for(a=170;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}
//��������:������ȷ����������
void sounderCorrect(){
	u8 i;
	//C
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(238);//
	}
	delay(500);	
	//E
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(189);
	}
	delay(500);
	//G
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(159);
	}
		delay(500);
	for(i=0;i<150;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(119);
	}
	sounder=0;		
} 
void sounderIncorrect(){
	u8 i;
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<100;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	sounder=0;
}
void sounderIncorrect3(){
	u8 i;
	for(i=0;i<200;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	delay(1000);
	for(i=0;i<220;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
	sounder=0;//�رշ�����
} 	
void Send74HC595(u8 dat){
	u8 i;
	for(i=0;i<8;i++){
		ds=dat>>7;//��8λ��������7λ�������λ
		dat<<=1;//��������һλ���θ�λ�����λ
		shcp=0;//��λ�Ĵ���ʱ���õ͵�ƽ
		_nop_();//����һ����������
		_nop_();//����һ����������
		shcp=1;//��λ�Ĵ���ʱ�Ӹߵ�ƽ
	}
	stcp=0;//�洢�Ĵ����õ͵�ƽ
	_nop_();//����һ����������
	_nop_();//����һ����������
	stcp=1;//�洢�Ĵ����øߵ�ƽ
}

//�������ܣ�����������봫��P1��
u8 keyproc(){
	//a�����ּ�������ÿ�ΰ���������ʱ����0
	u16 a=0,i;
	 GPIO_KEY=0x07;	//���з�ת��ɨ�谴�� P1.0~1.2�ܽŽ��а������Ƚ��йܽ��øߵ�ƽ ��������0000 0111
	 //����а��������£����µ��а����ܽ�Ϊ�͵�ƽ
	 if(GPIO_KEY!=0x07){
	 	delay(1000);//����
		if(GPIO_KEY!=0x07){//��������а���ȷʵ������
			switch(GPIO_KEY){//�ж���һ��
				case 0x06://������0000 0110 ����һ���а��������� 
					keyValue=1;//�����ǰ���1������
					break;
				case 0x05://0000 0101 �ڶ��б�����
					keyValue=2;//����2������
					break;
				case 0x03://0000 0011
					keyValue=3;//���谴��3������
					break;
			}
			GPIO_KEY=0x78;//��������а����øߵ�ƽ��P1.3~P1.7Ϊ�ߵ�ƽ����0111 1000
			//�������ּ�������1~9�������У�ÿ�а���������ȣ��ж���������ֻ���ڵ�һ�б������������ϼ���3�ı�������
			switch(GPIO_KEY){
				case 0x68://0110 1000 �ڶ��а���������
					keyValue+=3;//keyValueֵ����3*1
					break;
				case 0x58://0101 1000 �����б�����
					keyValue+=6;//keyValueֵ����3*2��
					break;
				case 0x38://0011 1000 �����б�����
					keyValue+=9;//keyValue����3*3��
					break;
			}
			//���ڰ���0�������ڵ����еڶ��У�����ʱkeyValueֵΪ11�������ж�
			if(keyValue==11){
				keyValue=0;//keyValueֵ��0
			}
			for(i=0;i<200;i++){
				sounder=~sounder;//��������ƽȡ��
				delay(94);
				}
			//ʶ���û����ַ�ֹ�����,������������ʱ�ӳ�500ms�����ӳ��㹻��ʱ��δ�����˳�ѭ��
			while(a<50&&GPIO_KEY!=0x78){
				delay(50000);//�ӳ�500ms
				a++;//a����
			}
		}
	 }
	 return pwdchart[keyValue];//���ذ���ʮ������ֵ
}
//�������ܣ���ʼ������
void UsartInit(){
	PCON=0x80;	//���ʿ��ƼĴ���SMOD��Ƶ��1
	SCON=0x50;	//���п��ƼĴ���SM0 0 SM1 1������ʽ2 REN�������ж���1
	TMOD=0x20; //01������ʽ2
	TH1=0xF3;//������4800 SMOD��Ƶ ����12MHz
	TL1=0xF3;
	TR1=1;//��Timer1�ж�
	ES=1;//�����ж�
	EA=1;//���ж�
}
//�������ܣ��ϵ��E2PROM��ȡ����
void getPassword(){
	u8 i;
	for(i=0;i<8;i++){
		recPwd[i]=At24c02Read(i);//��AT24C02��ȡ1λʮ��������д��recPwd����
	}			
}
//�������ܣ���ʼ���ⲿ�ж�0
void Int0Init(){
	EA=1;//��51��Ƭ�����ж�
	EX0=1;//��Int0�ж�
	IT0=1;//������ʽ�½��ش���
}
void Timer0Init(){
	TMOD|=0x02;//��ʱ��0������ʽ2
	TH0=0xF3;//
	TL0=0xF3;
	TR0=1;
}
//��������:�����ⲿ�ж�0���������룬Ҫ���û�ȷ�ϵ�ǰ���룬�������������벢ȷ�ϣ��������������ͬ�������봫��U3��д��EEPROM
void Int0()interrupt 0{
	delay(1000);//���ð�������
	if(rstpwd==0){
	u8 i,flag=0,confflag=0;
	for(i=0;i<200;i++){
		sounder=~sounder;//��������ƽȡ��
		delay(94);
	}
		if(wrongtime==3){
			LcdWriteCom(0x80);//LCD1602ָ���õ�һ��
			LcdWriteCom(0x01);//LCD1602����ָ��
			for(i=0;i<20;i++){
				LcdWriteData(resetunavail[i]);//ѭ����LCD1602д�뵱ǰ��������������ʾʹ����
			}
			delay15min();//��ʱ15����
			return;//����ֵ��	
		}
		keyPos=0;//ָ���־��0
		LcdWriteCom(0x80);//LCD1602ָ���õ�һ��
		LcdWriteCom(0x01);//��յ�һ��
		LcdWriteCom(0x80+0x40);//LCDָ���õڶ��п�ͷ
		LcdWriteCom(0x01);//��յڶ���
		LcdWriteCom(0x80);//LCD1602ָ���õ�һ�п�ͷ
		for(i=0;i<20;i++){
			LcdWriteData(resetpwd[i]);//ѭ��д������������ʾ
		}
		//ȷ�ϵ�ǰ����
		LcdWriteCom(0x80+0x40);//LCDָ���õڶ���
		while(keyPos<8){//ѭ��д��8��
			GPIO_KEY=0x07;//����������������øߵ�ƽ 0000 0111
			while(GPIO_KEY==0x07);//���û�б����±��ֵȴ�
			insData[keyPos]=keyproc();//������ֵ��������
			LcdWriteData('*');//����������LCD��Ļ����*���汣֤��˽
			keyPos++;//д����һλ����		
		}
		//У������
		for(i=0;i<8;i++){
		 	if(insData[i]!=recPwd[i]){//��������������ȡ������ĵ�iλ����
		 		flag=1;//��־��1
				break;//����ѭ��
		 	}	
		}
		if(flag==0){//�������������Ԥ���������
				keyPos=0;//����λ�ù���
				LcdWriteCom(0x80);//LCD1602����õ�һ��
				LcdWriteCom(0x01);//����ָ��
				for(i=0;i<15;i++){
					LcdWriteData(pwdcorrect[i]);//ѭ��д��������ȷ����ʾ
				}
				delay5s();//��ʱ5��
				LcdWriteCom(0x80);//LCD1602����õ�һ��
				LcdWriteCom(0x01);//����ָ��
				for(i=0;i<20;i++){
					LcdWriteData(pwdnew[i]);//ѭ��д���������������ʾ
				}
				LcdWriteCom(0x80+0x40);//LCD����õڶ���
				//����������
		while(keyPos<8){
			GPIO_KEY=0x07;
			while(GPIO_KEY==0x07);//��û�а���������ʱ����ѭ��
			newPwd[keyPos]=keyproc();//������ֵ��������
			LcdWriteData('*');//����������LCD��Ļ����*���汣֤��˽
			keyPos++;		
		}
		//�ٴ�����������
		LcdWriteCom(0x80);//LCD��궨λ��һ��
		LcdWriteCom(0x01);//����ָ��
		for(i=0;i<20;i++){
			LcdWriteData(cnfmpwd[i]);//ѭ��д��ȷ�����������ʾ
		}
		LcdWriteCom(0x80+0x40);//��궨λ�ڶ���
		keyPos=0;//�����־��0
		while(keyPos<8){
			GPIO_KEY=0x07;
			while(GPIO_KEY==0x07);
			confPwd[keyPos]=keyproc();
			LcdWriteData('*');
			keyPos++;		
		}
		//У������
		for(i=0;i<8;i++){
			if(newPwd[i]!=confPwd[i]){//�����һ��������������ڶ����ĳһλ����
		  		confflag=1;//��־��1
					break;//����ѭ��
			}
		}
		if(confflag==0){//�������������������ͬ
			LcdWriteCom(0x80);//LCD��궨λ��һ��
			LcdWriteCom(0x01);//����ָ��
			for(i=0;i<20;i++){
				LcdWriteData(rstscd[i]);//��ʾ�û���������ɹ�	
			}
		//��E2PROMд��������
			for(i=0;i<8;i++){
				At24c02Write(i,newPwd[i]);//��AT24C02�ĵ�i��λ��д���������iλ
				delay20ms();//Ԥ��20ms���ڻ���ÿ��д��24C02
			}
		}
		
			}	
		else{//����������벻ͬ
			LcdWriteCom(0x80);//LCD1602����õ�һ��
			LcdWriteCom(0x01);//����ָ��
			for(i=0;i<20;i++){
				LcdWriteData(notequal[i]);//��ʾ�û���������
			}
			sounderIncorrect();
			}
		}
		delay1s();//��ʱ1��
		intFlag=1;//�жϱ�־����intFlag��1
}

void main(){
	u8 i,flag;
	LcdInit();//LCD1602��ʼ��
	Int0Init();//�ⲿ�ж�0��ʼ��
	Timer0Init();//��ʱ��0��ʼ��
	UsartInit();//�����жϳ�ʼ��
	//E2PROMд���ʼ����
	for(i=0;i<8;i++){
		At24c02Write(i,0x01);//��ʼ����11111111
		delay20ms();
	}
	while(1){
		//�̵����ߵ�ƽ
		relay=1;
		loop:	
		flag=0;//��������־����
		keyPos=0;//��������λ������
		getPassword(); 		//��ȡ����
		LcdWriteCom(0x80);//LCDָ���õ�һ��
		for(i=0;i<21;i++){
			LcdWriteData(entpwd[i]);//LCD��ʾ������������
		}
		//ѭ����������
		LcdWriteCom(0x80+0x40);//LCD����õڶ���
	   	while(keyPos<8){
			GPIO_KEY=0x07;//������������øߵ�ƽ 0000 0111=0x07
			while(GPIO_KEY==0x07);//�ȴ���������
			if(intFlag==1){//�������ִ��ǰ��Ӧ���ж�
				intFlag=0;//��Ӧ�жϱ�־��0
				getPassword();//��ȡ������
				goto loop;//��תѭ����ͷ
			}
			insData[keyPos]=keyproc();//��ȡ������ֵ
			LcdWriteData('*');//�����������ǺŴ���
			keyPos++;		
		}
		//У������
		for(i=0;i<8;i++){
			if(recPwd[i]!=insData[i]){//�������������EEPROM�������벻ͬ
				flag=1;//��־��1
				break;//����ѭ��
			}
		}
		LcdWriteCom(0x01);//����ָ��
		LcdWriteCom(0x80);//LCD����õ�һ��
		LcdWriteData(0x01);//����
		//������ȷ
		if(flag==0){	
			for(i=0;i<12;i++){
				LcdWriteData(pwdcorrect[i]);//��ʾ�û�������ȷ
			}
			sounderCorrect();//��������ʾ������ȷ
			relay=0;//�ܽŵ͵�ƽ���̵�����ͨ
			delay1s();//��ʱ1��
			wrongtime=0;//���������0
		}
		//�������
		else{
			wrongtime++;//�����������1
			for(i=0;i<20;i++){
				LcdWriteData(pwderror[i]);//��ʾ���������������
			}
			sounderIncorrect();//��������������
			delay5s();//��ʱ5��
		}
		if(wrongtime==3){
			LcdWriteCom(0x80);//LCDָ���Ƶ���һ��
			LcdWriteCom(0x01);//����ָ��
			for(i=0;i<20;i++){
				LcdWriteData(excderrtime[i]);	//��ʾ���˳�����������������
			}
			sounderIncorrect3();//����������	
		   	delay15min();//��ʱ15����
			wrongtime=0;//��������������
		}
	}
}