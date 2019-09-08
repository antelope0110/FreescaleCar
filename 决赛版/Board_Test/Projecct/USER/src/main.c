#include "headfile.h"
#include "control.h"
#include "mpu6050.h"
#include "SEGGER_RTT.h"
//#include "calibration.h"

 int main(void)
{
  get_clk();//上电后必须运行一次这个函数，获取各个频率信息，便于后面各个模块的参数设置
  EnableInterrupts;
  HAL_Init();
  //camera_calibration();
  //while(1);
  Control();
}
