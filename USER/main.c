#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "string.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "24l01.h"

//ALIENTEKս��STM32������ʵ��49
//����USB��� ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾  		 	


//����x1,x2�ľ���ֵ
u32 usb_abs(u32 x1, u32 x2)
{
	if (x1>x2)return x1 - x2;
	else return x2 - x1;
}
//����USB ����/����
//enable:0,�Ͽ�
//       1,��������	   
void usb_port_set(u8 enable)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	if (enable)_SetCNTR(_GetCNTR()&(~(1 << 1)));//�˳��ϵ�ģʽ
	else
	{
		_SetCNTR(_GetCNTR() | (1 << 1));  // �ϵ�ģʽ
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12) = 0;
	}
}

int main(void)
{
	u8 tmp_buf[5];	 //����Ĵ���
	short X, Y;		 //���͵����Զ˵�����ֵ

	//[1]:0,�Ҽ��ɿ�;1,�Ҽ�����
	//[2]:0,�м��ɿ�;1,�м�����   		  
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	printf("���ڳ�ʼ�����\r\n");
	delay_ms(100);
	NRF24L01_Init();
	printf("24l01��ʼ�����\r\n");
	while (NRF24L01_Check())	//���NRF24L01�Ƿ���λ.
	{
		printf("δ��⵽24l01\r\n");
		delay_ms(200);
	}
	printf("��⵽24l01---OK\r\n");
	NRF24L01_RX_Mode();
	printf("����24l01Ϊ����ģʽ�ɹ���\r\n");

	usb_port_set(0); 	//USB�ȶϿ�
	delay_ms(300);
	usb_port_set(1);	//USB�ٴ�����
	//USB����
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	printf("USB����ʼ����ϣ�����\r\n");
	while (1)
	{
		if (NRF24L01_RxPacket(tmp_buf) == 0)
		{
			X = (s16)((tmp_buf[1] << 8) | tmp_buf[2]);
			Y = (s16)((tmp_buf[3] << 8) | tmp_buf[4]);
			printf(" %8d%8d\r\n", X, Y);
			Joystick_Send(tmp_buf[0], X, Y, 0);
		}
	}
}

