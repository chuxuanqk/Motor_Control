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


void Motor_Reset(void);							 								// 电机初始化
void Paper_Move_In(void);		 												// 进纸
void Paper_Move_Out(void);		 											// 出纸
void Cover_Seal(int32_t X_mm, int32_t Z_mm);			 	// 盖章
void Exchange_Seal(int32_t Z_mm, int32_t Seal_id);  // 换章
void Reset_Seal(int32_t Z_mm, int32_t Seal_id);

extern float FRE_MAX;
extern float FRE_MIN;

typedef struct Command{
    int32_t X_MM;               
    int32_t Y_1_MM;      
    int32_t Y_2_MM;       
    int16_t SEAL_ID;                   
}Command; 


static int32_t SEAL_ID[4] = {0, 200, 400, 600};


#endif
