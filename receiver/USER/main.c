#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"	 
#include "key.h"
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
//ALIENTEKminiSTM32��������չʵ�� 
//ATK-HC05��������ģ��ʵ��-�⺯���汾  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
//ALIENTEKս��STM32������ʵ��13
//TFTLCD��ʾʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//����   
 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;//���û�����ɫ 
}
////////////////////////////////////////////////////////////////////////////////
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
////////////////////////////////////////////////////////////////////////////////
//5�����ص����ɫ												 
//���败�������Ժ���
void rtp_test(void)
{
	u8 key;
	u8 i=0;	  
	while(1)
	{
	 	key=KEY_Scan(0);
		tp_dev.scan(0); 	
	
		LCD_ShowString(lcddev.width/4,lcddev.height*5/6,200,24,24,"NEXT"); 
		LCD_ShowString(lcddev.width*3/4,lcddev.height*5/6,200,24,24,"END"); 

		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
			u16 zero=0;

		 	if(tp_dev.x[0]<lcddev.width/2&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>=lcddev.height*2/3)
			{	
				// LCD_Clear(WHITE);
				LCD_Fill(zero,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,BLUE);
				LCD_ShowString(lcddev.width/4,lcddev.height*5/6,200,24,24,"NEXT"); 	
				u2_printf("next");
			}

		 	if(tp_dev.x[0]>=lcddev.width/2&&tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>=lcddev.height*2/3)
			{	
				// LCD_Clear(WHITE);
				LCD_Fill(zero,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,GREEN);
				LCD_ShowString(lcddev.width*3/4,lcddev.height*5/6,200,24,24,"END");	
				u2_printf("end");
			}
			
		}else delay_ms(10);	//û�а������µ�ʱ�� 	    
		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);//����
		    TP_Adjust();  //��ĻУ׼ 
			TP_Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)LED0=!LED0;
	}
} //for test, no use in the main

const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//���ݴ��������Ժ���
void ctp_test(void)
{
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//���һ�ε����� 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)//���5�㴥��
		{
			if((tp_dev.sta)&(1<<t))//�ж��Ƿ��е㴥����
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)//��LCD��Χ��
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//����
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//���
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
		delay_ms(5);i++;
		if(i%20==0)LED0=!LED0;
	}	
}
	
	//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,50,200,16,16,"ROLE:Master");	//����
	else LCD_ShowString(30,50,200,16,16,"ROLE:Slave ");			 		//�ӻ�
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,50,120,16,16,"STA:Connected ");			//���ӳɹ�
	else LCD_ShowString(120,50,120,16,16,"STA:Disconnect");	 			//δ����				 
}	  

void clear_buffer(u8* buffer, int length) {
	//strcpy((char*)buffer,"\0");
	int i;
	for (i = 0; i < length; i++) {
		buffer[i] = '\0';
	}
}

int split_clk(u8* buffer) {
	int a = buffer[2] - '0';
	int b = buffer[3] - '0';
	return a*10 + b;
}

int main(void)
 {	 
	int t=0;
	int countDown = 100;
	u8 countDownBuffer[20];
	int Begin = 0;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 buf[100];
	u8 send;
	u8 reclen=0;  
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	LED_Init();				//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();				//��ʼ������
	LCD_Init();				//��ʼ��LCD
   	tp_dev.init();			//��������ʼ��
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	// LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	// LCD_ShowString(60,70,200,16,16,"TOUCH TEST");	
	// LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	// LCD_ShowString(60,110,200,16,16,"2021/12/20");

	// if(tp_dev.touchtype!=0XFF)LCD_ShowString(60,130,200,16,16,"Press KEY0 to Adjust");//����������ʾ
	// delay_ms(1500);
	// Load_Drow_Dialog();	 	
	// if(tp_dev.touchtype&0X80)ctp_test();   	//����������
	// else rtp_test(); 						//����������
	
	while(HC05_Init()) 		//��ʼ��ATK-HC05ģ��  
	{
		LCD_ShowString(30,10,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,10,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	LCD_ShowString(30,10,200,16,16,"WK_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,30,200,16,16,"ATK-HC05 Standby!");  
//	sprintf((char*)buf,"%s %d\r\n",USART_RX_BUF,USART_RX_STA);
//	LCD_ShowString(30+40,240,200,16,16,buf);	//��ʾ��������	
 	 LCD_ShowString(30,80,200,16,16,"Send:");	
	LCD_ShowString(30,100,200,16,16,"Receive:");	

	POINT_COLOR=BLUE;
	HC05_Role_Show();	  
 	while(1) 
	{		
	LCD_ShowString(30,80,200,16,16,"Send:");	
	LCD_ShowString(30,100,200,16,16,"Receive:");	
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//�л�ģ����������
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//״̬ȡ��	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//��λATK-HC05ģ��
			}
		}else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//����/ֹͣ����  	 
			if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
		}else delay_ms(10);	   

		if(USART2_RX_STA&0X8000)			//���յ�һ��������
		{
			LCD_Fill(30,120,240,lcddev.height*2/3,WHITE);	//�����ʾ
			
			LCD_Fill((u16)0,(u16)lcddev.height*2/3	,(u16)lcddev.width/2,(u16)lcddev.height,WHITE);
			LCD_Fill((u16)lcddev.width/2,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,WHITE);
			
 			reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART2_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
 			LCD_ShowString(30,120,209,119,16,USART2_RX_BUF);//��ʾ���յ�������
			 
			if (USART2_RX_BUF[0] == 'T'){
				printf("%s\r\n", USART2_RX_BUF);
				countDown = split_clk(USART2_RX_BUF);
				Begin = 1;
			}
 			USART2_RX_STA=0;	 
		}
		
	

		if(t==50 && Begin == 1 && USART2_RX_BUF[0] != 'V' && USART2_RX_BUF[0] != 'O' && USART2_RX_BUF[0] != 'E' && USART2_RX_BUF[0] != '!')
			{
				HC05_Sta_Show();  	  
			  LED0=!LED0; 

				LCD_Fill(30,180,240,180+16,WHITE);
				countDown--;
				//printf("left: %d\r\n", countDown);
				//printf("t: %d\r\n", t);
				t=0;		
				clear_buffer(countDownBuffer, 20);			
				sprintf((char*)countDownBuffer,"LeftTime: %d\r\n",countDown);
				if (countDown < 0 ){
					//LCD_ShowString(30,180,200,16,16,"Time OUT!");
				}
				else{
					LCD_ShowString(30,180,200,16,16,countDownBuffer);
				}
			
			}	 
			
		if (t == 50){
			
			HC05_Sta_Show();  	  
			LED0=!LED0; 
			t = 0;			
		}

		t++;	

		tp_dev.scan(0); 	
	
		LCD_ShowString(lcddev.width/4-10,lcddev.height*5/6,200,24,24,"NEXT"); 
		LCD_ShowString(lcddev.width*3/4-10,lcddev.height*5/6,200,24,24,"END"); 

		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{	
			u16 zero=0;

		 	if(tp_dev.x[0]<lcddev.width/2&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>=lcddev.height*2/3)
			{	
				// LCD_Clear(WHITE);
				LCD_Fill(zero,(u16)lcddev.height*2/3	,(u16)lcddev.width/2,(u16)lcddev.height,BLUE);
				LCD_Fill((u16)lcddev.width/2,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,WHITE);
				LCD_ShowString(lcddev.width/4-10,lcddev.height*5/6,200,24,24,"NEXT"); 	
				LCD_ShowString(lcddev.width*3/4-10,lcddev.height*5/6,200,24,24,"END"); 
				u2_printf("next\r\n");
			}

		 	if(tp_dev.x[0]>=lcddev.width/2&&tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height&&tp_dev.y[0]>=lcddev.height*2/3)
			{	
				
				LCD_Fill((u16)lcddev.width/2,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,GREEN);
				LCD_Fill(zero,(u16)lcddev.height*2/3,(u16)lcddev.width/2,(u16)lcddev.height,WHITE);
				LCD_ShowString(lcddev.width/4-10,lcddev.height*5/6,200,24,24,"NEXT"); 	
				LCD_ShowString(lcddev.width*3/4-10,lcddev.height*5/6,200,24,24,"END"); 
				u2_printf("end\r\n");
			}
			
		}else {};	//û�а������µ�ʱ�� 	    


	}	


}
