/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Evaluation
Automatic Program Generator
� Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 2018/1/8
Author  : Freeware, for evaluation and non-commercial use only
Company : 
Comments: 


Chip type               : ATmega16L
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega16.h>

// Declare your global variables here



#define WRITE_SECOND 0x80//д��
#define READ_SECOND 0x81//����
#define WRITE_MINUTE 0x82//д����
#define READ_MINUTE 0x83//������
#define WRITE_HOUR 0x84//дСʱ
#define READ_HOUR 0x85//��С��
#define WRITE_DAY 0x86//д��
#define READ_DAY 0x87//����
#define WRITE_MONTH 0x88//д��
#define READ_MONTH 0x89//����
#define WRITE_PROTECT 0x8E//д����

#define SCLK PORTB.1  //ʱ��
#define DIO   PORTB.2  //��������
#define RST  PORTB.3  //��λ/Ƭѡ��



unsigned char show_dat[2],time[5];//ʱ��Ĵ��Ϊ�£��գ�ʱ���֣���

flash unsigned char led_7[10]={0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F}; /*�������ʾ�����ݣ�0~9  */
flash unsigned char position[4]={0x01,0x02,0x04,0x08};
bit time_1s;
bit m_start;
unsigned char CountNumber_s;
unsigned char CountNumber_m;
unsigned char posit;
unsigned char dis_buffer[4];  //ָ��4λ�������Ҫ��ʾ������
int time_counter;


void display(void)
{
    
    
    PORTC=led_7[dis_buffer[posit]];
    PORTA=position[posit];
    if(++posit>=4)
    {
        posit=0;
    }
   
}



void time_to_disbuffer(void)
{
   
        dis_buffer[0]=CountNumber_s%10;
        dis_buffer[1]=CountNumber_s/10;
        
        dis_buffer[2]=CountNumber_m%10;
        dis_buffer[3]=CountNumber_m/10;
        
        
        
}


interrupt [TIM0_COMP] void timer0_comp_isr(void)
{
    display();
    if(++time_counter>=500)
    {
        time_counter=0;
        time_1s=1;
    }
        
}



void delay1(unsigned int time)
{
	unsigned int i,j;
	for(i = 0;i < time; i++)
	{
	   for(j = 0;j < 1; j++);
    }
}

void DS1302writeByte(unsigned char data) //�� DS1302 д��һ���ֽ� 
{
    unsigned char i;
    //ACC = _data;
    for(i=8; i>0; i--)
    {
        if(data&0x01)
            DIO = 1;//дһλ����
        else
            DIO=0;
        SCLK = 1; //ʱ���ź�
        delay1(10);
        SCLK = 0;
        delay1(10);
        data=data>>1;//��λ��׼�����´�Ҫд�����ݡ�
    }
}


unsigned char DS1302OreadByte(void)//�� DS1302 ��ȡһ���ֽ�
{
    unsigned char i;
    unsigned char data=0;
    DDRB.2=0;
    for(i=8; i>0; i--)
    {
        data = data >>1;//��λ���Ա��´δ�Ŷ���������
        if(PINB.2==1)
            data|=0x80; //ACC7 = DIO;//�� 1 λ����
        SCLK = 1;//ʱ���ź�
        delay1(10);
        SCLK = 0;
        delay1(10);
    }
    DDRB.2=1;
    return(data);//���ض���������
}


void Write1302(unsigned char ucAddr, unsigned char ucDa)//д DS1302 �Ĵ���
{
    RST = 0;
    SCLK=0;
    RST=1;
    DS1302writeByte(ucAddr); // ��ַ������
    DS1302writeByte(ucDa); // д 1Byte ����
    SCLK = 1;
    RST = 0;
}

unsigned char Read1302(unsigned char ucAddr)  //�� DS1302 �ļĴ�������
{
    unsigned char ucData;
    RST = 0;
    SCLK = 0;
    RST = 1;
    DS1302writeByte(ucAddr|0x01); // ��ַ������
    ucData = DS1302OreadByte(); // �� 1Byte ����
    SCLK = 1;
    RST = 0;
    return(ucData);
}


void Initial(void) //��ʼ�� DS1302
{

    Write1302 (0x90,0x00);  
/*
��ַ0x90Ϊ���Ĵ��������ԶԳ������������ƣ�д��
���ݸ�4λ�̶�Ϊ1010��������Ͼ����ܳ�磩����4
λ����2λ��ѡ���ڲ���ѹ�����ܵĸ����ģ�01������
����·����1�������ܣ�10������2�������2λ��
�趨����ĵ������ֵ��01Ϊ2kŷ��10Ϊ4kŷ��11Ϊ8kŷ��
��ʹ�������繦�ܣ��뽫�Ĵ�������ַΪ90h�����ó�0x00��
*/ 
//��ʼ��Ϊ 4 �£�13 �գ�9 ʱ��24 �֣�45 ��
    Write1302 (WRITE_PROTECT,0x00); //��ֹд����
    Write1302 (WRITE_SECOND,0x45); //��λ��ʼ��
    Write1302 (WRITE_MINUTE,0x24); //���ӳ�ʼ��
    Write1302 (WRITE_HOUR,0x09); //Сʱ��ʼ��
    Write1302 (WRITE_DAY,0x13); //�ճ�ʼ��
    Write1302 (WRITE_MONTH,0x04);  //�³�ʼ��
    Write1302 (WRITE_PROTECT,0x80); //����д���� 
}

void get_time(unsigned char *time)
{ 
    *(time+4)=Read1302(READ_SECOND);//��ȡ����
    *(time+3)=Read1302(READ_MINUTE);//��ȡ����
    *(time+2)=Read1302(READ_HOUR); //��ȡСʱ
    *(time+1)=Read1302(READ_DAY); //��ȡ��
    *(time+0)=Read1302(READ_MONTH); //��ȡ��
}


void adopt_data(unsigned char n,unsigned char *show_data)//ת��Ϊ LCD ��ʾ������
{
    *show_data=n>>4;//ʮλ
    *(show_data+1)=n&0x0f;//��λ
}



void main(void)
{
    PORTC=0x00;
    DDRC=0xFF;
    
    PORTA=0x00;
    DDRA=0x0F;
    
    TCCR0=0x0B;          //  �ڲ�ʱ�ӣ�64��Ƶ��8MHz/64=125KHz����CTCģʽ
    TCNT0=0x00;
    OCR0=0x7C;           //��124+1��/125K=1ms
    TIMSK=0x02; 
    
    CountNumber_s=0;
    posit=0;
    m_start=0;
    
   // time_to_disbuffer();
    
    DDRB.2=1;
    DDRB.1=1;
    DDRB.3=1;
    
    #asm("sei")


Initial();
while(1)
{
        get_time(time); //��ȡ DS1302 ��ʱ��


        adopt_data(time[4],show_dat);//ת��Ϊ �������ʾ������
        CountNumber_s=show_dat[1]+show_dat[0]*10;
        adopt_data(time[3],show_dat);
        CountNumber_m=show_dat[1]+show_dat[0]*10;
        time_to_disbuffer();
        
}
}