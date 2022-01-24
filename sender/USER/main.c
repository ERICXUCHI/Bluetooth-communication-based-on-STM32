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
 	
char ques[10];
char ans[10];
char cclk[3];
char cpoint[2];
char ctotal[3];
int clk=0;
int isright=0;
int mode = 1; //1 question; 2 answer; 3 judging
u8 timestr[10];
	u8 sendcnt=0;
int sed=0;
int total = 0;
	int flag=0;
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
int main(void)
 {	 
	int t=0;
	int j=0;
	int k;
	int clk=0;
	u8 key;
	u8 sendmask=0;

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
	tp_dev.init();									//??????
   
	POINT_COLOR=RED;
	//LCD_ShowString(30,30,200,16,16,"ALIENTEK STM32 ^_^");	
	//LCD_ShowString(30,50,200,16,16,"HC05 BLUETOOTH COM TEST");	
	//LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
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
		}
		else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//����/ֹͣ����  	 
			if(sendmask==0)
				LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
		}
		else 
			delay_ms(1);	   
		if(t==1380)
		{
			
//			if(sendmask)					//��ʱ����
//			{
////				printf("%s",USART_RX_BUF);
//				LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
//				clear_buffer(sendbuf, 20);
//				sprintf((char*)sendbuf,"%s\r\n",USART_RX_BUF);
//	  			LCD_ShowString(30+40,160,200,16,16,sendbuf);	//��ʾ��������	
//			
//				u2_printf("%s\r\n",sendbuf);		//���͵�����ģ��
//				
//				//clear_buffer(USART_RX_BUF,USART_REC_LEN);
//				
				if(sed==1)
				{
					clk = (cclk[0]-'0')*10+cclk[1]-'0';
					sprintf((char*)timestr,"%d\r\n",sendcnt);
					POINT_COLOR=RED;
					LCD_Fill(30,120,240,120+16,WHITE);
					//LCD_ShowString(30+40,120,200,16,16,timestr);	//��ʾ��������	
					POINT_COLOR=BLUE;
					sendcnt++;
					if(sendcnt>clk&&flag==0&&isright==0)
					{
						printf("timeout");
						LCD_ShowString(30,200,200,16,16,"No right answer");	
						u2_printf("!Time Exceed Limit\r\n");		//���͵�����ģ��
						flag=1;
						mode = 3;
					}
				}
//			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART2_RX_STA&0X8000)			//���յ�һ��������
		{
		
			printf("%s\r\n", USART2_RX_BUF);
			
			LCD_Fill(30,200,240,320,WHITE);	//�����ʾ
 			reclen=USART2_RX_STA&0X7FFF;	//�õ����ݳ���
		  	USART2_RX_BUF[reclen]=0;	 	//���������
			if(reclen==9||reclen==8) 		//����DS1���
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//�ر�LED1
			}
			
 			LCD_ShowString(30,200,209,119,16,USART2_RX_BUF);//��ʾ���յ�������
			
			for(j=0;j<strlen(ans);j++)
			{
				if(ans[j]=='\r'||ans[j]==' '||ans[j]=='\n')
				{
					ans[j]='\0';
//					for(k=j;k<strlen(ans);k++)
//					{
//						ans[k]=ans[k+1];
//					}
					
				}
			}
			for(j=0;j<strlen((char*)USART2_RX_BUF);j++)
			{
				if(USART2_RX_BUF[j]=='\r'||USART2_RX_BUF[j]==' '||USART2_RX_BUF[j]=='\n')
				{
					USART2_RX_BUF[j]='\0';
//					for(k=j;k<strlen((char*)USART2_RX_BUF);k++)
//					{
//						USART2_RX_BUF[k]=USART2_RX_BUF[k+1];
//					}
					
				}
			}
			
			if(mode == 2){
				
				if(flag==1)
				{
						u2_printf("!TLE!\r\n");
						mode = 3;
				}else
				
				if(strcmp((char*)USART2_RX_BUF,ans)==0)
				{
					isright = 1;
					printf("RIGHT\n");
					total += cpoint[0] - '0';
					//u2_printf("%s--RIGHT\r\n",ques);		//���͵�����ģ��
					u2_printf("V,Right! + #%s\r\n",cpoint);
					mode = 3;
				}
				else if(strcmp((char*)USART2_RX_BUF,"next")==0)
				{
					printf("EnterNextQuestion\r\n");
				}
				else if(strcmp((char*)USART2_RX_BUF,"end")==0)
				{
					u2_printf("Over! Total: %d\r\n",total);
					mode = 4;
				}
				else
				{
					printf("WRONG\n");
					u2_printf("X,%s is WRONG + 0\r\n",ques);		//���͵�����ģ��
					printf("���⣺%s\n",ans);
					printf("*%s\r\n",USART2_RX_BUF);
				}
				USART2_RX_STA=0;	 
			}	 						
		}			
		t++;	
		

		tp_dev.scan(0);
		
		if(mode == 3){
		LCD_ShowString(lcddev.width / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "NEXT");
		LCD_ShowString(lcddev.width * 3 / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "END");
			LCD_ShowString(30,120,200,16,16, "Total Score: ");
		LCD_ShowString(130,120,200,16,16, (u8 *)ctotal);
		ctotal[0] = total/10+'0';
		ctotal[1] = total%10+'0';
		ctotal[2] = '\0';
		}
		
		if (tp_dev.sta & TP_PRES_DOWN) //??????
		{
			u16 zero = 0;			
			if (tp_dev.x[0] < lcddev.width / 2 && tp_dev.y[0] < lcddev.height && tp_dev.y[0] >= lcddev.height * 2 / 3 && mode ==3)
			{
				// LCD_Clear(WHITE);
				//LCD_Fill(zero, (u16)lcddev.height * 2 / 3, (u16)lcddev.width / 2, (u16)lcddev.height, BLUE);
				LCD_Fill((u16)lcddev.width / 2, (u16)lcddev.height * 2 / 3, (u16)lcddev.width, (u16)lcddev.height, WHITE);
				LCD_Fill(zero, (u16)lcddev.height * 2 / 3, (u16)lcddev.width / 2, (u16)lcddev.height, WHITE);
				//LCD_ShowString(lcddev.width / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "NEXT");
				//LCD_ShowString(lcddev.width * 3 / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "END");
				LCD_Fill(30,200,160,240,WHITE);
				LCD_ShowString(lcddev.width / 4 - 10, lcddev.height * 5 / 6, 200, 24, 20, "Enter Question");
				mode = 1;
				//u2_printf("next\r\n");
			}

			if (tp_dev.x[0] >= lcddev.width / 2 && tp_dev.x[0] < lcddev.width && tp_dev.y[0] < lcddev.height && tp_dev.y[0] >= lcddev.height * 2 / 3 && mode ==3)
			{

				//LCD_Fill((u16)lcddev.width / 2, (u16)lcddev.height * 2 / 3, (u16)lcddev.width, (u16)lcddev.height, GREEN);
				//LCD_Fill((u16)lcddev.width / 2, (u16)lcddev.height * 2 / 3, (u16)lcddev.width, (u16)lcddev.height, WHITE);//
				//LCD_Fill(zero, (u16)lcddev.height * 2 / 3, (u16)lcddev.width / 2, (u16)lcddev.height, WHITE);
				//LCD_ShowString(lcddev.width / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "NEXT");
				//LCD_ShowString(lcddev.width * 3 / 4 - 10, lcddev.height * 5 / 6, 200, 24, 24, "END");
				//u2_printf("end\r\n");
				//LCD_Fill(30,200,160,240,WHITE);
				LCD_Clear(WHITE);
				ctotal[0]=total/10+'0';
				ctotal[1]=total%10+'0';
				ctotal[2]='\0';
				LED0=0;
				printf("%s",USART_RX_BUF);
				u2_printf("END! Your SCORE: %s\r\n",ctotal);
				mode =4;
				LCD_ShowString(30,120,200,16,16, "Total Score: ");
				LCD_ShowString(130,120,200,16,16, (u8 *)ctotal);
			}
		}
		else
		{
		}; //?????????
		
		if(mode == 4){
			LCD_ShowString(30,100,200,16,16, "END");
			LCD_ShowString(30,120,200,16,16, "Total Score: ");
			LCD_ShowString(130,120,200,16,16, (u8 *)ctotal);
		}
		
		
	}											    
}
