#include "img_process.h"
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
图像未进行索引变换，即图像最上侧索引为0，最下侧为img_height-1
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

uint8 left_points[img_height];
uint8 right_points[img_height];
uint8 mid_points[img_height];			//此中线与左右边线算出来中线有区别（经过滤波，减缓中线跳变）
uint8 valid_row = img_height - 1;		//有效行索引
const uint8 track_width[img_height] = { 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
8 ,9 ,11 ,13 ,15 ,17 ,19 ,21 ,25 ,27 ,29 ,32 ,34 ,37 ,40 ,
42 ,45 ,47 ,49 ,51 ,53 ,56 ,58 ,60 ,61 ,63 ,66 ,69 ,71 ,73 ,
75 ,77 ,79 ,82 ,84 ,87 ,89 ,91 ,93 ,95 ,97 ,99 ,101 ,104 ,106 ,
108 ,110 ,112 ,114 ,116 ,118 ,120 ,122 ,124 ,126 ,128 ,130 ,132 ,133 ,135 ,
137 ,139 ,141 ,143 ,144 ,146 ,148 ,150 ,151 ,153 ,154 ,156 ,158 ,159 ,160 ,
164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,172 ,173 ,174 ,175 ,176 ,177 ,
178 ,179 ,179 ,180 ,181 ,181 ,182 ,183 ,183 ,183 ,183 ,183 ,187 ,187 ,187 };          //各行正常赛道宽度
/*
uint8 track_width[img_height] = { 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,
8 ,9 ,11 ,13 ,15 ,17 ,19 ,21 ,25 ,27 ,29 ,32 ,34 ,37 ,40 ,
42 ,45 ,47 ,49 ,51 ,53 ,56 ,58 ,60 ,61 ,63 ,66 ,69 ,71 ,73 ,
75 ,77 ,79 ,82 ,84 ,87 ,89 ,91 ,93 ,95 ,97 ,99 ,101 ,104 ,106 ,
108 ,110 ,112 ,114 ,116 ,118 ,120 ,122 ,124 ,126 ,128 ,130 ,132 ,133 ,135 ,
137 ,139 ,141 ,143 ,144 ,146 ,148 ,150 ,151 ,153 ,154 ,156 ,158 ,159 ,160 ,
164 ,165 ,166 ,167 ,168 ,169 ,170 ,171 ,172 ,172 ,173 ,174 ,175 ,176 ,177 ,
178 ,179 ,179 ,180 ,181 ,181 ,182 ,183 ,183 ,183 ,183 ,183 ,187 ,187 ,187 };          //各行正常赛道宽度
*/


extern PTX_n Switches[4];

//***************************************************环岛**************************************************
uint8 round_state;//1进环，2圆环中，3出环
extern int right_value1, left_value1;
const int MAGNETIC_CALIBRATION = 130;
uint8 round_kind = 1;//1左小环，2,左大环，3右小环，4,右大环
uint8 round_count = 1;//计环个数
//Lose_Line round_lose;
uint8 a = 0;
uint8 is_Roundabout()
{
	round_state = 0;
    if (left_value1 + right_value1 > MAGNETIC_CALIBRATION*4.f){
		if(gpio_get(Switches[3])==1){
			if(round_count==1){//第一个环
        	    round_kind = 1;
				if(gpio_get(Switches[1])==0){//是否屏蔽
					round_state = 4;
				}else{
					round_state = 1;
				}
			
        		round_count++;
        	}else if(round_count==2){//第二个环
        	    round_kind = 1;
				if(gpio_get(Switches[2])==0){//是否屏蔽
					round_state = 4;
				}else{
					round_state = 1;
				}
        		round_count++;
        	}/*else if(round_count==3){//第三个环
        	    round_kind = 1;
				if(gpio_get(Switches[3])==0){//是否屏蔽
					round_state = 4;
				}else{
					round_state = 1;
				}
        		round_count++;
        	}*/else {
        	    return 0;
        	}
		}else{
			if(round_count==1){//第一个环
            	round_kind = 3;
				if(gpio_get(Switches[1])==0){//是否屏蔽
					round_state = 4;
				}else{
					round_state = 1;
				}
        		round_count++;
        	}else if(round_count==2){//第二个环
            	round_kind = 3;
				if(gpio_get(Switches[2])==0){//是否屏蔽
					round_state = 4;
				}else{
					round_state = 1;
				}
        		round_count++;
        	}else {
            return 0;
        	}
		}
        return 1;
    }
    return 0;
}
//***************************************************环岛**************************************************







//*******************************************************标定*******************************************
//摄像头位置参考线绘制
void Camera_Calibration()
{
    //横线
    for (int i = 0; i < img_width; i++)
    {
        image[50][i] = 255;
    }
    //纵线
    for (int i = 0; i < img_height - 1; i++)
    {
        image[i][img_width / 2] = 255;
    }
}

////获取各行正常赛道宽度（将车置于长直道使用）
//void Get_Track_Width()
//{
//	for (int i = img_height - 1; i >= 0; i--)
//	{
//		track_width[i] = right_points[i] - left_points[i];
//	}
//}
//*******************************************************标定*******************************************

//*******************************************************绘制标识*******************************************
void Show_Line()
{
    for (int i = img_height - 1; i >= valid_row; i--)
    {
        image[i][right_points[i] - 5] = 0;
        image[i][left_points[i] + 5] = 0;
    }
    for (int i = img_width - 1; i >= 0; i--)
    {
        image[valid_row][i] = 255;
    }
}
//*******************************************************绘制标识*******************************************





//***************************************************找边线****************************************************
//定位灰度跳变位置
uint8 Search_Edge(uint8 Row_Index, uint8 Start, uint8 End, uint8 Edge_Gray_Mutation)
{
    if (Start > End)
    {
        //从右往左找（找左边线）
        for (int i = Start; i >= End; i--)
        {
            //判断灰度跳变，跳过一个像素（跳变边缘存在灰度过渡）
            if (i - 3 <= 0)
            {
                //边界位于图像左侧,不可能位于图像右侧
                return i - 1;
            }
            else if (image[Row_Index][i] - image[Row_Index][i - 3] >= Edge_Gray_Mutation)
            {
                return i - 2;
            }
        }
        return 1;
    }
    else
    {
        //从左往右找（找右边线）
        for (int i = Start; i <= End; i++)
        {
            //判断灰度跳变，跳过一个像素（跳变边缘存在灰度过渡）
            if (i + 3 > img_width)
            {
                return i + 1;
            }
            else
            {
                if (image[Row_Index][i] - image[Row_Index][i + 3] >= Edge_Gray_Mutation)
                {
                    return i + 2;
                }
            }
        }
        return img_width - 1;
    }
}
 int end_start = -1; int end_end = -1;
 
 uint8 IS_END(uint8 start_index,uint8 row_index)
{
    uint8 mutation_num = 0;
    for (int i = 0; i < (start_index > img_width / 2 ? img_width - start_index : start_index) - 1; i++)
    {
        if (abs(image[row_index][start_index + i - 1] - image[row_index][start_index + i]) > End_Gray_Difference ||
            abs(image[row_index][start_index - i + 1] - image[row_index][start_index - i]) > End_Gray_Difference)
        {
            mutation_num++;
        }
        if (mutation_num > 8)
        {
            return 1;
        }
    }
    return 0;
}
 
 
//获取左右边界
//获取左右边界

void Get_Edge_Related()
{
	int end_start = -1; int end_end = -1;
	uint8 last_middle = COL / 2;
	for (int i = img_height - 1; i >= 0; i--)
	{
		//右边线
		right_points[i] = Search_Edge(i, last_middle, img_width - 1, Gray_Scale_Difference);
		//左边线
		left_points[i] = Search_Edge(i, last_middle, 0, Gray_Scale_Difference);
		//中线缓存
		mid_points[i] = (right_points[i] + left_points[i]) / 2;
		//左边线矫正（矫正因灰度过渡导致边线外扩）
		for (int j = 1; j < Gray_Scale_Difference; j++)
		{
			if (image[i][left_points[i] + 1] < Dark_Gray_Scale - 10)
			{
				left_points[i] = Search_Edge(i, last_middle, 0, Gray_Scale_Difference - j);
			}
			else
			{
				break;
			}
		}
		//右边线矫正（矫正因灰度过渡导致边线外扩）
		for (int j = 1; j < Gray_Scale_Difference; j++)
		{
			if (image[i][right_points[i] - 1] < Dark_Gray_Scale - 10)
			{
				right_points[i] = Search_Edge(i, last_middle, img_width - 1, Gray_Scale_Difference - j);
			}
			else
			{
				break;
			}
		}
		if (IS_END(last_middle, i))
		{
			if (end_start == -1)
			{
				end_start = i + 3;
			}
		}
		else
		{
			//终点线屏蔽（边线补线）
			if (end_start != -1 && end_end == -1)
			{
				end_end = i - 3;
			}
			if (i == end_end)
			{
				float k_right = (float)(right_points[end_start] - right_points[i]) / (end_start - i);
				float k_left = (float)(left_points[end_start] - left_points[i]) / (end_start - i);
				for (int j = end_start; j > i; j--)
				{
					right_points[j] = right_points[end_start] + k_right * (j - end_start);
					left_points[j] = left_points[end_start] + k_left * (j - end_start);
				}
				end_start = -1;
				end_end = -1;
			}
			//找线终止条件
			if (!is_Track(i))
			{
				valid_row = i;
				break;
			}
		}
		valid_row = 0;
		//mid_points滤波
		if (last_middle - mid_points[i] > 5)
		{
			mid_points[i] = last_middle - 2;
		}
		else if (mid_points[i] - last_middle > 5)
		{
			mid_points[i] = last_middle + 2;
		}
		last_middle = mid_points[i];
	}
	Smooth_Edge();
}

//判断边线范围内是否是赛道
uint8 is_Track(uint8 Row_Index)
{
	if (image[Row_Index][(right_points[Row_Index] + left_points[Row_Index]) / 2] < Dark_Gray_Scale&&
		image[Row_Index][right_points[Row_Index]-1] < Dark_Gray_Scale&&
		image[Row_Index][left_points[Row_Index] + 1] < Dark_Gray_Scale)
	{
		return 0;
	}
	return 1;
}
//***************************************************找线****************************************************

//***************************************************边线预处理****************************************************
//平滑边线
void Smooth_Edge()
{
    for (int i = img_height - 1; i > valid_row + 2; i--)
    {
        //单点异常
        if (abs(right_points[i] - right_points[i - 2]) < 5
            && abs(right_points[i] - right_points[i - 1]) > 10)
        {
            right_points[i - 1] = (right_points[i] + right_points[i - 2]) / 2;
        }
        if (abs(left_points[i] - left_points[i - 2]) < 5 && abs(left_points[i] - left_points[i - 1]) > 10)
        {
            left_points[i - 1] = (left_points[i] + left_points[i - 2]) / 2;
        }
        //双点异常
        if (abs(right_points[i] - right_points[i - 3]) < 5
            && abs(right_points[i - 1] - right_points[i - 2]) < 5
                && abs(right_points[i] - right_points[i - 1]) > 10)
        {
            right_points[i - 1] = (right_points[i] + right_points[i - 3]) / 2;
            right_points[i - 2] = (right_points[i] + right_points[i - 3]) / 2;
        }
        if (abs(left_points[i] - left_points[i - 3]) < 5
            && abs(left_points[i - 1] - left_points[i - 2]) < 5
                && abs(left_points[i] - left_points[i - 1]) > 10)
        {
            left_points[i - 1] = (left_points[i] + left_points[i - 3]) / 2;
            left_points[i - 2] = (left_points[i] + left_points[i - 3]) / 2;
        }
        //三点异常
        if (abs(right_points[i] - right_points[i - 4]) < 5
            && abs(right_points[i - 1] - right_points[i - 3]) < 5
                && abs(right_points[i] - right_points[i - 1]) > 10)
        {
            right_points[i - 1] = (right_points[i] + right_points[i - 4]) / 2;
            right_points[i - 2] = (right_points[i] + right_points[i - 4]) / 2;
            right_points[i - 3] = (right_points[i] + right_points[i - 4]) / 2;
        }
        if (abs(left_points[i] - left_points[i - 4]) < 5
            && abs(left_points[i - 1] - left_points[i - 3]) < 5
                && abs(left_points[i] - left_points[i - 1]) > 10)
        {
            left_points[i - 1] = (left_points[i] + left_points[i - 4]) / 2;
            left_points[i - 2] = (left_points[i] + left_points[i - 4]) / 2;
            left_points[i - 3] = (left_points[i] + left_points[i - 4]) / 2;
        }
    }
}

//判断是否是直线（根据开始点与结束点生成直线，将每点与直线作比较）
uint8 is_Straight_Edge(uint8 start_index, uint8 end_index, EdgeSide side)
{
    float k;
    switch (side)
    {
      case Right_Edge:
        if (start_index == end_index)
        {
            return 1;
        }
        else
        {
            k = (float)(right_points[start_index] - right_points[end_index]) / (start_index - end_index);
            for (int i = (start_index < end_index ? start_index : end_index);
                 i <= (start_index < end_index ? end_index : start_index); i++)
            {
                if (fabs(right_points[i] - (k * (i - start_index) + right_points[start_index])) > 2)
                {
                    return 0;
                }
                if (i == (start_index < end_index ? end_index : start_index))
                {
                    return 1;
                }
            }
        }
      case Left_Edge:
        if (start_index == end_index)
        {
            return 1;
        }
        else
        {
            k = (float)(left_points[start_index] - left_points[end_index]) / (start_index - end_index);
            for (int i = (start_index < end_index ? start_index : end_index);
                 i <= (start_index < end_index ? end_index : start_index); i++)
            {
                if (fabs(left_points[i] - (k * (i - start_index) + left_points[start_index])) > 2)
                {
                    return 0;
                }
                if (i == (start_index < end_index ? end_index : start_index))
                {
                    return 1;
                }
            }
        }
      default:
        return 0;
    }
}
//***************************************************边线预处理****************************************************
extern int length;
//*************************************************元素判断****************************************************
TRACK_ELEMENT current_element; //当前赛道元素
//刷新当前赛道元素，终点线->路障->十字->圆环->断路->坡道->弯道
uint64 ramp_time=0;
void Refresh_Current_Element()
{
    if(current_element!=END&&current_element!=RAMP&&current_element!=ROUNDABOUT)
    {
       if (is_Cross())
       {
           current_element = CROSS;
       }
        else if (is_Roundabout() != 0)
        {
            current_element = ROUNDABOUT;
        }
        else if (is_Break())
        {
            current_element = BREAK;
        }
        else if (is_Ramp())
        {
            current_element = RAMP;
        }
        else
        {
            current_element = CURVE;
            ramp_time=0;
        }
	   //if(length<100||length>3500){
       		if (is_End())
       		{
       		    current_element = END;
       		}
		//}
    }
    else
    {
        if(ramp_time>=15&&current_element==RAMP)
        {
            current_element=CURVE;
            ramp_time=0;
        }
    }
}

//***************************************************终点线**************************************************
uint8 is_Break()
{
    uint8 mutation_num=0;
    if (valid_row > Break_Valid_Row)
    {
        for(int i=0;i<60;i++)
        {
            //排除终点线
            if(abs(image[valid_row-3][img_width/2+i]-image[valid_row-3][img_width/2+i+1])>End_Gray_Difference)
            {
                mutation_num++;
            }
             if(abs(image[valid_row-3][img_width/2-i]-image[valid_row-3][img_width/2-i-1])>End_Gray_Difference)
            {
                mutation_num++;
            }
            if(mutation_num>4)
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}
//***************************************************断路**************************************************

//***************************************************终点线**************************************************

//***************************************************十字**************************************************
typedef struct{
    int left_lose_num;
    int left_lose_end;
    int right_lose_num;
    int right_lose_end;
}Lose_Line;


uint8 width_flag;//赛道宽度标志位
uint8 left_cross_inflection, right_cross_inflection;//拐点
uint8 left_c_jump, right_c_jump;//跳变点
uint8 left_direction = 0, right_direction = 0;//1为正入十字，2为斜入十字
Lose_Line cross_lose;


uint8 FindLeftInflection(){
    uint8 left_cross_inflection = 0;
    float kc1 = 0, kc2 = 0;//转折点前后斜率
    for (int i = 109; i > valid_row + 5; --i){
        if (left_points[i] - left_points[i+2]>=0&&left_points[i - 2] - left_points[i] < 0 && left_points[i]>10){
            left_cross_inflection = i;
            kc1 = 10.f*(left_points[left_cross_inflection] - left_points[left_cross_inflection + 10]) / 5.f;
            kc2 = 10.f*(left_points[left_cross_inflection - 5] - left_points[left_cross_inflection]) / 5.f;
            if (kc1 >= 0 && kc2 < 0){
                return left_cross_inflection;
            }
        }
    }
    return 0;
}

uint8 FindRightInflection(){
    uint8 right_cross_inflection = 0;
    float kc1 = 0, kc2 = 0;//转折点前后斜率
    for (int i = 109; i > valid_row + 5; --i){
        if (right_points[i] - right_points[i+2]<=0 && right_points[i - 2] - right_points[i] > 0 && right_points[i] < 177){
            right_cross_inflection = i;
            kc1 = 10.f*(right_points[right_cross_inflection] - right_points[right_cross_inflection + 10]) / 5.f;
            kc2 = 10.f*(right_points[right_cross_inflection - 5] - right_points[right_cross_inflection]) / 5.f;
            if (kc1 < 0 && kc2 > 0){
                return right_cross_inflection;
            }
        }
    }
    return 0;
}


//车头向左
uint8 is_LeftCross(){
    left_cross_inflection = 0; right_cross_inflection = 0;
    left_c_jump = 0; right_c_jump = 0;
    cross_lose.left_lose_end = 119; cross_lose.right_lose_end = 119;
    width_flag = 0;

    while (right_points[cross_lose.right_lose_end--] > 182);
    while (left_points[cross_lose.left_lose_end--] < 5);
    if (FindLeftInflection() != 0){
        left_cross_inflection = FindLeftInflection();
        if (!(is_Straight_Edge(left_cross_inflection + 15, left_cross_inflection + 5, Left_Edge))) return 0;
        if (!(left_cross_inflection >= cross_lose.right_lose_end&&left_cross_inflection<img_width * 2 / 3)) return 0;
        uint8 min_left = left_points[left_cross_inflection];
        uint8 min_right = right_points[left_cross_inflection];
        for (int i = left_cross_inflection; i > valid_row + 5; --i){
            min_left = min_left < left_points[i] ? min_left : left_points[i];
            min_right = min_right < right_points[i] ? min_right : right_points[i];
            if (left_c_jump == 0 && left_points[i] - left_points[i - 2]<-10 && left_points[i - 2] - left_points[i - 3]>-3)
                left_c_jump = i - 2;
            if (left_c_jump > 0) break;
        }
        if (!(min_right > img_width * 3 / 4)) return 0;
        if (left_points[left_cross_inflection] < img_width * 1 / 4){
            if (!(min_left<10)) return 0;
        }
        else{
            if (!(min_left - left_points[left_cross_inflection] <  -img_width * 1 / 4)) return 0;
        }
        if (left_c_jump > 0 && left_c_jump>valid_row + 10){
            left_direction = 1;
            return 1;
        }
        else {
            left_direction = 2;
            return 1;
        }
    }
    return 0;
}


//车头向右
uint8 is_RightCross(){
    left_cross_inflection = 0; right_cross_inflection = 0;
    left_c_jump = 0; right_c_jump = 0;
    cross_lose.left_lose_end = 119; cross_lose.right_lose_end = 119;
    width_flag = 0;
    while (left_points[cross_lose.left_lose_end--] < 5);
    while (right_points[cross_lose.right_lose_end--] > 182);
    if (FindRightInflection() != 0){
        right_cross_inflection = FindRightInflection();
        if (!(is_Straight_Edge(right_cross_inflection + 15, right_cross_inflection + 5, Right_Edge))) return 0;
        if (!(right_cross_inflection >= cross_lose.left_lose_end&&right_cross_inflection>img_width * 1 / 3)) return 0;
        uint8 max_right = right_points[right_cross_inflection];
        uint8 max_left = left_points[right_cross_inflection];
        for (int i = right_cross_inflection; i>valid_row + 5; --i){
            max_right = max_right > right_points[i] ? max_right : right_points[i];
            max_left = max_left > left_points[i] ? max_left : left_points[i];
            if (right_c_jump == 0 && right_points[i] - right_points[i - 2] > 10 && right_points[i - 2] - right_points[i - 3] < 3)
                right_c_jump = i - 2;
            if (right_c_jump > 0) break;
        }
        if (!(max_left < img_width * 1 / 4)) return 0;
        if (right_points[right_cross_inflection]>img_width * 3 / 4){
            if (!(max_right > 177)) return 0;
        }
        else{
            if (!(max_right - right_points[right_cross_inflection] > img_width * 1 / 4)) return 0;
        }
        if (right_c_jump > 0 && right_c_jump>valid_row + 10){
            right_direction = 1;
            return 1;
        }
        else {
            right_direction = 2;
            return 1;
        }
    }
    return 0;
}

//车头正中间
uint8 is_midCross(){
    left_cross_inflection = 0; right_cross_inflection = 0;
    left_c_jump = 0; right_c_jump = 0;
    cross_lose.left_lose_end = 119; cross_lose.right_lose_end = 119;
    width_flag = 0;
    while (left_points[cross_lose.left_lose_end--] < 5);
    while (right_points[cross_lose.right_lose_end--] > 182);
    if (FindLeftInflection() !=0 && FindRightInflection() !=0){
        left_cross_inflection = FindLeftInflection();
        right_cross_inflection = FindRightInflection();
        uint8 max_inflextion = left_cross_inflection > right_cross_inflection ? left_cross_inflection : right_cross_inflection;
        uint8 min_left = left_points[left_cross_inflection], max_right = right_points[right_cross_inflection];
        for (int i = max_inflextion; i > valid_row + 7; --i){//找跳变回来的点
            if (left_c_jump == 0 && left_points[i] - left_points[i - 2]<-10 && left_points[i - 2] - left_points[i - 3]>-3)
                left_c_jump = i - 2;
            if (right_c_jump == 0 && right_points[i] - right_points[i - 2] > 10 && right_points[i - 2] - right_points[i - 3] < 3)
                right_c_jump = i - 2;
            if (left_c_jump > 0 && right_c_jump > 0) break;

        }
        for (int i = max_inflextion; i > valid_row; --i){
            min_left = min_left < left_points[i] ? min_left : left_points[i];
            max_right = max_right > right_points[i] ? max_right : right_points[i];
        }
        uint8 min_inflection = left_cross_inflection < right_cross_inflection ? left_cross_inflection : right_cross_inflection;
        if (!(myabs(left_cross_inflection - right_cross_inflection) < 10)) //两个拐点
            return 0;
        if (left_points[left_cross_inflection] < img_width * 1 / 4){
            if (!(min_left<10)) return 0;
        }
        else{
            if (!(min_left - left_points[left_cross_inflection] <  - img_width * 1 / 4)) return 0;
        }
        if (right_points[right_cross_inflection]>img_width * 3 / 4){
            if (!(max_right > 177)) return 0;
        }
        else{
            if (!(max_right - right_points[right_cross_inflection] > img_width * 1 / 4)) return 0;
        }
        if (left_c_jump > 0 && right_c_jump > 0&&left_c_jump>valid_row+10&&right_c_jump>valid_row+10){
            left_direction = 1;
            right_direction = 1;
            return 1;
        }
        else{
            left_direction = 2;
            right_direction = 2;
            return 1;
        }
    }
	return 0;
}


uint8 is_Cross(){
    if (is_midCross()) {
        if (left_direction == 1){
            for (int i = left_cross_inflection + 2; i > left_c_jump - 2; --i){
                left_points[i] = left_points[left_cross_inflection + 2] +
                    (left_cross_inflection + 2 - i)*(left_points[left_c_jump - 2] - left_points[left_cross_inflection + 2]) / (float)(left_cross_inflection - left_c_jump + 4);
            }
        }
        if (right_direction == 1){
            for (int i = right_cross_inflection + 2; i > right_c_jump - 2; --i){
                right_points[i] = right_points[right_cross_inflection + 2] +
                    (right_cross_inflection + 2 - i)*(right_points[right_c_jump - 2] - right_points[right_cross_inflection + 2]) / (float)(right_cross_inflection - right_c_jump + 4);
            }
        }
        if (left_direction == 2){
            for (int i = left_cross_inflection + 3; i > valid_row; --i){
                left_points[i] = left_points[left_cross_inflection + 3] +
                    (left_cross_inflection + 3 - i)*(left_points[left_cross_inflection + 3] - left_points[left_cross_inflection + 10]) / 7.f;
            }
        }
        if (right_direction == 2){
            for (int i = right_cross_inflection + 3; i > valid_row; --i){
                right_points[i] = right_points[right_cross_inflection + 3] +
                    (right_cross_inflection + 3 - i)*(right_points[right_cross_inflection + 3] - right_points[right_cross_inflection + 10]) / 7.f;
            }
        }
        for (int i = 119; i > valid_row; --i){
            mid_points[i] = (left_points[i] + right_points[i]) / 2;
        }
        return 1;
    }
    if (is_LeftCross() == 1){
        if (left_direction == 1){
            for (int i = left_cross_inflection + 2; i > left_c_jump - 2; --i){
                left_points[i] = left_points[left_cross_inflection + 2] +
                    (left_cross_inflection + 2 - i)*(left_points[left_c_jump - 2] - left_points[left_cross_inflection + 2]) / (float)(left_cross_inflection - left_c_jump + 4);
            }
        }
        if (left_direction == 2){
            for (int i = left_cross_inflection + 3; i > valid_row; --i){
                left_points[i] = left_points[left_cross_inflection + 3] +
                    (left_cross_inflection + 3 - i)*(left_points[left_cross_inflection + 3] - left_points[left_cross_inflection + 10]) / 7.f;
            }
        }
        for (int i = 119; i > valid_row; --i){
            mid_points[i] = limit(left_points[i] + track_width[i] / 2,0,187);;
        }
        return 1;
    }
    if (is_RightCross() == 1){
        if (right_direction == 1){
            for (int i = right_cross_inflection + 2; i > right_c_jump - 2; --i){
                right_points[i] = right_points[right_cross_inflection + 2] +
                    (right_cross_inflection + 2 - i)*(right_points[right_c_jump - 2] - right_points[right_cross_inflection + 2]) / (float)(right_cross_inflection - right_c_jump + 4);
            }
        }
        if (right_direction == 2){
            for (int i = right_cross_inflection + 3; i > valid_row; --i){
                right_points[i] = right_points[right_cross_inflection + 3] +
                    (right_cross_inflection + 3 - i)*(right_points[right_cross_inflection + 3] - right_points[right_cross_inflection + 10]) / 7.f;
            }
        }
        for (int i = 119; i > valid_row; --i){
            mid_points[i] = limit(right_points[i] - track_width[i] / 2,0,187);
        }
        return 1;
    }
    return 0;
}

//***************************************************十字**************************************************



//***************************************************坡道**************************************************
float ramp_k1, ramp_k2; uint8 right_is_ramp = 0;
uint8 is_Ramp()
{
    if(Ramp_Judge_Row-20<valid_row)
    {
        return 0;
    }
    else
    {
        for (int i = Ramp_Judge_Row; i > Ramp_Judge_Row - 20; i--)
        {
            if (right_points[i] - left_points[i] < track_width[i] + 15)
            {
                return 0;
            }
        }
    }
    if(is_Straight_Edge(Ramp_Judge_Row,Ramp_Judge_Row-15,Right_Edge)
       &&is_Straight_Edge(Ramp_Judge_Row,Ramp_Judge_Row-15,Left_Edge))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
//***************************************************坡道**************************************************


//***************************************************终点线**************************************************


uint64 last_end_time=0;uint8 end_times=0;
uint8 is_End()
{
    OLED_Print_Num1(64,1,end_times);
    if(IS_END(img_width/2,End_Judge_Row))
    {
        if(last_end_time==0)
        {
            end_times++;
            last_end_time=time;
        }
        else if(time-last_end_time>100)
        {
            end_times++;
            last_end_time=time;
        }
    }
	if(end_times>=2)
		return 1;
	else
		return 0;
}
//***************************************************终点线**************************************************


//*************************************************元素判断****************************************************

