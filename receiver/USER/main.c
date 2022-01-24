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
//ALIENTEKminiSTM32开发板扩展实验 
//ATK-HC05蓝牙串口模块实验-库函数版本  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
//ALIENTEK战舰STM32开发板实验13
//TFTLCD显示实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 

void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);//清屏   
 	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
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
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
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
//5个触控点的颜色												 
//电阻触摸屏测试函数
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

		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
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
			
		}else delay_ms(10);	//没有按键按下的时候 	    
		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
		{
			LCD_Clear(WHITE);//清屏
		    TP_Adjust();  //屏幕校准 
			TP_Save_Adjdata();	 
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)LED0=!LED0;
	}
} //for test, no use in the main

const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//电容触摸屏测试函数
void ctp_test(void)
{
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//最后一次的数据 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)//最多5点触摸
		{
			if((tp_dev.sta)&(1<<t))//判断是否有点触摸？
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)//在LCD范围内
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
		delay_ms(5);i++;
		if(i%20==0)LED0=!LED0;
	}	
}
	
	//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)LCD_ShowString(30,50,200,16,16,"ROLE:Master");	//主机
	else LCD_ShowString(30,50,200,16,16,"ROLE:Slave ");			 		//从机
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)LCD_ShowString(120,50,120,16,16,"STA:Connected ");			//连接成功
	else LCD_ShowString(120,50,120,16,16,"STA:Disconnect");	 			//未连接				 
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
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();				//初始化与LED连接的硬件接口
	KEY_Init();				//初始化按键
	LCD_Init();				//初始化LCD
   	tp_dev.init();			//触摸屏初始化
	POINT_COLOR=RED;//设置字体为红色 
	// LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	// LCD_ShowString(60,70,200,16,16,"TOUCH TEST");	
	// LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	// LCD_ShowString(60,110,200,16,16,"2021/12/20");

	// if(tp_dev.touchtype!=0XFF)LCD_ShowString(60,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示
	// delay_ms(1500);
	// Load_Drow_Dialog();	 	
	// if(tp_dev.touchtype&0X80)ctp_test();   	//电容屏测试
	// else rtp_test(); 						//电阻屏测试
	
	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		LCD_ShowString(30,10,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,10,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	 										   	   
	LCD_ShowString(30,10,200,16,16,"WK_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,30,200,16,16,"ATK-HC05 Standby!");  
//	sprintf((char*)buf,"%s %d\r\n",USART_RX_BUF,USART_RX_STA);
//	LCD_ShowString(30+40,240,200,16,16,buf);	//显示发送数据	
 	 LCD_ShowString(30,80,200,16,16,"Send:");	
	LCD_ShowString(30,100,200,16,16,"Receive:");	

	POINT_COLOR=BLUE;
	HC05_Role_Show();	  
 	while(1) 
	{		
	LCD_ShowString(30,80,200,16,16,"Send:");	
	LCD_ShowString(30,100,200,16,16,"Receive:");	
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//切换模块主从设置
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key=!key;  					//状态取反	   
				if(key==0)HC05_Set_Cmd("AT+ROLE=0");
				else HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//复位ATK-HC05模块
			}
		}else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//发送/停止发送  	 
			if(sendmask==0)LCD_Fill(30+40,160,240,160+16,WHITE);//清除显示
		}else delay_ms(10);	   

		if(USART2_RX_STA&0X8000)			//接收到一次数据了
		{
			LCD_Fill(30,120,240,lcddev.height*2/3,WHITE);	//清除显示
			
			LCD_Fill((u16)0,(u16)lcddev.height*2/3	,(u16)lcddev.width/2,(u16)lcddev.height,WHITE);
			LCD_Fill((u16)lcddev.width/2,(u16)lcddev.height*2/3,(u16)lcddev.width,(u16)lcddev.height,WHITE);
			
 			reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
		  	USART2_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==9||reclen==8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 ON")==0)LED1=0;	//打开LED1
				if(strcmp((const char*)USART2_RX_BUF,"+LED1 OFF")==0)LED1=1;//关闭LED1
			}
 			LCD_ShowString(30,120,209,119,16,USART2_RX_BUF);//显示接收到的数据
			 
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

		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
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
			
		}else {};	//没有按键按下的时候 	    


	}	


}
