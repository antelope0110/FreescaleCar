/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2016,��ɿƼ�
* All rights reserved.
* ��������QQȺ��179029047
*
* �����������ݰ�Ȩ������ɿƼ����У�δ��������������ҵ��;��
* ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
*
* @file       		isr.c
* @brief      		�жϺ�����
* @company	   		�ɶ���ɿƼ����޹�˾
* @author     		Go For It(1325536866)
* @version    		v1.0
* @Software 		IAR 7.7 or MDK 5.17
* @Target core		MK60DN512VLL10
* @Taobao   		https://seekfree.taobao.com/
* @date       		2016-02-25
********************************************************************************************************************/



#include "isr.h"
#include "control.h"
#include "headfile.h"

//-------------------------------------------------------------------------------------------------------------------
//  @brief      PROTA�ж�ִ�к���
//  @return     void
//  @since      v1.0
//  Sample usage:               ��A�������жϹ����ҷ����жϵ�ʱ����Զ�ִ�иú���
//-------------------------------------------------------------------------------------------------------------------
void PORTA_IRQHandler(void)
{

    //����жϱ�־��һ�ַ���ֱ�Ӳ����Ĵ�����ÿһλ��Ӧһ������
    PORTA->ISFR = 0xffffffff;
    //ʹ�����Ǳ�д�ĺ궨����������жϵ�����
    //PORTA_FLAG_CLR(A1);
}


extern uint8 barrier_flag;
void PORTB_IRQHandler(void)
{
    if(PORTB->ISFR&(1<<7))
    {
        VSYNC();
    }
    else if(PORTB->ISFR&(1<<19))
    {
		barrier_flag = 1;
    }
    //����жϱ�־��һ�ַ���ֱ�Ӳ����Ĵ�����ÿһλ��Ӧһ������
    PORTB->ISFR = 0xffffffff;
    //ʹ�����Ǳ�д�ĺ궨����������жϵ�����
    //PORTA_FLAG_CLR(A1);
}

void PORTC_IRQHandler(void)
{
    if(PORTC->ISFR&(1<<12))
    {
        //����
        Key_1();
    }
    else
    {
        if(PORTC->ISFR&(1<<13))
	{
            //����
            Key_2();
	}
        else
        {
            
        }
    }
    //����жϱ�־��һ�ַ���ֱ�Ӳ����Ĵ�����ÿһλ��Ӧһ������
    PORTC->ISFR = 0xffffffff;
    //ʹ�����Ǳ�д�ĺ궨����������жϵ�����
    //PORTC_FLAG_CLR(C1);
}

uint32 photo_switch_triger_span=0xffffffff;
void PORTE_IRQHandler(void)
{
    if(PORTE->ISFR&(1<<26))
    {
        if(current_element!=BREAK)
        {
            if(current_element==RAMP)
            {
                current_element=CURVE;
                pit_time_start(pit3);
            }
            else
            {
                if(photo_switch_triger_span==0xffffffff)
                {
                    current_element=RAMP;
                    photo_switch_triger_span=0;
                }
                else
                {
                    photo_switch_triger_span=pit_time_get(pit3);
                    if(photo_switch_triger_span>0x5ffffff0)
                    {
                        current_element=RAMP;
                    }
                }
            }
        }
    }
    //����жϱ�־��һ�ַ���ֱ�Ӳ����Ĵ�����ÿһλ��Ӧһ������
    PORTE->ISFR = 0xffffffff;
    //ʹ�����Ǳ�д�ĺ궨����������жϵ�����
}

void DMA0_IRQHandler(void)
{
    DMA_IRQ_CLEAN(DMA_CH0);
    row_finished();
}

void PIT0_IRQHandler(void)
{
    //����ٶȻ�
    Speed_Control();
    //���pit0�жϱ�־λ
    PIT_FlAG_CLR(pit0);
}

extern CAR_SPEED current_speed;

void PIT1_IRQHandler(void)
{
    time++;
    ramp_time++;
    //����pid����
    Different_Speed();
    //���pit1�жϱ�־λ
    PIT_FlAG_CLR(pit1);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UART3�ж�ִ�к���
//  @return     void
//  @since      v1.0
//  Sample usage:               ��UART3�����жϹ����ҷ����жϵ�ʱ����Զ�ִ�иú���
//-------------------------------------------------------------------------------------------------------------------
void UART3_RX_TX_IRQHandler(void)
{
    if(UART3->S1 & UART_S1_RDRF_MASK)                                     //�������ݼĴ�����
    {
        //�û���Ҫ�����������
        mt9v032_cof_uart_interrupt();
    }
    if(UART3->S1 & UART_S1_TDRE_MASK )                                    //�������ݼĴ�����
    {
        //�û���Ҫ����������

    }
}

void UART4_RX_TX_IRQHandler(void)
{
    if(UART4->S1 & UART_S1_RDRF_MASK)                                     //�������ݼĴ�����
    {
        Control_Remote();
		Ultrasonic_Get_Distance();
    }
    if(UART4->S1 & UART_S1_TDRE_MASK )                                    //�������ݼĴ�����
    {
        //�û���Ҫ����������

    }
}

void UART0_RX_TX_IRQHandler(void)
{
    if(UART0->S1 & UART_S1_RDRF_MASK)                                     //�������ݼĴ�����
    {
        Ultrasonic_Get_Distance();
    }
    if(UART0->S1 & UART_S1_TDRE_MASK )                                    //�������ݼĴ�����
    {
        //�û���Ҫ����������

    }
}

/*
�жϺ������ƣ��������ö�Ӧ���ܵ��жϺ���
Sample usage:��ǰ������DMA0�жϣ�Ȼ��͵�����ȥ���ĸ���DMA0���жϺ������ƣ��ҵ���дһ�������Ƶĺ�������
void DMA0_IRQHandler(void)
{
;
}
�ǵý����жϺ������־λ


DMA0_IRQHandler
DMA1_IRQHandler
DMA2_IRQHandler
DMA3_IRQHandler
DMA4_IRQHandler
DMA5_IRQHandler
DMA6_IRQHandler
DMA7_IRQHandler
DMA8_IRQHandler
DMA9_IRQHandler
DMA10_IRQHandler
DMA11_IRQHandler
DMA12_IRQHandler
DMA13_IRQHandler
DMA14_IRQHandler
DMA15_IRQHandler
DMA_Error_IRQHandler
MCM_IRQHandler
FTFL_IRQHandler
Read_Collision_IRQHandler
LVD_LVW_IRQHandler
LLW_IRQHandler
Watchdog_IRQHandler
RNG_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
CAN0_ORed_Message_buffer_IRQHandler
CAN0_Bus_Off_IRQHandler
CAN0_Error_IRQHandler
CAN0_Tx_Warning_IRQHandler
CAN0_Rx_Warning_IRQHandler
CAN0_Wake_Up_IRQHandler
I2S0_Tx_IRQHandler
I2S0_Rx_IRQHandler
CAN1_ORed_Message_buffer_IRQHandler
CAN1_Bus_Off_IRQHandler
CAN1_Error_IRQHandler
CAN1_Tx_Warning_IRQHandler
CAN1_Rx_Warning_IRQHandler
CAN1_Wake_Up_IRQHandler
Reserved59_IRQHandler
UART0_LON_IRQHandler
UART0_RX_TX_IRQHandler
UART0_ERR_IRQHandler
UART1_RX_TX_IRQHandler
UART1_ERR_IRQHandler
UART2_RX_TX_IRQHandler
UART2_ERR_IRQHandler
UART3_RX_TX_IRQHandler
UART3_ERR_IRQHandler
UART4_RX_TX_IRQHandler
UART4_ERR_IRQHandler
UART5_RX_TX_IRQHandler
UART5_ERR_IRQHandler
ADC0_IRQHandler
ADC1_IRQHandler
CMP0_IRQHandler
CMP1_IRQHandler
CMP2_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
FTM2_IRQHandler
CMT_IRQHandler
RTC_IRQHandler
RTC_Seconds_IRQHandler
PIT0_IRQHandler
PIT1_IRQHandler
PIT2_IRQHandler
PIT3_IRQHandler
PDB0_IRQHandler
USB0_IRQHandler
USBDCD_IRQHandler
ENET_1588_Timer_IRQHandler
ENET_Transmit_IRQHandler
ENET_Receive_IRQHandler
ENET_Error_IRQHandler
Reserved95_IRQHandler
SDHC_IRQHandler
DAC0_IRQHandler
DAC1_IRQHandler
TSI0_IRQHandler
MCG_IRQHandler
LPTimer_IRQHandler
Reserved102_IRQHandler
PORTA_IRQHandler
PORTB_IRQHandler
PORTC_IRQHandler
PORTD_IRQHandler
PORTE_IRQHandler
Reserved108_IRQHandler
Reserved109_IRQHandler
SWI_IRQHandler
*/



