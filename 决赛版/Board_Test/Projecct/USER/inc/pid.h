

typedef struct
{
  int Kp;//����ϵ��
  int Kd;//΢��ϵ��
  int Ki;//����ϵ��
  float Error;//��ǰ���
  float Error1;//��һ�����
  float Error2;//���ϴ����
  float Error_Sum;//����ۻ�
}PID_TypeDef;

int PID_Increment(int Target,float Actual,PID_TypeDef* PID_Struct);
int PID_Location(int Target,float Actual,PID_TypeDef* PID_Struct);