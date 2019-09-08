#include "pid.h"

int PID_Increment(int Target,float Actual,PID_TypeDef* PID_Struct)
{
  int PID_Inc;
  PID_Struct->Error=Target-Actual;
  PID_Inc=(int)(PID_Struct->Kd*(PID_Struct->Error-PID_Struct->Error1)/1000+
    PID_Struct->Kp*PID_Struct->Error/1000+
      PID_Struct->Ki*(PID_Struct->Error-2*PID_Struct->Error1+PID_Struct->Error2)/1000);
  PID_Struct->Error2=PID_Struct->Error1;
  PID_Struct->Error1=PID_Struct->Error;
  return PID_Inc;
}

int PID_Location(int Target,float Actual,PID_TypeDef* PID_Struct)
{
  int PID_Loc;
  PID_Struct->Error=Target-Actual;
  PID_Struct->Error_Sum+=PID_Struct->Error;
  PID_Loc=(int)(PID_Struct->Kp*PID_Struct->Error/1000+
    PID_Struct->Kd*(PID_Struct->Error-PID_Struct->Error1)/1000+
      PID_Struct->Ki*PID_Struct->Error_Sum/1000);
  PID_Struct->Error1=PID_Struct->Error;
  return PID_Loc;
}
