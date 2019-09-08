#include "SEEKFREE_MT9V032.h"
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
图像未进行索引变换，即图像最上侧索引为0，最下侧为img_height-1
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#define img_width       COL
#define img_height      ROW

extern const uint8 track_width[img_height];

//*******************************************************标定*******************************************
void Camera_Calibration();
void Get_Track_Width();
//*******************************************************标定*******************************************

//*******************************************************绘制标识*******************************************
void Show_Line();
//*******************************************************绘制标识*******************************************

//***************************************************找边线****************************************************
extern uint8 left_points[img_height];		//左边线
extern uint8 right_points[img_height];		//右边线
extern uint8 mid_points[img_height];		//此中线与左右边线算出来中线有区别（经过滤波，减缓中线跳变）
extern uint8 valid_row;						//有效边线范围（0:valid_row)
#define Valid_Row_Width         6			//有效行判定条件（赛道宽度）
#define Gray_Scale_Difference   13			//边界判断灰度差值
#define Light_Gray_Scale		80			//全局亮像素灰度值
#define Dark_Gray_Scale			170			//全局暗像素灰度值
uint8 Search_Edge(uint8 Row_Index, uint8 Start, uint8 End, uint8 Edge_Gray_Mutation);		//定位灰度跳变位置
void Get_Edge_Related();                                        //获取左右边界及中线
uint8 is_Track(uint8 Row_Index);								//判断边线范围内是否是赛道
//***************************************************找边线****************************************************

//***************************************************边线预处理****************************************************
typedef enum
{
	Right_Edge,
	Left_Edge
}EdgeSide;

void Smooth_Edge();
uint8 is_Straight_Edge(uint8 start_index, uint8 end_index, EdgeSide side);
//***************************************************边线预处理****************************************************

//*************************************************元素判断****************************************************
typedef enum
{
	CURVE,              //弯道,缺省类型
	CROSS,              //十字
	END,                //终点线
	BREAK,              //断路
	RAMP,               //坡道
	BARRIER,            //横断
	ROUNDABOUT,         //环岛
}TRACK_ELEMENT;

extern TRACK_ELEMENT current_element;	//目前赛道元素标识

extern uint64 time;                             //元素间隔计时单位100ms
void Refresh_Current_Element();			//刷新当前赛道元素

uint8 is_Cross();                       //判断‘十字’

#define End_Judge_Row			100      //终点线线判断行
#define End_Gray_Difference		30		//终点线跳变灰度差
uint8 is_End();                         //判断‘终点线’

#define Break_Valid_Row			90		//断路有效行最小值（图像最上侧索引为零，所以为最大值）
uint8 is_Break();                       //判断‘断路’

extern uint64 ramp_time;
#define Ramp_Judge_Row			50		//坡道判断行
uint8 is_Ramp();                        //判断‘坡道’

#define Roundabout_Mutation_Pixel	30	//圆环判断跳变像素数量
uint8 is_Roundabout();					//判断‘左环岛’与‘右圆环’
//*************************************************元素判断****************************************************

