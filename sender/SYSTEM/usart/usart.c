#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "string.h"
#include "usart2.h"
#include "delay.h"
#include "lcd.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
extern char ques[10];
extern char ans[10];
extern char cpoint[10];
extern char cclk[3];
extern int total;
extern int sed;
extern int clk;
extern int flag;
extern int sendcnt;
extern int isright;
extern int total;
extern char ctotal[3];
extern int mode;
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void clear_buffer2(u8* buffer, int length) {
	//strcpy((char*)buffer,"\0");
	int i;
	for (i = 0; i < length; i++) {
		buffer[i] = '\0';
	}
}
void split(char*origin,char* cclk,char* cpoint,char* ques,char* ans)
{
	int i=0;
	int tmp2=0;
	int tmp3=0;
	int tmp=0;
	for(i=0;origin[i]!='|';i++)
	{
		cclk[i]=origin[i];
	}
	cclk[i+1]='\0';
	i=i+1;
	tmp2=i;
	for(;origin[i]!='|';i++)
	{
		cpoint[i-tmp2]=origin[i];
	}
	
	cpoint[i-tmp2+1]='\0';
	i=i+1;
	tmp3=i;

	for(;origin[i]!='|';i++)
	{
		ques[i-tmp3]=origin[i];
	}
	ques[i-tmp3+1]='\0';
	i=i+1;
	tmp=i;
	for(;i!=strlen(origin);i++)
	{
		ans[i-tmp]=origin[i];
	}
	ans[i-tmp+1]='\0';
}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
		u8 sendbuf2[20];
			u8 Res;
		int i=0;
//		clear_buffer(USART_RX_BUF,USART_REC_LEN);

#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
					else 
					{
						for(i=USART_RX_STA&0X3FFF;i<200;i++)
						{
							USART_RX_BUF[i]='\0';
						}
						
						if(mode == 1 || mode ==2){
						USART_RX_STA|=0x8000;	//��������� 
						LED1=!LED1;
						USART_RX_STA = 0;
						
						clear_buffer2((u8*) cclk, 3);
						clear_buffer2((u8*) cpoint, 10);
						clear_buffer2((u8*) ques, 10);
						clear_buffer2((u8*) ans, 10);
						
						//if(USART_RX_BUF[0]=='e'&&USART_RX_BUF[1]=='n'&&USART_RX_BUF[2]=='d')
						if(0)
						{
							ctotal[0]=total/10+'0';
							ctotal[1]=total%10+'0';
							ctotal[2]='\0';
							LED0=0;
							printf("%s",USART_RX_BUF);
							u2_printf("END! Your SCORE: %s\r\n",ctotal);
						}
						else
						{
							mode = 2;
							split((char*)USART_RX_BUF,cclk,cpoint,ques,ans);
							
							LCD_Fill(30+40,160,240,160+16,WHITE);//�����ʾ
							clear_buffer2(sendbuf2, 20);
							sprintf((char*)sendbuf2,"%s\r\n",ques);
							printf("ʱ�䣺%s,", cclk);
							printf("����: %s,", cpoint);
							printf("����: %s,", ques);
							printf("�𰸣�%s",ans);
							
							printf("�ܷ�: %d", total);
							
							//POINT_COLOR=RED;
							//LCD_Fill(30,220,240,250,YELLOW);
							//LCD_ShowString(30,220,200,16,16,"����ʱ: ");
							POINT_COLOR=BLUE;
							LCD_ShowString(30+40,160,200,16,16,sendbuf2);	//��ʾ��������	
							
							
								u2_printf("T:%s,S:%d,P:%s,Q:%s\r\n", cclk,total,cpoint,sendbuf2);		//���͵�����ģ��
							sed=1;
							flag=0;
							sendcnt=0;
							isright=0;
					//clear_buffer(USART_RX_BUF,USART_REC_LEN);
						}
					}
				}
			}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

