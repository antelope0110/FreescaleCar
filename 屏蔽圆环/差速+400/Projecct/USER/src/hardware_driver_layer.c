#include "hardware_driver_layer.h"
#include "headfile.h"

//*****************************************Motor*****************************************
//电机初始化
void HDL_Motor_Init()
{
    gpio_init(Motor_Right_Dir,GPO,1);
    ftm_pwm_init(Motor_FTM, Motor_Left_Speed, Motor_Frequency, 0);
    ftm_pwm_init(Motor_FTM, Motor_Right_Speed, Motor_Frequency, 0);
    gpio_init(Motor_Left_Dir,GPO,1);
}

//设置电机速度
void HDL_Set_Speed(CAR_SPEED speed)
{
    //左轮
    if(speed.left_speed>=0)
    {
        if(speed.left_speed<Motor_Speed_Limit_Factor*FTM0_PRECISON)
        {
            gpio_set(Motor_Left_Dir,1);
            ftm_pwm_duty(Motor_FTM,Motor_Left_Speed,speed.left_speed);
        }
        else
        {
            gpio_set(Motor_Left_Dir,1);
            ftm_pwm_duty(Motor_FTM,Motor_Left_Speed,Motor_Speed_Limit_Factor*FTM0_PRECISON);
        }
    }
    else
    {
        if(speed.left_speed>-Motor_Speed_Limit_Factor*FTM0_PRECISON)
        {
            gpio_set(Motor_Left_Dir,0);
            ftm_pwm_duty(Motor_FTM,Motor_Left_Speed,-speed.left_speed);
        }
        else
        {
            gpio_set(Motor_Left_Dir,0);
            ftm_pwm_duty(Motor_FTM,Motor_Left_Speed,Motor_Speed_Limit_Factor*FTM0_PRECISON);
        }
    }
    //右轮
    if(speed.right_speed>=0)
    {
        if(speed.right_speed<Motor_Speed_Limit_Factor*FTM0_PRECISON)
        {
            gpio_set(Motor_Right_Dir,1);
            ftm_pwm_duty(Motor_FTM,Motor_Right_Speed,speed.right_speed);
        }
        else
        {
            gpio_set(Motor_Right_Dir,1);
            ftm_pwm_duty(Motor_FTM,Motor_Right_Speed,Motor_Speed_Limit_Factor*FTM0_PRECISON);
        }
    }
    else
    {
        if(speed.right_speed>-Motor_Speed_Limit_Factor*FTM0_PRECISON)
        {
            gpio_set(Motor_Right_Dir,0);
            ftm_pwm_duty(Motor_FTM,Motor_Right_Speed,-speed.right_speed);
        }
        else
        {
            gpio_set(Motor_Right_Dir,0);
            ftm_pwm_duty(Motor_FTM,Motor_Right_Speed,Motor_Speed_Limit_Factor*FTM0_PRECISON);
        }
    }
}
//*****************************************Motor*****************************************


//*****************************************Servo*****************************************
//舵机初始化
void HDL_Servo_Init()
{
    ftm_pwm_init(Servo_FTM,Servo_CH,Servo_FTM_Frequency,Servo_Mid);
}

//设置舵角
void HDL_Set_Angle(int angle)
{
    if (angle >= Servo_Range)
    {
        ftm_pwm_duty(Servo_FTM,Servo_CH,Servo_Mid+Servo_Range);
    }
    else if(angle<=-Servo_Range)
    {
        ftm_pwm_duty(Servo_FTM,Servo_CH,Servo_Mid-Servo_Range);
    }
    else
    {
        ftm_pwm_duty(Servo_FTM,Servo_CH,Servo_Mid+angle);
    }
}
//*****************************************Servo*****************************************

//*****************************************Encoder*****************************************
//初始化编码器
void HDL_Encoder_Init()
{
    ftm_quad_init(Encoder_Left);
    ftm_quad_init(Encoder_Right);
}

//获取编码器值
CAR_SPEED HDL_Encoder_Get_Value()
{
    CAR_SPEED speed;
    speed.right_speed=-ftm_quad_get(Encoder_Right);
    speed.left_speed=ftm_quad_get(Encoder_Left);
    return speed;
}

//清空编码器值
void HDL_Encoder_Clean()
{
    ftm_quad_clean(Encoder_Right);
    ftm_quad_clean(Encoder_Left);
}
//*****************************************Encoder*****************************************

//*****************************************Buzzer*****************************************
//初始化蜂鸣器
void HDL_Buzzer_Init()
{
    gpio_init(Buzzer_Pin,GPO,1);
}
//*****************************************Buzzer*****************************************

//*****************************************LED*****************************************
PTX_n LEDs[2]={C4,C5};
//初始化LED
void HDL_LED_Init()
{
    for(int i=0;i<sizeof(LEDs);i++)
    {
        gpio_init(LEDs[i],GPO,1);
    }
}
//*****************************************LED*****************************************

//*****************************************************Key******************************************************
//初始化按键
PTX_n Keys[2]={C12,C13};
void HDL_Key_Init()
{
    for(int i=0;i<sizeof(Keys);i++)
    {
        port_init(Keys[i],ALT1|IRQ_FALLING|PULLUP);
    }
}
//*****************************************************Key******************************************************

//*****************************************************Switch******************************************************
PTX_n Switches[4]={C0,C1,C2,C3};
//初始化拨码开关
void HDL_Switch_Init()
{
    for(int i=0;i<sizeof(Switches);i++)
    {
        gpio_init(Switches[i],GPI,0);
    }
}
//*****************************************************Switch******************************************************

//*****************************************************Remote Control******************************************************
//初始化遥控
void HDL_RemoteControl_Init()
{
    uart_init(uart4,9600);
    uart_rx_irq_en(uart4);
}
//*****************************************************Remote Control******************************************************

//*****************************************************Ultrasonic*****************************************
//-------------------------------------------------------------------------------------------------------------------
//  @brief      微秒级systick延时函数
//  @param      us              延时多少毫秒
//  @return     void
//  @since      v1.0
//  Sample usage:               systick_delay_us(1000);   //延时1000微秒
//-------------------------------------------------------------------------------------------------------------------
/*void systick_delay_us(uint32 us)
{
    //get_clk();//获取内核时钟便于后面设置
	while(us--) systick_delay(core_clk_mhz);
}*/

void HDL_Ultrasonic_Init()
{
//    uart_init(uart0,9600);
//    uart_rx_irq_en(uart0);
//    uart_putchar(uart0,0xE8);   //串口地址
//    systick_delay_us(50);
//    uart_putchar(uart0,0x02);   //寄存器
//    systick_delay_us(50);
//    uart_putchar(uart0,0x75);   //开启第六级降噪
//    systick_delay_ms(500);     //延时等待配置完成
    port_init(B19,PULLUP|ALT1|IRQ_FALLING);
}


void HDL_Get_Distance_Once()
{
    uart_putchar(uart0,0xE8);   //串口地址
    uart_putchar(uart0,0x02);   //寄存器
    uart_putchar(uart0,0x14);   //探测指令（探测量程1.5米,返回值范围79-8655μs）
}
//*****************************************************Ultrasonic*****************************************

