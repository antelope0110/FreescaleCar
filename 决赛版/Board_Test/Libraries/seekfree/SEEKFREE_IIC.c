/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2017,逐飞科技
* All rights reserved.
* 技术讨论QQ群：179029047
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file       		SEEKFREE_IIC.c
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



#include "SEEKFREE_IIC.h"


#define SDA(port)       gpio_get (port.SDA)
#define SDA0(port)      gpio_set (port.SDA, 0)		//IO口输出低电平
#define SDA1(port)      gpio_set (port.SDA, 1)		//IO口输出高电平  
#define SCL0(port)      gpio_set (port.SCL, 0)		//IO口输出低电平
#define SCL1(port)      gpio_set (port.SCL, 1)		//IO口输出高电平
#define DIR_OUT(port)   gpio_ddr (port.SDA, GPO)    //输出方向
#define DIR_IN(port)    gpio_ddr (port.SDA, GPI)    //输入方向


//内部数据定义
uint8 IIC_ad_main; //器件从地址	    
uint8 IIC_ad_sub;  //器件子地址	   
uint8 *IIC_buf;    //发送|接收数据缓冲区	    
uint8 IIC_num;     //发送|接收数据个数	     

#define ack 1      //主应答
#define no_ack 0   //从应答	 



//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC延时
//  @return     void						
//  @since      v1.0
//  Sample usage:				如果IIC通讯失败可以尝试增加j的值
//-------------------------------------------------------------------------------------------------------------------
void simiic_delay(void)
{
  //64为100K的速率(bus频率为100M)
  //特别提示OV7725的通信速率不能太高，最好50K左右，j设置为120通信速率为60K，60K的时候可以正常通信
  //其他IIC器件一般可以400K的通信速率
  uint16 j=300;   
  while(j--);
}


//内部使用，用户无需调用
void IIC_start(IIC_Port port)
{
  SDA1(port);
  SCL1(port);
  simiic_delay();
  SDA0(port);
  simiic_delay();
  SCL0(port);
}

//内部使用，用户无需调用
void IIC_stop(IIC_Port port)
{
  SDA0(port);
  SCL0(port);
  simiic_delay();
  SCL1(port);
  simiic_delay();
  SDA1(port);
  simiic_delay();
}

//主应答(包含ack:SDA=0和no_ack:SDA=0)
//内部使用，用户无需调用
void I2C_SendACK(IIC_Port port,unsigned char ack_dat)
{
  SCL0(port);
  simiic_delay();
  if(ack_dat) SDA0(port);
  else    	SDA1(port);
  
  SCL1(port);
  simiic_delay();
  SCL0(port);
  simiic_delay();
}


static int SCCB_WaitAck(IIC_Port port)
{
  SCL0(port);
  DIR_IN(port);
  simiic_delay();
  
  SCL1(port);
  simiic_delay();
  
  if(SDA(port))           //应答为高电平，异常，通信失败
  {
    DIR_OUT(port);
    SCL0(port);
    return 0;
  }
  DIR_OUT(port);
  SCL0(port);
  simiic_delay();
  return 1;
}

//字节发送程序
//发送c(可以是数据也可是地址)，送完后接收从应答
//不考虑从应答位
//内部使用，用户无需调用
void send_ch(IIC_Port port,uint8 c)
{
  uint8 i = 8;
  while(i--)
  {
    if(c & 0x80)	SDA1(port);//SDA 输出数据
    else			SDA0(port);
    c <<= 1;
    simiic_delay();
    SCL1(port);                //SCL 拉高，采集信号
    simiic_delay();
    SCL0(port);                //SCL 时钟线拉低
  }
  SCCB_WaitAck(port);
}

//字节接收程序
//接收器件传来的数据，此程序应配合|主应答函数|IIC_ack_main()使用
//内部使用，用户无需调用
uint8 read_ch(IIC_Port port)
{
  uint8 i;
  uint8 c;
  c=0;
  SCL0(port);
  simiic_delay();
  SDA1(port);             //置数据线为输入方式
  DIR_IN(port);
  for(i=0;i<8;i++)
  {
    simiic_delay();
    SCL0(port);         //置时钟线为低，准备接收数据位
    simiic_delay();
    SCL1(port);         //置时钟线为高，使数据线上数据有效
    simiic_delay();
    c<<=1;
    if(SDA(port)) c+=1;   //读数据位，将接收的数据存c
  }
  DIR_OUT(port);
  SCL0(port);
  simiic_delay();
  I2C_SendACK(port,no_ack);
  
  return c;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC写数据到设备寄存器函数
//  @param      dev_add			设备地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat				写入的数据
//  @return     void						
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void simiic_write_reg(IIC_Port port,uint8 dev_add, uint8 reg, uint8 dat)
{
  IIC_start(port);
  send_ch(port,(dev_add<<1) | 0x00);   //发送器件地址加写位
  send_ch(port,reg );   				 //发送从机寄存器地址
  send_ch(port,dat );   				 //发送需要写入的数据
  IIC_stop(port);
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC从设备寄存器读取数据
//  @param      dev_add			设备地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      type			选择通信方式是IIC  还是 SCCB
//  @return     uint8			返回寄存器的数据			
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
uint8 simiic_read_reg(IIC_Port port,uint8 dev_add, uint8 reg, IIC_type type)
{
  uint8 dat;
  IIC_start(port);
  send_ch(port, (dev_add<<1) | 0x00);  //发送器件地址加写位
  send_ch(port, reg );   				//发送从机寄存器地址
  if(type == SCCB)IIC_stop(port);
  
  IIC_start(port);
  send_ch(port, (dev_add<<1) | 0x01);  //发送器件地址加读位
  dat = read_ch(port);   				//发送需要写入的数据
  IIC_stop(port);
  
  return dat;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC端口初始化
//  @param      NULL
//  @return     void	
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
void IIC_init(IIC_Port port)
{
  gpio_init (port.SCL, GPO,1);
  gpio_init (port.SDA, GPO,1);
  
  
  port_init_NoAlt (port.SCL, ODO | PULLUP);//ODO
  port_init_NoAlt (port.SDA, ODO | PULLUP);
}

