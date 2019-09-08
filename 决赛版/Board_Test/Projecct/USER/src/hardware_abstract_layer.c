#include "hardware_abstract_layer.h"
#include "SEEKFREE_NRF24L01.h"
#include "electro_magnetic.h"
#include "mpu6050.h"

uint64 time=0;  //计时单位：100ms
//*****************************************************Car_Speed******************************************************
#define Wheel_Circumference     210     //轮周uint:mm
#define Pulse_Per_Round         1160
CAR_SPEED HAL_Get_Speed(int T)//编码器读取电机速度           T计数周期uint:ms        返回速度uint:mm/s
{
    CAR_SPEED speed={0,0};
    speed=HDL_Encoder_Get_Value();
    speed.left_speed*=(Wheel_Circumference*1000/Pulse_Per_Round/T);
    speed.right_speed*=(Wheel_Circumference*1000/Pulse_Per_Round/T);
    HDL_Encoder_Clean();
    return speed;
}
//*****************************************************Car_Speed******************************************************

//*****************************************************PIT_Init******************************************************
void PIT_Init()
{
    pit_init_ms(pit0,Speed_Circuit_T);
    pit_init_ms(pit1,100);
    enable_irq(PIT0_IRQn);
    enable_irq(PIT1_IRQn);
}
//*****************************************************PIT_Init******************************************************

int speed_mode = 0;
extern PTX_n Switches[4];
//*****************************************************HAL_Init******************************************************
extern int Default_Car_Speed,Tope_Car_Speed,Break_Speed;
extern int Default_Direction_Judge_Row;
void HAL_Init()
{
	HDL_Buzzer_Init();
    OLED_Init();
    HDL_Servo_Init();
    HDL_Motor_Init();
    HDL_Encoder_Init();
    HDL_Switch_Init();
    HDL_Key_Init();
    HDL_LED_Init();
    uart_init(CAMERA_UART, 512000);
    HDL_Camera_Init();
    HDL_RemoteControl_Init();
    PIT_Init();
    HDL_Ultrasonic_Init();
    ElectroMagnetic_Init();
	while(!speed_mode){
		if(gpio_get(Switches[0])==0){//亮一次
			Default_Car_Speed = 230;
			Tope_Car_Speed = 270;
			Break_Speed = 100;
			Default_Direction_Judge_Row = 65;
			
			gpio_set(LEDs[1],0);systick_delay_ms(1000);
			speed_mode = 1;
		}
	}
	HDL_Buzzer_unBeep();
}
//*****************************************************HAL_Init******************************************************

