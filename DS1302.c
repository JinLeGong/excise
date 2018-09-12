/*****************************************************
This program was produced by the
CodeWizardAVR V2.05.0 Evaluation
Automatic Program Generator
© Copyright 1998-2010 Pavel Haiduc, HP InfoTech s.r.l.
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



#define WRITE_SECOND 0x80//Ð´Ãë
#define READ_SECOND 0x81//¶ÁÃë
#define WRITE_MINUTE 0x82//Ð´·ÖÖÓ
#define READ_MINUTE 0x83//¶Á·ÖÖÓ
#define WRITE_HOUR 0x84//Ð´Ð¡Ê±
#define READ_HOUR 0x85//¶ÁÐ¡ÖÓ
#define WRITE_DAY 0x86//Ð´ÈÕ
#define READ_DAY 0x87//¶ÁÈÕ
#define WRITE_MONTH 0x88//Ð´ÔÂ
#define READ_MONTH 0x89//¶ÁÔÂ
#define WRITE_PROTECT 0x8E//Ð´±£»¤

#define SCLK PORTB.1  //Ê±ÖÓ
#define DIO   PORTB.2  //Êý¾ÝÊäÈë
#define RST  PORTB.3  //¸´Î»/Æ¬Ñ¡Ïß



unsigned char show_dat[2],time[5];//Ê±¼äµÄ´æ·ÅÎªÔÂ£¬ÈÕ£¬Ê±£¬·Ö£¬Ãë

flash unsigned char led_7[10]={0xD7,0x14,0xCD,0x5D,0x1E,0x5B,0xDB,0x15,0xDF,0x5F}; /*ÊýÂë¹ÜÏÔÊ¾µÄÄÚÈÝ£¬0~9  */
flash unsigned char position[4]={0x01,0x02,0x04,0x08};
bit time_1s;
bit m_start;
unsigned char CountNumber_s;
unsigned char CountNumber_m;
unsigned char posit;
unsigned char dis_buffer[4];  //Ö¸¶¨4Î»ÊýÂë¹ÜÖÐÒªÏÔÊ¾µÄÄÚÈÝ
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

void DS1302writeByte(unsigned char data) //Ïò DS1302 Ð´ÈëÒ»¸ö×Ö½Ú 
{
    unsigned char i;
    //ACC = _data;
    for(i=8; i>0; i--)
    {
        if(data&0x01)
            DIO = 1;//Ð´Ò»Î»Êý¾Ý
        else
            DIO=0;
        SCLK = 1; //Ê±ÖÓÐÅºÅ
        delay1(10);
        SCLK = 0;
        delay1(10);
        data=data>>1;//ÒÆÎ»£¬×¼±¸ºÃÏÂ´ÎÒªÐ´µÄÊý¾Ý¡£
    }
}


unsigned char DS1302OreadByte(void)//Ïò DS1302 ¶ÁÈ¡Ò»¸ö×Ö½Ú
{
    unsigned char i;
    unsigned char data=0;
    DDRB.2=0;
    for(i=8; i>0; i--)
    {
        data = data >>1;//ÒÆÎ»£¬ÒÔ±ãÏÂ´Î´æ·Å¶Á³öµÄÊý¾Ý
        if(PINB.2==1)
            data|=0x80; //ACC7 = DIO;//¶Á 1 Î»Êý¾Ý
        SCLK = 1;//Ê±ÖÓÐÅºÅ
        delay1(10);
        SCLK = 0;
        delay1(10);
    }
    DDRB.2=1;
    return(data);//·µ»Ø¶Áµ½µÄÊý¾Ý
}


void Write1302(unsigned char ucAddr, unsigned char ucDa)//Ð´ DS1302 ¼Ä´æÆ÷
{
    RST = 0;
    SCLK=0;
    RST=1;
    DS1302writeByte(ucAddr); // µØÖ·£¬ÃüÁî
    DS1302writeByte(ucDa); // Ð´ 1Byte Êý¾Ý
    SCLK = 1;
    RST = 0;
}

unsigned char Read1302(unsigned char ucAddr)  //¶Á DS1302 µÄ¼Ä´æÆ÷ÄÚÈÝ
{
    unsigned char ucData;
    RST = 0;
    SCLK = 0;
    RST = 1;
    DS1302writeByte(ucAddr|0x01); // µØÖ·£¬ÃüÁî
    ucData = DS1302OreadByte(); // ¶Á 1Byte Êý¾Ý
    SCLK = 1;
    RST = 0;
    return(ucData);
}


void Initial(void) //³õÊ¼»¯ DS1302
{

    Write1302 (0x90,0x00);  
/*
µØÖ·0x90Îª³äµç¼Ä´æÆ÷£¬¿ÉÒÔ¶Ô³äµçµçÁ÷½øÐÐÏÞÖÆ£¬Ð´Èë
ÄÚÈÝ¸ß4Î»¹Ì¶¨Îª1010£¨ÆäËû×éºÏ¾ù²»ÄÜ³äµç£©£¬µÍ4
Î»µÄÊ×2Î»ÊÇÑ¡ÔñÄÚ²¿½µÑ¹¶þ¼«¹ÜµÄ¸öÊýµÄ£¬01´ú±íÔÚ
³äµç»ØÂ·´®Èë1¸ö¶þ¼«¹Ü£¬10´ú±í´®Èë2¸ö£»×îºó2Î»¿É
Éè¶¨´®ÈëµÄµç×èµÄÊýÖµ£º01Îª2kÅ·£¬10Îª4kÅ·£¬11Îª8kÅ·¡£
²»Ê¹ÓÃä¸Á÷³äµç¹¦ÄÜ£¬Çë½«¼Ä´æÆ÷£¨µØÖ·Îª90h£©ÉèÖÃ³É0x00¡£
*/ 
//³õÊ¼»¯Îª 4 ÔÂ£¬13 ÈÕ£¬9 Ê±£¬24 ·Ö£¬45 Ãë
    Write1302 (WRITE_PROTECT,0x00); //½ûÖ¹Ð´±£»¤
    Write1302 (WRITE_SECOND,0x45); //ÃëÎ»³õÊ¼»¯
    Write1302 (WRITE_MINUTE,0x24); //·ÖÖÓ³õÊ¼»¯
    Write1302 (WRITE_HOUR,0x09); //Ð¡Ê±³õÊ¼»¯
    Write1302 (WRITE_DAY,0x13); //ÈÕ³õÊ¼»¯
    Write1302 (WRITE_MONTH,0x04);  //ÔÂ³õÊ¼»¯
    Write1302 (WRITE_PROTECT,0x80); //ÔÊÐíÐ´±£»¤ 
}

void get_time(unsigned char *time)
{ 
    *(time+4)=Read1302(READ_SECOND);//¶ÁÈ¡ÃëÖÓ
    *(time+3)=Read1302(READ_MINUTE);//¶ÁÈ¡·ÖÖÓ
    *(time+2)=Read1302(READ_HOUR); //¶ÁÈ¡Ð¡Ê±
    *(time+1)=Read1302(READ_DAY); //¶ÁÈ¡ÈÕ
    *(time+0)=Read1302(READ_MONTH); //¶ÁÈ¡ÔÂ
}


void adopt_data(unsigned char n,unsigned char *show_data)//×ª»»Îª LCD ÏÔÊ¾µÄÊý¾Ý
{
    *show_data=n>>4;//Ê®Î»
    *(show_data+1)=n&0x0f;//¸öÎ»
}



void main(void)
{
    PORTC=0x00;
    DDRC=0xFF;
    
    PORTA=0x00;
    DDRA=0x0F;
    
    TCCR0=0x0B;          //  ÄÚ²¿Ê±ÖÓ£¬64·ÖÆµ£¨8MHz/64=125KHz£©£¬CTCÄ£Ê½
    TCNT0=0x00;
    OCR0=0x7C;           //£¨124+1£©/125K=1ms
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
        get_time(time); //»ñÈ¡ DS1302 µÄÊ±¼ä


        adopt_data(time[4],show_dat);//×ª»»Îª ÊýÂë¹ÜÏÔÊ¾µÄÊý¾Ý
        CountNumber_s=show_dat[1]+show_dat[0]*10;
        adopt_data(time[3],show_dat);
        CountNumber_m=show_dat[1]+show_dat[0]*10;
        time_to_disbuffer();
        
}
}