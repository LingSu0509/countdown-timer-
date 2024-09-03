#include<reg51.h>

#define LEDData P0

unsigned char code LEDCode[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char KEY[]={0x77,0xb7};

unsigned char minute,second,count,z,sw,r=1,pat,reset,stop=1;        	//�֣��룬������
//��־λz�����жϵ���ʱ�Ƿ�ʼ����־λsw���������ѡλ������ʱ�䣬pat��pat���ı�־λ��reset��reset���ı�־λ��stop����ʶ��patλ�Ƿ񱻰���
sbit secondpoint=P0^7;  //��������
sbit led1=P2^6;         //�����ѡλ1
sbit led2=P2^2;         //�����ѡλ2
sbit led3=P2^3;         //�����ѡλ3
sbit led4=P2^4;         //�����ѡλ4
sbit LED5=P2^0;         //LED1
sbit LED6=P2^1;         //LED2
sbit alam=P2^5;         //������
sbit add=P3^0;          //add���������ڼ�ʱ��
sbit sub=P3^1;          //sub���������ڼ�ʱ��
sbit onoff=P3^2;        //onoff���������ڿ�ʼ����ͣʱ��
sbit swit=P3^3;         //switch���������������ѡλ���Ӽ�ʱ��
bit playon=0;		     //����ʱ���б�־λ��Ϊ1ʱ��ʾ��ʱ����
bit timeover=0;	     //����ʱ������־λ��Ϊ1ʱ��ʾʱ���Ѿ���
bit halfsecond=0;       //���������־λ


void Delay5ms(void)     //��ʱ����Delay5ms()
{
	unsigned int i;
	for(i=50;i>0;i--);
}
void time1(void)        //��ʱ����time1()
{
	unsigned int i;
	for(i=180;i>0;i--);
}
void Delay(void)       //��ʱ����time1Delay()
{
	unsigned int i;
	for(i=1000;i>0;i--);
}
void display(void)
{
//-----------��ʾʱ���--------------
	LEDData=LEDCode[minute/10];
	led1=0;                    //��ʾ����ܷ��ӵĵ�һλ
	Delay5ms();
	led1=1;
	LEDData=LEDCode[minute%10];
	led2=0;                    //��ʾ����ܷ��ӵĵڶ�λ
	Delay5ms();
	led2=1;
//-------------���---------------
	if(halfsecond==1)
		LEDData=0x80;
	led2=0;
	Delay5ms();
	led2=1;
	secondpoint=1;
//-----------��ʾʱ����------------
	LEDData=LEDCode[second/10];
	led3=0;                     //��ʾ��������ӵĵ�һλ
	Delay5ms();
	led3=1;
	LEDData=LEDCode[second%10];
	led4=0;;                    //��ʾ��������ӵĵڶ�λ
	Delay5ms();
	led4=1;
}

//=========================����������================================================
void keyscan(void)
{
	if(P1!=0xf7)//���󰴼�����ʶ��
	{
		Delay();
		switch(P1)
		{
			case 0xb7: reset=0;P1=0xf7;break;
			case 0x77: pat=0;P1=0xf7;break;
		}
		alam=1;
	}
	if(swit==0)    //����switch����swֵ��ת������ѡλ
	{
		sw=!sw;
	}
	if(reset==0)    //����reset������������ʱ�䡢�رշ��������ر�LED��
	{
		TR0=0;//�رյ���ʱ
		minute=0x05;//ʱ�䰴����Ϊ05��00
		second=0x00;
		alam=0;
		LED5=0x00;//LED1��LED2�ر�
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
			timeover=0;//��ʼʱ������ʱ������־λ��0����ʾ����ʱδ����
			playon=!playon;	//����ʱ��ʼ��־λ
			LED5=~LED5;        //LED1״̬��ת
			LED6=~LED6;        //LED2״̬��ת
			TR0=playon;	      //������ʱ
			do
				display();   //��ʾʱ��
			while(onoff==0);
		}
	}
	
	if(playon==0)  //�жϵ���ʱ�Ƿ�ʼ��δ��ʼ��������ʱ��
	{
		if(add==0)//��ʱ�䰴���ж�
		{
			display();
			if(add==0&&sw==0)//swΪ0ʱ�����Լӷ���
			{
				if(minute<99)
					minute++;
				else
					minute=0;
			}
			if(add==0&&sw==1)//swΪ1ʱ�����Լ�����
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
		if(sub==0)//��ʱ�䰴���ж�
		{
			display();
			if(sub==0&&sw==0)//swΪ0ʱ�����Լ�����
			{
				if(minute>0)
					minute--;
				else
					minute=0;
			}
			if(sub==0&&sw==1)//swΪ1ʱ�����Լ�����
			{
				if(second>0)
					second--;
				else
					second=0;
			}
			do
				display();//��ʾʱ��
			while(sub==0);
		}
	}
	if(pat==0)//����pat���رշ�������LED��
	{
		timeover=1;
		playon=0;
		alam=1;
		TR0=0;//��ʱ���ر�
		minute=0;//��ʱ����Ϊ00:00
		second=0;
		LED5=0;//��LED�ƹر�
		LED6=0;
		pat=1;
		stop=0;
	}
}

//******************************������*************************************************
void main(void)
{
	P1=0xf7;
	LED5=0;  //��ʼ״̬��LED1�ر�
	LED6=0;  //��ʼ״̬��LED2�ر�
	alam=1;  //��ʼ״̬���������ر�
	add=1;  //��ʼ״̬��add������Ϊ�ߵ�ƽ
	sub=1;  //��ʼ״̬��sub������Ϊ�͵�ƽ
	sw=1;//sw��ʼ״̬����1��ʾ���Խ��з��ӵļӼ�����0��ʾ���Խ������ӵļӼ�
	pat=1;//��pat����Ϊ�ߵ�ƽ���͵�ƽʱ���ڽ�ʱ�����ã��رշ��������ر�LED��
	onoff=1;//��onoff��Ϊ�ߵ�ƽ
	reset=r;//��reset����Ϊ�ߵ�ƽ
	halfsecond=1;//����ʱ��ʱ����
	minute=0x05;//�����ʱ���ʼֵΪ05��00
	second=0x00;
	TMOD=0x01;//�жϷ���1,ʹ��T0��ʱ��
	ET0=1;
 	TL0=0xE0;
 	TH0=0xB1;
 	EA=1;
 	TR0=0;//�رռ�ʱ��
	z=1;
	while(1)//ѭ���жϰ�������ʾ����
	{
		keyscan();
		display();
	}				
}

//*********************************�жϷ�����**************************************
void  time0_int(void) interrupt 1
{//0.02 �� �ж�1��
		TR0=1;
		count++;
		if(count==50)//1��ʱLED��״̬��
		{
			LED5=~LED5;
			LED6=~LED6;
			count=0;
			halfsecond=0;
			display();
			keyscan();
			if(second==0)
			{
				if(minute>0)//������Ϊ0ʱ�ж����ӵ���ʱ�Ƿ����0
				{
					second=59;
					minute--;
				}
				else  //����ʱ����00��00
				{
					while(1)
					{
						LED5=1;//LED1��LED2һֱ��
						LED6=1;
						alam=0;//����������
						time1();
						alam=1;//����������
						time1();
						keyscan();
						if(stop==0)//����ʶ��pat���Ƿ񱻰��£����������ѭ��
						{
							stop=1;
							break;
						}
					}
					timeover=1;//����ʱ����
					playon=0;
					TR0=0;//�رռ�ʱ��
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