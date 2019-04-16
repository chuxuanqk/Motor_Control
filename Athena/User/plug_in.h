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

void Stamper_Init(void);          
void Stamper_Ctr(void);
void SimpleTest(void);       // 测试模块

void Servo_Config(void);

void Motor_Reset(void);
void Paper_Move_1(void);
void Paper_Move_2(void);
void Stamped(void);

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
