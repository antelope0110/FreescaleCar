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
			Default_Car_Speed = 250;
			Tope_Car_Speed = 300;
			Break_Speed = 130;
			Default_Direction_Judge_Row = 65;

			gpio_set(LEDs[1],0);systick_delay_ms(1000);
			speed_mode = 1;
		}else if(gpio_get(Switches[1])==0){//亮两次
			Default_Car_Speed = 270;
			Tope_Car_Speed = 330;
			Break_Speed = 130;
			Default_Direction_Judge_Row = 63;

			gpio_set(LEDs[1],0);systick_delay_ms(500);
			gpio_set(LEDs[1],1);systick_delay_ms(500);
			gpio_set(LEDs[1],0);
			speed_mode = 2;
		}else if(gpio_get(Switches[2])==0){//亮三次
			Default_Car_Speed = 280;
			Tope_Car_Speed = 350;
			Break_Speed = 130;
			Default_Direction_Judge_Row = 60;

			gpio_set(LEDs[1],0);systick_delay_ms(300);
			gpio_set(LEDs[1],1);systick_delay_ms(300);
			gpio_set(LEDs[1],0);systick_delay_ms(300);
			gpio_set(LEDs[1],1);systick_delay_ms(300);
			gpio_set(LEDs[1],0);
			speed_mode = 3;
		}else if(gpio_get(Switches[3])==0){//亮四次
			Default_Car_Speed = 300;
			Tope_Car_Speed = 400;
			Break_Speed = 130;
			Default_Direction_Judge_Row = 58;

			gpio_set(LEDs[1],0);systick_delay_ms(200);
			gpio_set(LEDs[1],1);systick_delay_ms(200);
			gpio_set(LEDs[1],0);systick_delay_ms(200);
			gpio_set(LEDs[1],1);systick_delay_ms(200);
			gpio_set(LEDs[1],0);systick_delay_ms(200);
			gpio_set(LEDs[1],1);systick_delay_ms(200);
			gpio_set(LEDs[1],0);
			speed_mode = 4;
		}
	}
	HDL_Buzzer_unBeep();
}
//*****************************************************HAL_Init******************************************************

