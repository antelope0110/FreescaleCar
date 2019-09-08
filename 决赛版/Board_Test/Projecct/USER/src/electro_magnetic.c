#include "electro_magnetic.h"
#include "MK60_adc.h"
#include "hardware_driver_layer.h"
#include "SEEKFREE_OLED.h"

float mySqrt(float x)
{
    float a = x;
    unsigned int i = *(unsigned int *)&x;
    i = (i + 0x3f76cf62) >> 1;
    x = *(float *)&i;
    x = (x + a / x) * 0.5;
    return x;
}

void ElectroMagnetic_Init()
{
    adc_init(ADC0_SE8);
    adc_init(ADC0_SE9);
    adc_init(ADC0_SE12);
    adc_init(ADC0_SE13);
}

int right_value1,left_value1,right_value2,left_value2;
float error=0;
float error1=0;
float pre_error=0;
float pre_error1=0;
float ElectroMagnetic_GetValue()
{

    right_value1=0,left_value1=0;
    right_value2=0,left_value2=0;
    for(int i=0;i<10;i++)
    {
        left_value1+=adc_once(ADC0_SE12, ADC_10bit);
        right_value1+=adc_once(ADC0_SE13, ADC_10bit);
        left_value2+=adc_once(ADC0_SE8, ADC_10bit);
        right_value2+=adc_once(ADC0_SE9, ADC_10bit);
    }
    right_value1=right_value1/10;
    left_value1=left_value1/10;
    right_value2=right_value2/10;
    left_value2=left_value2/10;

    if(right_value1+left_value1>10){
    	error=400*(right_value1-left_value1)/(right_value1+left_value1);
        error=error*(myabs(error))/400;
    }
    pre_error=0.20*pre_error+0.20*error;
    if((right_value1+left_value1)>30)
        error1=400*(right_value2-left_value2)/(right_value1+left_value1-right_value2+left_value2);
    else
        error1=400*(right_value2-left_value2)/myabs(50-right_value2+left_value2);
    pre_error1=0.3*pre_error1+0.25*error1;

    OLED_Print_Num1(0, 2, pre_error+pre_error1);
    OLED_Print_Num1(0, 5, pre_error);OLED_Print_Num1(64, 5, pre_error1);
    OLED_Print_Num1(0,6,left_value1);OLED_Print_Num1(64,6,right_value1);
    OLED_Print_Num1(0,7,left_value2);OLED_Print_Num1(64,7,right_value2);
    return pre_error+pre_error1;
}