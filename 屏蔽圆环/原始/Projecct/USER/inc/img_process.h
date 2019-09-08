#include "SEEKFREE_MT9V032.h"
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ͼ��δ���������任����ͼ�����ϲ�����Ϊ0�����²�Ϊimg_height-1
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

#define img_width       COL
#define img_height      ROW

extern const uint8 track_width[img_height];

//*******************************************************�궨*******************************************
void Camera_Calibration();
void Get_Track_Width();
//*******************************************************�궨*******************************************

//*******************************************************���Ʊ�ʶ*******************************************
void Show_Line();
//*******************************************************���Ʊ�ʶ*******************************************

//***************************************************�ұ���****************************************************
extern uint8 left_points[img_height];		//�����
extern uint8 right_points[img_height];		//�ұ���
extern uint8 mid_points[img_height];		//�����������ұ�����������������𣨾����˲��������������䣩
extern uint8 valid_row;						//��Ч���߷�Χ��0:valid_row)
#define Valid_Row_Width         6			//��Ч���ж�������������ȣ�
#define Gray_Scale_Difference   13			//�߽��жϻҶȲ�ֵ
#define Light_Gray_Scale		80			//ȫ�������ػҶ�ֵ
#define Dark_Gray_Scale			170			//ȫ�ְ����ػҶ�ֵ
uint8 Search_Edge(uint8 Row_Index, uint8 Start, uint8 End, uint8 Edge_Gray_Mutation);		//��λ�Ҷ�����λ��
void Get_Edge_Related();                                        //��ȡ���ұ߽缰����
uint8 is_Track(uint8 Row_Index);								//�жϱ��߷�Χ���Ƿ�������
//***************************************************�ұ���****************************************************

//***************************************************����Ԥ����****************************************************
typedef enum
{
	Right_Edge,
	Left_Edge
}EdgeSide;

void Smooth_Edge();
uint8 is_Straight_Edge(uint8 start_index, uint8 end_index, EdgeSide side);
//***************************************************����Ԥ����****************************************************

//*************************************************Ԫ���ж�****************************************************
typedef enum
{
	CURVE,              //���,ȱʡ����
	CROSS,              //ʮ��
	END,                //�յ���
	BREAK,              //��·
	RAMP,               //�µ�
	BARRIER,            //���
	ROUNDABOUT,         //����
}TRACK_ELEMENT;

extern TRACK_ELEMENT current_element;	//Ŀǰ����Ԫ�ر�ʶ

extern uint64 time;                             //Ԫ�ؼ����ʱ��λ100ms
void Refresh_Current_Element();			//ˢ�µ�ǰ����Ԫ��

uint8 is_Cross();                       //�жϡ�ʮ�֡�

#define End_Judge_Row			100      //�յ������ж���
#define End_Gray_Difference		30		//�յ�������ҶȲ�
uint8 is_End();                         //�жϡ��յ��ߡ�

#define Break_Valid_Row			90		//��·��Ч����Сֵ��ͼ�����ϲ�����Ϊ�㣬����Ϊ���ֵ��
uint8 is_Break();                       //�жϡ���·��

extern uint64 ramp_time;
#define Ramp_Judge_Row			50		//�µ��ж���
uint8 is_Ramp();                        //�жϡ��µ���

#define Roundabout_Mutation_Pixel	30	//Բ���ж�������������
uint8 is_Roundabout();					//�жϡ��󻷵����롮��Բ����
//*************************************************Ԫ���ж�****************************************************

