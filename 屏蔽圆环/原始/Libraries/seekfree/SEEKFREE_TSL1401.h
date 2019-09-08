/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2017,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		SEEKFREE_TSL1401.h
 * @brief      		红孩儿(线阵CCD)函数库
 * @company	   		成都逐飞科技有限公司
 * @author     		Go For It(1325536866)
 * @Software 		IAR 7.7 or MDK 5.17
 * @Target core		MK66FX
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2017-09-19
 * @note	
					TSL1401接线定义：
					------------------------------------ 
					    模块管脚       	单片机管脚
					    CCD_AO	   		B0
					    CCD_CLK	   		E4
					    CCD_SI	   		E5
					------------------------------------ 
 ********************************************************************************************************************/



#ifndef _TSL1401_c
#define _TSL1401_c

#include "headfile.h"

extern uint16 ccd_data[128];                   //CCD数据


void ccd_init(void);
void ccd_collect(void);
















#endif

