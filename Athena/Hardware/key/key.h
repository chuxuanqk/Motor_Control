#ifndef __KEY_H
#define __KEY_H

#include "sys.h"


#define S1 PEin(4)         
#define S2 PEin(3)         
//#define S3 PEin(2) 
#define ZU PEin(2)						// Z轴上限开关
//#define K1 PEin(5)          // 光电开关Y1, PE5无法使用,可能是引脚资源的问题
#define K1 PEin(9)          	// 光电开关Y1
#define K2 PEin(6)          	// 光电开关Y2
#define LX PEin(7)          	// X轴限位开关
#define TP PEin(8)        		// 转盘限位开关
//#define ZD PEin(9)          // Z轴下限开关


void KEY_Init(void);
void NVIC2_Init(void);
void NVIC3_Init(void);
void NVIC4_Init(void);
void NVIC5_Init(void);


void EXTIX_Init(void);
void EXTIX_Config(void);

void EXTIX_DISABLE(uint32_t EL);
void EXTIX_ENABLE(uint32_t EL);

void stop(void);


enum K_State{
    Start=1,    // 启动
    Reset,      // 复位
    Paper_skip, // 走纸
    Run_Stamp,  // 盖章
    PTE1,       // 光电开关1
		PTE2,       // 光电开关2 
    X_Limit,    // X轴限位开关
    Z_UP,       // Z上限位
    Z_DOWN,     // Z下限位
		S2_KEY,			// S2按键
		TP_Limit,   // TP限位
};

extern uint32_t STEP;
extern uint8_t FLAG;           // 状态标志位

#endif
