

typedef struct
{
  int Kp;//比例系数
  int Kd;//微分系数
  int Ki;//积分系数
  float Error;//当前误差
  float Error1;//上一次误差
  float Error2;//上上次误差
  float Error_Sum;//误差累积
}PID_TypeDef;

int PID_Increment(int Target,float Actual,PID_TypeDef* PID_Struct);
int PID_Location(int Target,float Actual,PID_TypeDef* PID_Struct);