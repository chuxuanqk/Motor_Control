#ifndef __PLUG_IN_H
#define __PLUG_IN_H

#include "sys.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "Motor_Setup.h"


int Power(int base, int32_t exponent);
int32_t Usart_Size(u8 *buf);
int32_t Recv_Int(void);

void Stamper_Init(void);     // 设备初始化设置     
void Stamper_Ctr(void);			 // 主控制
void SimpleTest(void);       // 测试模块

void Servo_Config(void);     // 舵机抓手设置

void Motor_Reset(void);			 // 电机初始化
void Paper_Move_1(void);		 // 走纸第一阶段
void Paper_Move_2(void);		 // 走纸第二阶段
void Cover_Seal(void);			 // 盖章
void Exchange_Seal(void);    // 换章



extern float FRE_MAX;
extern float FRE_MIN;

typedef struct Command{
    int32_t X_STEP;         
    int32_t Y_STEP;        
    int32_t X_1_STEP;      
    int32_t Y_1_STEP;       
    int16_t Z_STEP;         
    uint8_t state;          
    uint8_t flag;           
}Command; 




#endif
