#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "24l01.h"
#include "stdlib.h"
#include "adc.h"
#include "timer.h"
#define   send_data   31
 int main(void)
 {	 
	u8 mode;u8 iRand=0;
	volatile u8 CRC_Data=0;
	float send_success=0,send_fail=0,send_sale=0;int total_send=0;
    u8 status;	int i=0;
	u8 tmp_buf[100]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};	
    u8 rec_buf[100]={0};
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();		  		//初始化与LED连接的硬件接口
 	NRF24L01_Init();    	//初始化NRF24L01 
	mode=1; //RXMODE	
	//mode=1; //TXMODE
	if(mode)
	{	
	 Adc_Init();
	 TIM3_Int_Init(2,7199);//10Khz的计数频率，计数到5000为500ms 	
	}

	while(NRF24L01_Check())
	{
		printf("NRF24L01 Error");
		delay_ms(200);
 		delay_ms(200);
	} 
	printf("NRF24L01 OK");
    delay_ms(2000);

	

 	while(1)
	{	
	 if(mode==0)//RX模式
	  { 
		NRF24L01_RX_Mode();	 printf("NRF24L01 RX_Mode \r\n");	
		while(1)
		{	 
	     	status=NRF24L01_RxPacket(rec_buf);
        	CRC_Data=0;		
			if(status==0)         //一旦接收到信息,则显示出来.
			{
				
			  printf("Received DATA:");                            
			  LED0=!LED0;
              delay_us(100);	
			  for(i=0;i<32;i++)
					printf(" %d ",rec_buf[i]);	
				
			   for(i=0;i<31;i++)
				  CRC_Data^=rec_buf[i];
			 printf("CRC:%d \r\n",CRC_Data);
		   if(CRC_Data==rec_buf[send_data])
					{ printf("rev data success \r\n");send_success++;}
			else
					{ printf("rev data fail \r\n");send_fail++;}
			
			send_sale=send_fail/(send_fail+send_success);
			total_send=send_fail+send_success;	
					{	
					 printf("Send total:%d \r\n",total_send);	
					  printf("Send fail conunt : %f\r\n",send_sale);
					}	
		    }

        			
		}
			
	 }	
	else        //TX模式
	{	
   		
       NRF24L01_TX_Mode();		 printf("NRF24L01 TX_Mode \r\n"); 
	while(1)
		{
		for(i=0;i<send_data;i++)	
		{			
		srand(time);
		iRand=rand()%100;//产生随机数0~100	
        printf(" %d ",iRand);		
		tmp_buf[i]=iRand;	
//		tmp_buf[i]=i;	
        CRC_Data =	CRC_Data^tmp_buf[i];		
		}  
	
         tmp_buf[send_data]=CRC_Data;		
		 printf("send mode\n");
	     status=NRF24L01_TxPacket(tmp_buf);
		 printf("%02x ",status);
			if(status==TX_OK)
			{
							
				printf("Sended DATA:\r\n");
				LED1=!LED1;send_success++;
				delay_ms(100);	
				printf("Send fail conunt : %f  \r\n",send_sale);
			}
			else
			{		
					   
				printf("Send Failed \r\n ");
                printf("Send fail count : %f  \r\n",send_sale);				
				LED0=!LED0;send_fail++;
				delay_ms(100);
			}
		send_sale=send_fail/(send_fail+send_success); CRC_Data=0;			    
		}
	} 
	
}   
	   


}


