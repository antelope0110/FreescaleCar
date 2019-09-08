#include "control.h"
#include "SEEKFREE_OLED.h"
#include "electro_magnetic.h"

CAR_SPEED current_speed={0,0};
uint8 send_img=0;
int length = 0;//总路程
float direction_error=0;
int Tope_Car_Speed = 0;     //最高车速
int Default_Car_Speed = 0;    //默认车速
int Break_Speed = 0;         //断路速度
int Default_Direction_Judge_Row = 0;      //默认方向判断行
float average_error=0;   //中线平均误差
//*****************************************************总控制******************************************************
int car_speed=0;
uint8 barrier_flag = 0;
uint8 barrier_count = 0;

//障碍处理
void Control_Barrier()
{
	//if(barrier_count==0){
		car_speed=100;
		HDL_Set_Angle(Servo_Range);
		systick_delay_ms(2000);
		HDL_Set_Angle(-Servo_Range);
		systick_delay_ms(3000);
	
}
extern uint8 round_count;
void Control()
{
	gpio_set(LEDs[0],0);
	while(1)
	{
		ElectroMagnetic_GetValue();
		if (mt9v032_finish_flag == 1)
		{
			Get_Edge_Related();
			Refresh_Current_Element();        //更新当前赛道元素
			gpio_turn(LEDs[0]);
			switch(current_element)
			{
			case CURVE:
				//if(get_index==0){
				if(round_count==1){
					if(valid_row>70&&barrier_count==0)//&&myabs(direction_error)<10)//&&average_error<10)//&&length>500&&length<1000)
            		{
            	      Control_Barrier();
					  barrier_count++;
            		}
					barrier_flag = 0;
				}
				//	HDL_Get_Distance_Once();
				//}
				HDL_Buzzer_unBeep();
				OLED_P8x16Str(0,3,"Curve");
				Control_Curve();
				break;
			case CROSS:
				HDL_Buzzer_Beep();
				OLED_P8x16Str(0,3,"Cross");
				Control_Cross();
				break;
			case END:
				HDL_Buzzer_Beep();
				OLED_P8x16Str(0,3,"End");
				Control_End();
				break;
			case BREAK:
				HDL_Buzzer_Beep();
				OLED_P8x16Str(0,3,"Break");
				Control_Break();
				break;
			case ROUNDABOUT:
				HDL_Buzzer_Beep();
				OLED_P8x16Str(0,3,"Roundabout");
				Control_Roundabout();
				break;
			case RAMP:
				HDL_Buzzer_Beep();
				OLED_P8x16Str(0,3,"Ramp");
				Control_Ramp();
				break;
				
			}
			if(send_img)
			{
				//Camera_Calibration();
				//Show_Line();
				seekfree_sendimg_032();
			}
			
			OLED_Print_Num1(0,0,length);
			OLED_Print_Num1(0,1,valid_row);
			mt9v032_finish_flag = 0;
		}
		
	}
}
//*****************************************************总控制******************************************************

//*****************************************************车方向控制******************************************************
uint8 direction_judge_row=0;
//舵机控制
void Control_Servo(PID_TypeDef pid)
{
	HDL_Set_Angle(PID_Location(0,direction_error,&pid));
}
//*****************************************************车方向控制******************************************************

//*****************************************************车速控制******************************************************
//总差速
extern uint8 round_state;
extern int speed_mode;
int different=0;
PID_TypeDef DifferentSpeed_PID={1000,0,0,0,0,0,0};
void Different_Speed()
{
	if(myabs(average_error)>10)
	{
		if(speed_mode<=2)
			DifferentSpeed_PID.Kp = 1000;
		else
			DifferentSpeed_PID.Kp = 1200;
		different=PID_Location(0,direction_error,&DifferentSpeed_PID);
	}
}

//电机转速控制
PID_TypeDef Speed_Left_PID={90000,8000,0,0,0,0,0};
PID_TypeDef Speed_Right_PID={90000,8000,0,0,0,0,0};
int temp_length = 0;
int8 length_count = 0;

int round_length = 0;

int round_error_count = 0;
int round_error;

uint8 emergency = 0;
				
void Speed_Control()
{
	if(round_state==2){
		round_error_count++;
		round_error += direction_error;
	}else if(round_state==4){
		round_error_count = 0;	
	}
	
	
	CAR_SPEED target_speed={0,0};
	target_speed.right_speed=car_speed+different;
	target_speed.left_speed=car_speed-different;
	if(target_speed.left_speed<=0)
	{
		target_speed.left_speed=0;
	}
	if(target_speed.right_speed<=0)
	{
		target_speed.right_speed=0;
	}
	current_speed = HAL_Get_Speed(Speed_Circuit_T);      //获取速度
	if(length_count<100){
		length_count++;
		temp_length += (current_speed.left_speed + current_speed.right_speed)/2;
	}else{
		length += temp_length/1000;
		length_count = 0;
		temp_length = 0;
	}
	if(current_element==ROUNDABOUT&&round_length == 0){//记录下进入圆环之前的路程
		round_length = length;
	}
				
	if(emergency){
		target_speed.right_speed=0;
		target_speed.left_speed=0;
	}
	CAR_SPEED speed_cricuit={current_speed.left_speed/10,current_speed.right_speed/10};
	speed_cricuit.left_speed+=PID_Increment(target_speed.left_speed,current_speed.left_speed/10,&Speed_Left_PID);
	speed_cricuit.right_speed+=PID_Increment(target_speed.right_speed,current_speed.right_speed/10,&Speed_Right_PID);
	HDL_Set_Speed(speed_cricuit);
}


/*
PID_TypeDef Speed_PID={30000,4000,0,0,0,0,0};
void Speed_Control()
{
current_speed = HAL_Get_Speed(Speed_Circuit_T);      //获取速度
CAR_SPEED speed_cricuit={(current_speed.left_speed+current_speed.right_speed)/20,
(current_speed.left_speed+current_speed.right_speed)/20};
speed_cricuit.left_speed+=PID_Increment(target_speed.left_speed,speed_cricuit.left_speed,&Speed_PID);
speed_cricuit.right_speed+=PID_Increment(target_speed.right_speed,speed_cricuit.right_speed,&Speed_PID);
HDL_Set_Speed(speed_cricuit);
}

PID_TypeDef DifferentSpeed_PID={1000,0,0,0,0,0,0};
//总差速
void Different_Speed()
{
Different_Speed_Control(DifferentSpeed_PID);
}

//摄像头差速
void Different_Speed_Control(PID_TypeDef PID)
{
int different=PID_Location(0,direction_error,&PID);
target_speed.right_speed=car_speed+different;
target_speed.left_speed=car_speed-different;
if(target_speed.left_speed<=0)
{
target_speed.left_speed=0;
	}
if(target_speed.right_speed<=0)
{
target_speed.right_speed=0;
	}
}
*/

//*****************************************************车速控制******************************************************

//*****************************************************元素分别控制******************************************************
//终点线处理
extern uint8 end_times;


//弯道处理
extern int left_value1,right_value1;
extern int MAGNETIC_CALIBRATION;
float last_error;
PID_TypeDef Servo_PID_Curve={2100,600,0,0,0,0,0};
void Control_Curve()
{
	direction_judge_row=Default_Direction_Judge_Row;
	int err[120];
	for(int i = 119;i>valid_row;--i){
		err[i] = 94 - mid_points[i];
	}
	float sum = 0;
	for(int i = 119;i>valid_row;--i){
		sum += err[i];
	}
	average_error = sum/(119 - valid_row);
	if(left_value1 + right_value1> MAGNETIC_CALIBRATION*3.f){
		car_speed=Default_Car_Speed - 50;
		if(valid_row<direction_judge_row)
		{
			direction_error=(right_points[direction_judge_row]+left_points[direction_judge_row]-img_width)/2;
			last_error=direction_error;
		}
		else
		{
			direction_error=(last_error>0)?(last_error+5):(last_error-5);
		}
	}else{
		if(fabs(average_error)<7&&valid_row<35)
		{
			car_speed=Tope_Car_Speed;
			if(valid_row<direction_judge_row)
			{
				direction_error=0.5f*((right_points[direction_judge_row]+left_points[direction_judge_row]-img_width)/2);
				last_error=direction_error;
			}
			else
			{
				direction_error=(last_error>0)?(last_error+5):(last_error-5);
			}
		}
		else{
			car_speed=Default_Car_Speed ;
			if(valid_row<direction_judge_row)
			{
				direction_error=(right_points[direction_judge_row]+left_points[direction_judge_row]-img_width)/2;
				last_error=direction_error;
			}
			else
			{
				direction_error=(last_error>0)?(last_error+5):(last_error-5);
			}
		}
	}
	
	Control_Servo(Servo_PID_Curve);//舵机pid控制
}

void Control_End()
{
	//systick_delay_ms(100);
	emergency = 1;
	
	direction_error=(right_points[direction_judge_row]+left_points[direction_judge_row]-img_width)/2;
	Control_Servo(Servo_PID_Curve);//舵机pid控制
}


//十字处理
void Control_Cross()
{
	direction_judge_row=Default_Direction_Judge_Row;
	car_speed=Default_Car_Speed;
	if(valid_row<direction_judge_row)
	{
		direction_error=0.7f*(mid_points[direction_judge_row]-img_width/2);
		last_error=direction_error;
	}
	else
	{
		direction_error=(last_error>0)?(last_error+5):(last_error-5);
	}
	Control_Servo(Servo_PID_Curve);//舵机pid控制
}

//环岛处理
uint8 r_jump_temp;
uint8 white;
uint8 white_sum;


void Control_LeftSmallRoundabout(){
	if (round_state == 1){
		car_speed = IN_SPEED;
		r_jump_temp = 0;
		white = 0;
		white_sum = 0;
		for (int i = 119; i > valid_row + 1; --i){
			if (left_points[i] - left_points[i - 1] < -10){
				r_jump_temp = i - 1;
				break;
			}
		}
		for (int i = 119; i>r_jump_temp; --i){
			if (left_points[i]<5)
				white++;
			
		}
		if (r_jump_temp>0 && r_jump_temp>30 && white>(119-r_jump_temp) * 3 / 4){//走到岔路口固定打角直接进圆环
			for (int i = 119; i>r_jump_temp; --i){
				right_points[i] = right_points[119] + (i - 119)*(float)(left_points[r_jump_temp] - right_points[119]) / (float)(r_jump_temp-119);
				mid_points[i] = limit(right_points[i]  - track_width[i]/2 -10, 0, 187);
			}
			valid_row = r_jump_temp;
			direction_judge_row = Round_Direction_Judge_Row;
			if (valid_row<direction_judge_row)
			{
				direction_error = 0.9f *(mid_points[direction_judge_row] - img_width / 2);
				last_error = direction_error;
			}
			else
			{
				direction_error = mid_points[valid_row] - img_width / 2;
				last_error = direction_error;
			}
			Control_Servo(Servo_PID_Curve);//舵机pid控制
			systick_delay_ms(IN_DELAY_TIME);
			round_state = 2;
		}
		else{//否则以右线求偏差
			direction_error = limit(right_points[Round_Direction_Judge_Row] - track_width[Round_Direction_Judge_Row] / 2 - 5 - img_width / 2, 0, 187);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}
	else if (round_state == 2){//进入圆环正常巡线
		car_speed = CRUISE_SPEED;
		uint8 right_white = 0;
		uint8 right_temp = 0;
		for(int i =119;i>valid_row;--i){
			if(right_points[i]>182)
				right_white++;
			if(right_temp ==0 && right_points[i] - right_points[i-1]<0&&right_points[i-1] - right_points[i-2]<0)
				right_temp = i;
		}
		if(right_temp>Round_Direction_Judge_Row){//&&right_white>(119 - valid_row)*2/5){
			round_state = 3;
		}
		else{
			direction_error = 0.9f*(mid_points[Round_Direction_Judge_Row] - img_width / 2);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}else if(round_state==3){
		car_speed = OUT_SPEED;
		direction_error = round_error / round_error_count;
		Control_Servo(Servo_PID_Curve);
		if(left_value1 + right_value1>MAGNETIC_CALIBRATION*3.5f)
			round_state = 4;
	}
	else if (round_state == 4){
		car_speed = Default_Car_Speed;
		for (int i = 119; i > valid_row; --i){
			mid_points[i] = right_points[i] - track_width[i]/2;
		}
		direction_error = (mid_points[Round_Direction_Judge_Row] - img_width / 2);
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if (left_value1 + right_value1 < MAGNETIC_CALIBRATION*2.5f){
			current_element = CURVE;
		}
		OLED_Print_Num1(64, 2, round_state);
	}
}

void Control_RightSmallRoundabout(){
	if (round_state == 1){
		car_speed = IN_SPEED;
		r_jump_temp = 0;
		white = 0;
		for (int i = 119; i > valid_row + 1; --i){
			if (right_points[i] - right_points[i - 1] > 10){
				r_jump_temp = i - 1;
				break;
			}
		}
		for (int i = 119; i>r_jump_temp; --i){
			if (right_points[i]>182)
				white++;
		}
		if (r_jump_temp>0 && r_jump_temp>30 && white>(119-r_jump_temp) * 3 / 4){//走到岔路口补线固定打角直接进圆环
			for (int i = 119; i>r_jump_temp; --i){
				left_points[i] = left_points[119] + (i - 119)*(float)(right_points[r_jump_temp] - left_points[119]) / (float)(r_jump_temp-119);
				mid_points[i] = limit(left_points[i] + track_width[i]/2 + 10, 0, 187);
			}
			valid_row = r_jump_temp;//更新有效行
			direction_judge_row = Round_Direction_Judge_Row;
			if (valid_row<direction_judge_row)
			{
				direction_error = 0.9f*(mid_points[direction_judge_row] - img_width / 2);
				last_error = direction_error;
			}
			else
			{
				direction_error = mid_points[valid_row] - img_width / 2;
				last_error = direction_error;
			}	
			Control_Servo(Servo_PID_Curve);//舵机pid控制
			systick_delay_ms(IN_DELAY_TIME);
			round_state = 2;
		}
		else{//否则以左线求偏差
			direction_error = limit(left_points[Round_Direction_Judge_Row] + track_width[Round_Direction_Judge_Row] / 2 + 5 - img_width / 2, 0, 187);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}
	else if (round_state == 2){//进入圆环正常巡线
		car_speed = CRUISE_SPEED;
		uint8 left_white = 0;
		uint8 left_temp = 0;
		for(int i =119;i>valid_row;--i){
			if(left_points[i]<5)
				left_white++;
			if(left_temp ==0 && left_points[i] - left_points[i-1]>0&&left_points[i-1] - left_points[i-2]>0)
				left_temp = i;
		}
		if(left_temp>Round_Direction_Judge_Row){//&&right_white>(119 - valid_row)*2/5){
			round_state = 3;
		}
		else{
			direction_error = 0.8f*(mid_points[Round_Direction_Judge_Row] - img_width / 2);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}else if(round_state==3){
		car_speed = OUT_SPEED;
		direction_error = round_error / round_error_count;
		Control_Servo(Servo_PID_Curve);
		if(left_value1 + right_value1>MAGNETIC_CALIBRATION*3.5f)
			round_state = 4;
	}
	else if (round_state == 4){
		car_speed = Default_Car_Speed;
		for (int i = 119; i > valid_row; --i){
			mid_points[i] = limit(left_points[i] + track_width[i]/2,0,187);
		}
		direction_error = (mid_points[Round_Direction_Judge_Row] - 10 - img_width / 2);
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if (left_value1 + right_value1 < MAGNETIC_CALIBRATION*3.f){//出圆环
			
			current_element = CURVE;
		}
		OLED_Print_Num1(64, 2, round_state);
	}
}

void Control_LeftBigRoundabout(){
	if (round_state == 1){//沿左线走
		car_speed = Default_Car_Speed;
		r_jump_temp = 0;
		for (int i = 119; i > valid_row + 1; --i){
			if (left_points[i] - left_points[i - 1] < -10){
				r_jump_temp = i - 1;
				break;
			}
		}
		valid_row = r_jump_temp + 1;
		direction_judge_row = Default_Direction_Judge_Row;	
		if (valid_row<direction_judge_row)
		{
			direction_error = 1.1f* (left_points[direction_judge_row] + track_width[direction_judge_row]/2 - img_width / 2);
			last_error = direction_error;
		}
		else
		{
			direction_error = left_points[valid_row]+ track_width[valid_row]/2 - img_width / 2;
			last_error = direction_error;
		}
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if(left_value1+right_value1<MAGNETIC_CALIBRATION*2.5f)
			round_state = 2;
		OLED_Print_Num1(64, 2, round_state);
	}
	else if (round_state == 2){//进入圆环正常巡线
		car_speed = Default_Car_Speed;
		uint8 right_white = 0;
		uint8 right_temp = 0;
		for(int i =119;i>valid_row;--i){
			if(right_points[i]>182)
				right_white++;
			if(right_temp ==0 && right_points[i] - right_points[i-1]<0&&right_points[i-1] - right_points[i-2]<0)
				right_temp = i;
		}
		if(right_temp>Round_Direction_Judge_Row){//&&right_white>(119 - valid_row)*2/5){
			round_state = 3;
		}
		else{
			direction_error = 0.9f*(mid_points[Round_Direction_Judge_Row] - img_width / 2);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}else if(round_state==3){
		car_speed = Default_Car_Speed;
		direction_error = round_error / round_error_count;
		Control_Servo(Servo_PID_Curve);
		if(left_value1 + right_value1>MAGNETIC_CALIBRATION*3.5f)
			round_state = 4;
	}
	else if (round_state == 4){
		car_speed = Default_Car_Speed;
		for (int i = 119; i > valid_row; --i){
			mid_points[i] = right_points[i] - track_width[i]/2;
		}
		direction_error = (mid_points[Round_Direction_Judge_Row] - img_width / 2);
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if (left_value1 + right_value1 < MAGNETIC_CALIBRATION*2.5f){
			current_element = CURVE;
		}
		OLED_Print_Num1(64, 2, round_state);
	}
}

void Control_RightBigRoundabout(){
	
	if (round_state == 1){//沿左线走
		car_speed = Default_Car_Speed;
		r_jump_temp = 0;
		for (int i = 119; i > valid_row + 1; --i){
			if (right_points[i] - right_points[i - 1] > 10){
				r_jump_temp = i - 1;
				break;
			}
		}
		valid_row = r_jump_temp + 1;
		direction_judge_row = Default_Direction_Judge_Row;	
		if (valid_row<direction_judge_row)
		{
			direction_error = 1.1f* (right_points[direction_judge_row] - track_width[direction_judge_row]/2 - img_width / 2);
			last_error = direction_error;
		}
		else
		{
			direction_error = right_points[valid_row] - track_width[valid_row]/2 - img_width / 2;
			last_error = direction_error;
		}
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if(left_value1+right_value1<MAGNETIC_CALIBRATION*2.5f)
			round_state = 2;
	}
	else if (round_state == 2){//进入圆环正常巡线
		car_speed = Default_Car_Speed;
		uint8 left_white = 0;
		uint8 left_temp = 0;
		for(int i =119;i>valid_row;--i){
			if(left_points[i]<5)
				left_white++;
			if(left_temp ==0 && left_points[i] - left_points[i-1]>0&&left_points[i-1] - left_points[i-2]>0)
				left_temp = i;
		}
		if(left_temp>Round_Direction_Judge_Row){//&&right_white>(119 - valid_row)*2/5){
			round_state = 3;
		}
		else{
			direction_error = 0.8f*(mid_points[Round_Direction_Judge_Row] - img_width / 2);
			Control_Servo(Servo_PID_Curve);//舵机pid控制
		}
		OLED_Print_Num1(64, 2, round_state);
	}else if(round_state==3){
		car_speed = Default_Car_Speed;
		direction_error = round_error / round_error_count;
		Control_Servo(Servo_PID_Curve);
		if(left_value1 + right_value1>MAGNETIC_CALIBRATION*3.5f)
			round_state = 4;
	}
	else if (round_state == 4){
		car_speed = Default_Car_Speed;
		for (int i = 119; i > valid_row; --i){
			mid_points[i] = limit(left_points[i] + track_width[i]/2,0,187);
		}
		direction_error = (mid_points[Round_Direction_Judge_Row] - 10 - img_width / 2);
		Control_Servo(Servo_PID_Curve);//舵机pid控制
		if (left_value1 + right_value1 < MAGNETIC_CALIBRATION*3.f){//出圆环
			
			current_element = CURVE;
		}
		OLED_Print_Num1(64, 2, round_state);
	}
}


extern uint8 round_kind;
void Control_Roundabout()
{
	
	OLED_Print_Num1(64, 0, round_kind);
	if (round_kind == 1)
	{
		Control_LeftSmallRoundabout();
	}
	else if (round_kind == 2)
	{
		Control_LeftBigRoundabout();
	}
	else if(round_kind==3)
	{
		Control_RightSmallRoundabout();
	}
	else if(round_kind==4)
	{
		Control_RightBigRoundabout();
	}
}

//断路处理
PID_TypeDef Magnet_Servo_PID_Curve = {1400,0,0,0,0,0,0};
void Control_Break()
{
	car_speed=Break_Speed;
	direction_error= ElectroMagnetic_GetValue();
	Control_Servo(Magnet_Servo_PID_Curve);//舵机pid控制
}

//坡道处理
void Control_Ramp()
{
	car_speed=250;
	direction_judge_row=80;
	if(valid_row<direction_judge_row)
	{
		direction_error=(right_points[direction_judge_row]+left_points[direction_judge_row]-img_width)/2;
		last_error=direction_error;
	}
	else
	{
		direction_error=(last_error>0)?(last_error+5):(last_error-5);
	}
	Control_Servo(Servo_PID_Curve);//舵机pid控制
}


//*****************************************************元素分别控制******************************************************

//*****************************************************遥控控制******************************************************
void Control_Remote()
{
	uint8 buffer;
	uart_querychar(uart4,&buffer);
	switch(buffer)
	{
	case '1':
		break;
	case '2':
		break;
	case '3':
		break;
	case '4':
		break;
	case '5':
		break;
	case '6':
		break;
	}
}
//*****************************************************遥控控制******************************************************

//*****************************************************按键控制******************************************************
void Key_1()
{
	send_img=send_img?0:1;      //开关画线传图
}

void Key_2()
{
	car_speed=0;
}
//*****************************************************按键控制******************************************************

//*****************************************************超声波控制******************************************************
uint16 barrier_distance=0xffff;
uint8 get_index=0;
void Ultrasonic_Get_Distance()
{
	uint8 buff;
	uart_querychar(uart0,&buff);
	if(get_index==0)
	{
		barrier_distance=buff;
		get_index=1;
	}
	else
	{
		barrier_distance=(barrier_distance<<8)+buff;
		get_index=0;
	}
}
//*****************************************************超声波控制******************************************************

