#include "common.h"
#include "MK60_pit.h"

//****************************Servo**********************************
#define Servo_FTM               ftm0
#define Servo_CH                ftm_ch7         //����ź�ͨ��
#define Servo_FTM_Frequency     100             //����ź�Ƶ��
#define Servo_Range             180             //�����Ƿ�Χ
#define Servo_Mid               1500            //����м�

void HDL_Servo_Init(void);                      //��ʼ�����
void HDL_Set_Angle(int angle);                //���ö��
//****************************Servo**********************************

//****************************Motor**********************************
#define Motor_Frequency                 10000           //����ź�Ƶ��
#define Motor_FTM                       ftm3
#define Motor_Left_Dir                  E8              //���������
#define Motor_Right_Speed               ftm_ch7         //�Ҳ����ٶ�
#define Motor_Left_Speed                ftm_ch6         //������ٶ�
#define Motor_Right_Dir                 E10             //�Ҳ�������
#define Motor_Speed_Limit_Factor        0.7f            //����޷�ϵ��

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
#define Buzzer_Pin              A19                     //����������
void HDL_Buzzer_Init();                                 //��������ʼ��
#define HDL_Buzzer_Beep()       gpio_set(Buzzer_Pin,1)  //��������
#define HDL_Buzzer_unBeep()     gpio_set(Buzzer_Pin,0)  //����������
//****************************Buzzer**********************************

//****************************Camera**********************************
#define HDL_Camera_Init()       camera_init()
//****************************Camera**********************************


//****************************LED**********************************
extern PTX_n LEDs[2];
void HDL_LED_Init();            //LED��ʼ��
//****************************LED**********************************

//****************************Key**********************************
void HDL_Key_Init();            //������ʼ��
//****************************Key**********************************

//*****************************Switch*****************************
void HDL_Switch_Init();         //���뿪�س�ʼ��
//*****************************Switch*****************************

//*****************************Remote Control*****************************
void HDL_RemoteControl_Init();                  //ң�س�ʼ��
//*****************************Remote Control*****************************

//*****************************Ultrasonic*****************************
void HDL_Ultrasonic_Init();
void HDL_Get_Distance_Once();
//*****************************Ultrasonic*****************************
