/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2017,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_IIC.h
 * @brief      		模拟IIC函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @Software 		IAR 7.7 or MDK 5.17
 * @Target core		MK66FX
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2017-09-19
 * @note	
					模拟IIC接线定义
					------------------------------------ 
						SDA                 C17
						SCL                 C16
					------------------------------------ 
 ********************************************************************************************************************/



#ifndef _SEEKFREE_IIC_h
#define _SEEKFREE_IIC_h



#include "headfile.h"

typedef struct
{
  PTX_n SCL;
  PTX_n SDA;
}IIC_Port;

typedef enum IIC       //DAC模块
{
    IIC,
    SCCB
} IIC_type;



void  IIC_start(IIC_Port port);
void  IIC_stop(IIC_Port port);
void  IIC_ack_main(IIC_Port port,uint8 ack_main);
void  send_ch(IIC_Port port,uint8 c);
uint8 read_ch(IIC_Port port);
void  simiic_write_reg(IIC_Port port,uint8 dev_add, uint8 reg, uint8 dat);
uint8 simiic_read_reg(IIC_Port port,uint8 dev_add, uint8 reg, IIC_type type);
void  IIC_init(IIC_Port port);











#endif

