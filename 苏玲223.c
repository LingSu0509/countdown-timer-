#include<reg51.h>

#define LEDData P0

unsigned char code LEDCode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char KEY[]={0x77,0xb7};

unsigned char minute,second,count,z,sw,r=1,pat,reset,stop=1;        	//分，秒，计数器
//标志位z用于判断倒计时是否开始，标志位sw用于数码管选位来设置时间，pat是pat键的标志位，reset是reset键的标志位，stop用于识别pat位是否被按下
sbit secondpoint=P0^7;  //秒闪动点
sbit led1=P2^6;         //数码管选位1
sbit led2=P2^2;         //数码管选位2
sbit led3=P2^3;         //数码管选位3
sbit led4=P2^4;         //数码管选位4
sbit LED5=P2^0;         //LED1
sbit LED6=P2^1;         //LED2
sbit alam=P2^5;         //蜂鸣器
sbit add=P3^0;          //add按键，用于加时间
sbit sub=P3^1;          //sub按键，用于减时间
sbit onoff=P3^2;        //onoff按键，用于开始或暂停时间
sbit swit=P3^3;         //switch按键，用于数码管选位来加减时间
bit playon=0;		     //倒计时进行标志位，为1时表示计时开启
bit timeover=0;	     //倒计时结束标志位，为1时表示时间已经完
bit halfsecond=0;       //秒点闪动标志位


void Delay5ms(void)     //延时函数Delay5ms()
{
	unsigned int i;
	for(i=50;i>0;i--);
}
void time1(void)        //延时函数time1()
{
	unsigned int i;
	for(i=180;i>0;i--);
}
void Delay(void)       //延时函数time1Delay()
{
	unsigned int i;
	for(i=1000;i>0;i--);
}
void display(void)
{
//-----------显示时间分--------------
	LEDData=LEDCode[minute/10];
	led1=0;                    //显示数码管分钟的第一位
	Delay5ms();
	led1=1;
	LEDData=LEDCode[minute%10];
	led2=0;                    //显示数码管分钟的第二位
	Delay5ms();
	led2=1;
//-------------秒点---------------
	if(halfsecond==1)
		LEDData=0x80;
	led2=0;
	Delay5ms();
	led2=1;
	secondpoint=1;
//-----------显示时间秒------------
	LEDData=LEDCode[second/10];
	led3=0;                     //显示数码管秒钟的第一位
	Delay5ms();
	led3=1;
	LEDData=LEDCode[second%10];
	led4=0;;                    //显示数码管秒钟的第二位
	Delay5ms();
	led4=1;
}

//=========================按键检测程序================================================
void keyscan(void)
{
	if(P1!=0xf7)//矩阵按键按键识别
	{
		Delay();
		switch(P1)
		{
			case 0xb7: reset=0;P1=0xf7;break;
			case 0x77: pat=0;P1=0xf7;break;
		}
		alam=1;
	}
	if(swit==0)    //按下switch键，sw值翻转，用于选位
	{
		sw=!sw;
	}
	if(reset==0)    //按下reset键，用于重置时间、关闭蜂鸣器、关闭LED灯
	{
		TR0=0;//关闭倒计时
		minute=0x05;//时间按重置为05：00
		second=0x00;
		alam=0;
		LED5=0x00;//LED1和LED2关闭
		LED6=0x00;
		reset=r;
	}
	else
		reset=r;
	
	if(onoff==0)
	{
		z=!z;
		display();
		if(z==0)
		{
			timeover=0;//开始时，倒计时结束标志位置0，表示倒计时未结束
			playon=!playon;	//倒计时开始标志位
			LED5=~LED5;        //LED1状态翻转
			LED6=~LED6;        //LED2状态翻转
			TR0=playon;	      //开启计时
			do
				display();   //显示时间
			while(onoff==0);
		}
	}
	
	if(playon==0)  //判断倒计时是否开始，未开始可以设置时间
	{
		if(add==0)//加时间按键判断
		{
			display();
			if(add==0&&sw==0)//sw为0时，可以加分钟
			{
				if(minute<99)
					minute++;
				else
					minute=0;
			}
			if(add==0&&sw==1)//sw为1时，可以减秒钟
			{
				if(second<99)
					second++;
				else
					second=0;
			}
			do
				display();
			while(add==0);
		}
		if(sub==0)//减时间按键判断
		{
			display();
			if(sub==0&&sw==0)//sw为0时，可以减分钟
			{
				if(minute>0)
					minute--;
				else
					minute=0;
			}
			if(sub==0&&sw==1)//sw为1时，可以加秒钟
			{
				if(second>0)
					second--;
				else
					second=0;
			}
			do
				display();//显示时间
			while(sub==0);
		}
	}
	if(pat==0)//按下pat键关闭蜂鸣器和LED灯
	{
		timeover=1;
		playon=0;
		alam=1;
		TR0=0;//计时器关闭
		minute=0;//将时间置为00:00
		second=0;
		LED5=0;//将LED灯关闭
		LED6=0;
		pat=1;
		stop=0;
	}
}

//******************************主函数*************************************************
void main(void)
{
	P1=0xf7;
	LED5=0;  //初始状态将LED1关闭
	LED6=0;  //初始状态将LED2关闭
	alam=1;  //初始状态将警报器关闭
	add=1;  //初始状态将add按键置为高电平
	sub=1;  //初始状态将sub按键置为低电平
	sw=1;//sw初始状态，置1表示可以进行分钟的加减，置0表示可以进行秒钟的加减
	pat=1;//将pat键置为高电平，低电平时用于将时间重置，关闭蜂鸣器，关闭LED灯
	onoff=1;//将onoff置为高电平
	reset=r;//将reset键置为高电平
	halfsecond=1;//设置时间时秒点打开
	minute=0x05;//数码管时间初始值为05：00
	second=0x00;
	TMOD=0x01;//中断方法1,使用T0计时器
	ET0=1;
 	TL0=0xE0;
 	TH0=0xB1;
 	EA=1;
 	TR0=0;//关闭计时器
	z=1;
	while(1)//循环判断按键和显示按键
	{
		keyscan();
		display();
	}				
}

//*********************************中断服务函数**************************************
void  time0_int(void) interrupt 1
{//0.02 秒 判断1次
		TR0=1;
		count++;
		if(count==50)//1秒时LED灯状态翻
		{
			LED5=~LED5;
			LED6=~LED6;
			count=0;
			halfsecond=0;
			display();
			keyscan();
			if(second==0)
			{
				if(minute>0)//当分钟为0时判断秒钟倒计时是否减到0
				{
					second=59;
					minute--;
				}
				else  //倒计时减到00：00
				{
					while(1)
					{
						LED5=1;//LED1和LED2一直亮
						LED6=1;
						alam=0;//蜂鸣器响起
						time1();
						alam=1;//蜂鸣器响起
						time1();
						keyscan();
						if(stop==0)//用于识别pat键是否被按下，按下则结束循环
						{
							stop=1;
							break;
						}
					}
					timeover=1;//倒计时结束
					playon=0;
					TR0=0;//关闭计时器
				}
			}
			else{
				second--;
			}
		}
	halfsecond=1;
	alam=0;
	TL0=0xE0;
 	TH0=0xB1;
}