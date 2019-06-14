#include "reg52.h"
#include "i2c.h"//i2c总线时序相关函数
#include "lcd.h"//lcd时序相关函数
#include "intrins.h"
typedef unsigned int u16;
typedef unsigned char u8;
#define GPIO_KEY P1//宏定义P1口矩阵键盘
sbit shcp=P0^0;//位定义移位寄存器时钟
sbit ds=P0^1;		//位定义串行数据输入
sbit stcp=P0^2;	//位定义存储寄存器时钟
sbit rstpwd=P3^2;//位定义重置按键
sbit sounder=P1^7;//位定义蜂鸣器
sbit relay=P2^4;//位定义继电器
u8 code entpwd[]="Enter Password:";//输入密码提示数组
u8 code pwdcorrect[]="Pwd Correct!";//密码正确
u8 code resetpwd[]="Reseting Password:";//重置密码
u8 code pwdnew[]="New Password:";//新密码
u8 code cnfmpwd[]="Confirm Password:";//确认密码
u8 code notequal[]="Input Error!";//输入错误
u8 code entrstmode[]="Reset Pwd";//重置密码
u8 code pwderror[]="Wrong Pwd!";//密码错误
u8 code rstscd[]="Reset Succeed!";//重置成功
u8 code excderrtime[]="Wait 15 mins";//等待15分钟
u8 code resetunavail[]="Please wait 15 mins!";//等待15分钟
u8 code pwdchart[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};	//密码码表
u8 code anode[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x83,0xc6,0xa1,0x86,0x8e};//共阳数码管码表
u8 code bitselect[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};//74HC138段选数组
u8 code convdigit[]={'0','1','2','3','4','5','6','7','8','9'};
u8 keyPos=0,keyValue=-1,insData[8],recPwd[8],newPwd[8],confPwd[8],intFlag=0,wrongtime;//keyPos存储LED数码管段选位置,keyValue保存键盘输入密码数值,insData保存8位解锁时输入密码，recPwd保存从AT24C02C读取8位密码值,newPwd和confPwd分别保存重置密码时新密码和确认密码的两次输入,intFlag是中断标志,为0时MCU未产生中断,为1时中断已响应
//延时函数
void delay(u16 i){
	while(i--);
}
void delay1s(void)   //延时1秒函数 误差 0us
{
    unsigned char a,b,c;
    for(c=167;c>0;c--)
        for(b=171;b>0;b--)
            for(a=16;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}

void delay20ms(void)   //延时20ms函数，由辅助软件生成 误差 0us
{
    unsigned char a,b;
    for(b=215;b>0;b--)
        for(a=45;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
    _nop_();  //if Keil,require use intrins.h
}
void delay5s(void)   //延时5秒 误差 0us
{
    unsigned char a,b,c;
    for(c=165;c>0;c--)
        for(b=100;b>0;b--)
            for(a=150;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
    _nop_();  //if Keil,require use intrins.h
}
void delay15min(void)   //误差 0us
{
    unsigned char a,b,c,d;
    for(d=212;d>0;d--)
        for(c=104;c>0;c--)
            for(b=119;b>0;b--)
                for(a=170;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}
//函数功能:密码正确蜂鸣器发声
void sounderCorrect(){
	u8 i;
	//C
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(238);//
	}
	delay(500);	
	//E
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(189);
	}
	delay(500);
	//G
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(159);
	}
		delay(500);
	for(i=0;i<150;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(119);
	}
	sounder=0;		
} 
void sounderIncorrect(){
	u8 i;
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<100;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	sounder=0;
}
void sounderIncorrect3(){
	u8 i;
	for(i=0;i<200;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<200;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	delay(1000);
	for(i=0;i<220;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
	sounder=0;//关闭蜂鸣器
} 	
void Send74HC595(u8 dat){
	u8 i;
	for(i=0;i<8;i++){
		ds=dat>>7;//将8位数据右移7位保留最高位
		dat<<=1;//数据左移一位，次高位变最高位
		shcp=0;//移位寄存器时钟置低电平
		_nop_();//闲置一个机器周期
		_nop_();//闲置一个机器周期
		shcp=1;//移位寄存器时钟高电平
	}
	stcp=0;//存储寄存器置低电平
	_nop_();//闲置一个机器周期
	_nop_();//闲置一个机器周期
	stcp=1;//存储寄存器置高电平
}

//函数功能：矩阵键盘输入传送P1口
u8 keyproc(){
	//a是松手检测参数，每次按键被按下时先清0
	u16 a=0,i;
	 GPIO_KEY=0x07;	//行列反转法扫描按键 P1.0~1.2管脚接列按键，先将列管脚置高电平 即二进制0000 0111
	 //如果有按键被按下，按下的列按键管脚为低电平
	 if(GPIO_KEY!=0x07){
	 	delay(1000);//消抖
		if(GPIO_KEY!=0x07){//矩阵键盘列按键确实被按下
			switch(GPIO_KEY){//判断哪一列
				case 0x06://二进制0000 0110 即第一列有按键被按下 
					keyValue=1;//假设是按键1被按下
					break;
				case 0x05://0000 0101 第二列被按下
					keyValue=2;//按键2被按下
					break;
				case 0x03://0000 0011
					keyValue=3;//假设按键3被按下
					break;
			}
			GPIO_KEY=0x78;//矩阵键盘行按键置高电平，P1.3~P1.7为高电平，即0111 1000
			//由于数字键盘区域1~9三行三列，每行按键数量相等，判断输入数字只需在第一行被按下数基础上加上3的倍数即可
			switch(GPIO_KEY){
				case 0x68://0110 1000 第二行按键被按下
					keyValue+=3;//keyValue值加上3*1
					break;
				case 0x58://0101 1000 第三行被按下
					keyValue+=6;//keyValue值加上3*2行
					break;
				case 0x38://0011 1000 第四行被按下
					keyValue+=9;//keyValue加上3*3行
					break;
			}
			//由于按键0被安排在第四行第二列，按下时keyValue值为11，单独判断
			if(keyValue==11){
				keyValue=0;//keyValue值归0
			}
			for(i=0;i<200;i++){
				sounder=~sounder;//蜂鸣器电平取反
				delay(94);
				}
			//识别用户松手防止误操作,当按键被按下时延迟500ms，当延迟足够长时间未松手退出循环
			while(a<50&&GPIO_KEY!=0x78){
				delay(50000);//延迟500ms
				a++;//a自增
			}
		}
	 }
	 return pwdchart[keyValue];//返回按键十六进制值
}
//函数功能：初始化串口
void UsartInit(){
	PCON=0x80;	//功率控制寄存器SMOD倍频置1
	SCON=0x50;	//串行控制寄存器SM0 0 SM1 1工作方式2 REN允许串行中断置1
	TMOD=0x20; //01工作方式2
	TH1=0xF3;//波特率4800 SMOD倍频 晶振12MHz
	TL1=0xF3;
	TR1=1;//打开Timer1中断
	ES=1;//串口中断
	EA=1;//总中断
}
//函数功能：上电从E2PROM读取密码
void getPassword(){
	u8 i;
	for(i=0;i<8;i++){
		recPwd[i]=At24c02Read(i);//从AT24C02读取1位十六进制数写入recPwd数组
	}			
}
//函数功能：初始化外部中断0
void Int0Init(){
	EA=1;//打开51单片机总中断
	EX0=1;//打开Int0中断
	IT0=1;//触发方式下降沿触发
}
void Timer0Init(){
	TMOD|=0x02;//定时器0工作方式2
	TH0=0xF3;//
	TL0=0xF3;
	TR0=1;
}
//函数功能:进入外部中断0，重设密码，要求用户确认当前密码，并且输入新密码并确认，如果两次输入相同将新密码传送U3并写入EEPROM
void Int0()interrupt 0{
	delay(1000);//重置按键消抖
	if(rstpwd==0){
	u8 i,flag=0,confflag=0;
	for(i=0;i<200;i++){
		sounder=~sounder;//蜂鸣器电平取反
		delay(94);
	}
		if(wrongtime==3){
			LcdWriteCom(0x80);//LCD1602指针置第一行
			LcdWriteCom(0x01);//LCD1602清屏指令
			for(i=0;i<20;i++){
				LcdWriteData(resetunavail[i]);//循环向LCD1602写入当前不可重置密码提示使用者
			}
			delay15min();//延时15分钟
			return;//返回值空	
		}
		keyPos=0;//指针标志置0
		LcdWriteCom(0x80);//LCD1602指针置第一行
		LcdWriteCom(0x01);//清空第一行
		LcdWriteCom(0x80+0x40);//LCD指针置第二行开头
		LcdWriteCom(0x01);//清空第二行
		LcdWriteCom(0x80);//LCD1602指针置第一行开头
		for(i=0;i<20;i++){
			LcdWriteData(resetpwd[i]);//循环写入重置密码提示
		}
		//确认当前密码
		LcdWriteCom(0x80+0x40);//LCD指针置第二行
		while(keyPos<8){//循环写入8次
			GPIO_KEY=0x07;//矩阵键盘三列引脚置高电平 0000 0111
			while(GPIO_KEY==0x07);//如果没有被按下保持等待
			insData[keyPos]=keyproc();//按键数值传入数组
			LcdWriteData('*');//输入数字在LCD屏幕上用*代替保证隐私
			keyPos++;//写入下一位密码		
		}
		//校验密码
		for(i=0;i<8;i++){
		 	if(insData[i]!=recPwd[i]){//如果输入密码与读取到密码的第i位不等
		 		flag=1;//标志置1
				break;//跳出循环
		 	}	
		}
		if(flag==0){//如果输入密码与预存密码相等
				keyPos=0;//输入位置归零
				LcdWriteCom(0x80);//LCD1602光标置第一行
				LcdWriteCom(0x01);//清屏指令
				for(i=0;i<15;i++){
					LcdWriteData(pwdcorrect[i]);//循环写入密码正确的提示
				}
				delay5s();//延时5秒
				LcdWriteCom(0x80);//LCD1602光标置第一行
				LcdWriteCom(0x01);//清屏指令
				for(i=0;i<20;i++){
					LcdWriteData(pwdnew[i]);//循环写入输入新密码的提示
				}
				LcdWriteCom(0x80+0x40);//LCD光标置第二行
				//输入新密码
		while(keyPos<8){
			GPIO_KEY=0x07;
			while(GPIO_KEY==0x07);//当没有按键被按下时保持循环
			newPwd[keyPos]=keyproc();//按键数值传入数组
			LcdWriteData('*');//输入数字在LCD屏幕上用*代替保证隐私
			keyPos++;		
		}
		//再次输入新密码
		LcdWriteCom(0x80);//LCD光标定位第一行
		LcdWriteCom(0x01);//清屏指令
		for(i=0;i<20;i++){
			LcdWriteData(cnfmpwd[i]);//循环写入确认新密码的提示
		}
		LcdWriteCom(0x80+0x40);//光标定位第二行
		keyPos=0;//输入标志置0
		while(keyPos<8){
			GPIO_KEY=0x07;
			while(GPIO_KEY==0x07);
			confPwd[keyPos]=keyproc();
			LcdWriteData('*');
			keyPos++;		
		}
		//校验密码
		for(i=0;i<8;i++){
			if(newPwd[i]!=confPwd[i]){//如果第一遍输入新密码与第二遍的某一位不等
		  		confflag=1;//标志置1
					break;//跳出循环
			}
		}
		if(confflag==0){//如果新密码两次输入相同
			LcdWriteCom(0x80);//LCD光标定位第一行
			LcdWriteCom(0x01);//清屏指令
			for(i=0;i<20;i++){
				LcdWriteData(rstscd[i]);//提示用户重置密码成功	
			}
		//向E2PROM写入新密码
			for(i=0;i<8;i++){
				At24c02Write(i,newPwd[i]);//向AT24C02的第i个位置写入新密码第i位
				delay20ms();//预留20ms用于缓冲每次写入24C02
			}
		}
		
			}	
		else{//如果两次输入不同
			LcdWriteCom(0x80);//LCD1602光标置第一行
			LcdWriteCom(0x01);//清屏指令
			for(i=0;i<20;i++){
				LcdWriteData(notequal[i]);//提示用户输入有误
			}
			sounderIncorrect();
			}
		}
		delay1s();//延时1秒
		intFlag=1;//中断标志参数intFlag置1
}

void main(){
	u8 i,flag;
	LcdInit();//LCD1602初始化
	Int0Init();//外部中断0初始化
	Timer0Init();//定时器0初始化
	UsartInit();//串口中断初始化
	//E2PROM写入初始密码
	for(i=0;i<8;i++){
		At24c02Write(i,0x01);//初始密码11111111
		delay20ms();
	}
	while(1){
		//继电器高电平
		relay=1;
		loop:	
		flag=0;//密码错误标志清零
		keyPos=0;//密码输入位置清零
		getPassword(); 		//获取密码
		LcdWriteCom(0x80);//LCD指针置第一行
		for(i=0;i<21;i++){
			LcdWriteData(entpwd[i]);//LCD提示主人输入密码
		}
		//循环输入密码
		LcdWriteCom(0x80+0x40);//LCD光标置第二行
	   	while(keyPos<8){
			GPIO_KEY=0x07;//矩阵键盘三列置高电平 0000 0111=0x07
			while(GPIO_KEY==0x07);//等待按键按下
			if(intFlag==1){//如果命令执行前响应过中断
				intFlag=0;//响应中断标志置0
				getPassword();//获取新密码
				goto loop;//跳转循环开头
			}
			insData[keyPos]=keyproc();//获取按键键值
			LcdWriteData('*');//输入密码用星号代替
			keyPos++;		
		}
		//校验密码
		for(i=0;i<8;i++){
			if(recPwd[i]!=insData[i]){//如果输入密码与EEPROM接受密码不同
				flag=1;//标志置1
				break;//跳出循环
			}
		}
		LcdWriteCom(0x01);//清屏指令
		LcdWriteCom(0x80);//LCD光标置第一行
		LcdWriteData(0x01);//清屏
		//密码正确
		if(flag==0){	
			for(i=0;i<12;i++){
				LcdWriteData(pwdcorrect[i]);//提示用户输入正确
			}
			sounderCorrect();//蜂鸣器提示密码正确
			relay=0;//管脚低电平，继电器导通
			delay1s();//延时1秒
			wrongtime=0;//错误次数清0
		}
		//密码错误
		else{
			wrongtime++;//错误计数增加1
			for(i=0;i<20;i++){
				LcdWriteData(pwderror[i]);//提示主人密码输入错误
			}
			sounderIncorrect();//蜂鸣器短鸣三次
			delay5s();//延时5秒
		}
		if(wrongtime==3){
			LcdWriteCom(0x80);//LCD指针移到第一行
			LcdWriteCom(0x01);//清屏指令
			for(i=0;i<20;i++){
				LcdWriteData(excderrtime[i]);	//提示主人超过最大输入次数限制
			}
			sounderIncorrect3();//蜂鸣器报警	
		   	delay15min();//延时15分钟
			wrongtime=0;//输入错误次数清零
		}
	}
}