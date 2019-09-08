#include "common.h"
#include "MK60_pit.h"

//****************************Servo**********************************
#define Servo_FTM               ftm0
#define Servo_CH                ftm_ch7         //舵机信号通道
#define Servo_FTM_Frequency     100             //舵机信号频率
#define Servo_Range             180             //舵机打角范围
#define Servo_Mid               1500            //舵机中间

void HDL_Servo_Init(void);                      //初始化舵机
void HDL_Set_Angle(int angle);                //设置舵角
//****************************Servo**********************************

//****************************Motor**********************************
#define Motor_Frequency                 10000           //电机信号频率
#define Motor_FTM                       ftm3
#define Motor_Left_Dir                  E8              //左侧电机方向
#define Motor_Right_Speed               ftm_ch7         //右侧电机速度
#define Motor_Left_Speed                ftm_ch6         //左侧电机速度
#define Motor_Right_Dir                 E10             //右侧电机方向
#define Motor_Speed_Limit_Factor        0.7f            //电机限幅系数

typedef enum
{
  Motor_Right = 0,
  Motor_Left,
  Motor_Both
} MOTOR_SIDE;

typedef struct
{
  int left_speed;
  int right_speed;
}CAR_SPEED;

void HDL_Motor_Init(void);
void HDL_Set_Speed(CAR_SPEED);
//****************************Motor**********************************

//****************************Encoder**********************************
#define Encoder_Left            ftm2
#define Encoder_Right           ftm1

void HDL_Encoder_Init(void);
CAR_SPEED HDL_Encoder_Get_Value();
void HDL_Encoder_Clean();
//****************************Encoder**********************************


//****************************Buzzer**********************************
#define Buzzer_Pin              A19                     //蜂鸣器引脚
void HDL_Buzzer_Init();                                 //蜂鸣器初始化
#define HDL_Buzzer_Beep()       gpio_set(Buzzer_Pin,1)  //蜂鸣器响
#define HDL_Buzzer_unBeep()     gpio_set(Buzzer_Pin,0)  //蜂鸣器不响
//****************************Buzzer**********************************

//****************************Camera**********************************
#define HDL_Camera_Init()       camera_init()
//****************************Camera**********************************


//****************************LED**********************************
extern PTX_n LEDs[2];
void HDL_LED_Init();            //LED初始化
//****************************LED**********************************

//****************************Key**********************************
void HDL_Key_Init();            //按键初始化
//****************************Key**********************************

//*****************************Switch*****************************
void HDL_Switch_Init();         //拨码开关初始化
//*****************************Switch*****************************

//*****************************Remote Control*****************************
void HDL_RemoteControl_Init();                  //遥控初始化
//*****************************Remote Control*****************************

//*****************************Ultrasonic*****************************
void HDL_Ultrasonic_Init();
void HDL_Get_Distance_Once();
//*****************************Ultrasonic*****************************
