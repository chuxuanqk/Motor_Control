#ifndef __KEY_H
#define __KEY_H

#include "sys.h"


#define S1 PEin(4)         
#define S2 PEin(3)         
//#define S3 PEin(2) 
#define ZU PEin(2)						// Z�����޿���
//#define K1 PEin(5)          // ��翪��Y1, PE5�޷�ʹ��,������������Դ������
#define K1 PEin(9)          	// ��翪��Y1
#define K2 PEin(6)          	// ��翪��Y2
#define LX PEin(7)          	// X����λ����
#define TP PEin(8)        		// ת����λ����
//#define ZD PEin(9)          // Z�����޿���


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
    Start=1,    // ����
    Reset,      // ��λ
    Paper_skip, // ��ֽ
    Run_Stamp,  // ����
    PTE1,       // ��翪��1
		PTE2,       // ��翪��2 
    X_Limit,    // X����λ����
    Z_UP,       // Z����λ
    Z_DOWN,     // Z����λ
		S2_KEY,			// S2����
		TP_Limit,   // TP��λ
};

extern uint32_t STEP;
extern uint8_t FLAG;           // ״̬��־λ

#endif
