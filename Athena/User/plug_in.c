#include "plug_in.h"


Command cmd;
float FRE_MAX = 12800.0;
float FRE_MIN = 400.0;

int32_t num = 800;
float m_fre_max = 5600;
float m_fre_min = 400;
float m_flexible = 4.0;


int8_t Servo_flag = 0;
uint16_t arr = 19999;
uint16_t psc = 71;

int Power(int base, int32_t exponent)
{
    int ret=1.0;
    int i = 0;
    for(; i<=exponent; ++i)
    {
        ret*=base;
    }

    return ret;
}


/**********************************
 * 
 *      计算usart发送的信息size
 * 
 * *******************************/
int32_t Usart_Size(u8 *buf)
{
    int count=0;

    while(*buf != 0x0d)
    {
        ++buf;
        ++count;
    }
    return count;
}


/****************************************
 * 
 *       串口接收值转为INT
 * 
 **************************************/
int32_t Recv_Int(void)
{
    int32_t Value=0;
    uint16_t i=0;
    int32_t temp;

    if(USART_RX_STA&0x8000)
    {
        for(; i<(USART_RX_STA&0x3FFF); ++i)
        {   
			printf("Buf:%d\r\n", USART_RX_BUF[i]);
            temp = (USART_RX_BUF[i] - 48)*Power(10, i);
            Value += temp;
        }
		USART_RX_STA=0;
    }
    
    return Value;
}

/****************************************
 * 
 *       解析串口传输的命令
 * 
 **************************************/
int16_t Decode(void)
{
    //uint16_t i=0;

    if(USART_RX_STA&0x8000)
    {
        if((USART_RX_STA&0x3FFF) != 12){
            return -1;
        }else{
            cmd.X_STEP =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
            cmd.Y_STEP =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
            cmd.X_STEP =  (USART_RX_BUF[6]-48)*100+(USART_RX_BUF[7]-48)*10+(USART_RX_BUF[8]-48);
            cmd.Y_STEP =  (USART_RX_BUF[9]-48)*100+(USART_RX_BUF[10]-48)*10+(USART_RX_BUF[11]-48);
            cmd.Z_STEP =  (USART_RX_BUF[12]-48)*100+(USART_RX_BUF[13]-48)*10+(USART_RX_BUF[14]-48);
            cmd.state  =  USART_RX_BUF[15];
            cmd.flag   =  USART_RX_BUF[16];
        }
        USART_RX_STA=0;
    }
    return 0;
}



/*************************************
 * 函数名:Stamper_Init
 * 函数功能:印章机配置初始化
 * 输入:无
 * 输出:无
 * ***********************************/
void Stamper_Init(void)
{
		delay_init();
		NVIC_Configuration();

		uart_init(9600);
		LED_Init();
		EXTIX_Config();
		DIR_ENA_GPIO_Config();

		TIM2_Init(9999, 8);
		TIM3_Init(9999, 8);
		TIM4_Init(9999, 8);	
}


/********************************************
* 函数名: Stamper_Ctr
* 函数功能: 电控主模块
* 输入: 无
* 输出: 无
*********************************************/
void SimpleTest(void)
{
		if(FLAG == Start)
		{
				FLAG = 0;
				printf("Start\r\n");
			
				// X轴运动
				Motor_Move(8000, 6400, 800, X_MOTOR);
				while(Status != 0);
				printf("finish:X\r\n");
//				
//				// Z轴运动
//				Enable_TIMX_OCXInit(TIM2, TIM_OC3Init);
//				Motor_Move(-3200, 6400, 800, Z_MOTOR);
//				while(Status != 0);
//				printf("finish:Z\r\n");
//				Disable_TIMX_OCXInit(TIM2, TIM_OC3Init); 
//			
//				// TP轴运动
//				Enable_TIMX_OCXInit(TIM2, TIM_OC4Init);
//				Motor_Move(800, 6400, 800, TP_MOTOR);
//				while(Status != 0);
//				printf("finish:TP\r\n");
//				Disable_TIMX_OCXInit(TIM2, TIM_OC4Init); 
				
				// Y1和Y2轴运动
//				Motor_Y1_Init(9999, CCW);
//				Motor_Y2_Init(4999, CCW);
//				while(FLAG == 0);
//				printf("finish:%d\r\n", FLAG);
				
		}		
}	


/********************************************
* 函数名: 		Motor_Reset
* 函数功能: 	X/Z/抓手复位
* 输入: 			无
* 输出: 			无
*********************************************/
void Motor_Reset(void)
{
		int32_t x_rad = 800*4;
		int32_t z_rad = 800*3;
		int16_t x_maxfre = 800*5;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
	
		// Z轴复位动作
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);

		EXTIX_DISABLE(EXTI9_5_IRQn);
		// X轴复位
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
		delay_ms(300);
		Motor_Move(-(8*x_rad), x_maxfre, x_minfre, X_MOTOR);
		EXTIX_ENABLE(EXTI9_5_IRQn);
		while(Status != 0);
}	

	
/********************************************
* 函数名: 		Paper_Move_1
* 函数功能: 	走纸第一阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_1(void)
{
		uint16_t Y1_arr = 9999;
		uint16_t Y2_arr = 4999;
	
		Motor_Y1_Init(Y1_arr, CCW);
		Motor_Y2_Init(Y2_arr, CCW);
		while(1)
		{
			if(FLAG == PTE2)
			{
				FLAG = 0;
				break;
			}
		}
}
	
/********************************************
* 函数名: 		Paper_Move_2
* 函数功能: 	走纸第二阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_2(void)
{
		FLAG = 0;
		uint16_t Y2_arr = 4999;

		Motor_Y2_Init(Y2_arr, CCW);
		while(K2 == 0);
		Disable_TIMX_OCXInit(Y2_TIMx, TIM_OC2Init);
}

/********************************************
* 函数名: 		Stamped
* 函数功能: 	盖章模块
* 输入: 			无
* 输出: 			无
*********************************************/
void Stamped(void)
{
		int32_t x_rad = 800*45;
		int32_t z_rad = 800*10;
		int16_t x_maxfre = 800*14;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		
	
		// X轴向右运动
		delay_ms(1000);
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
	
		delay_ms(2000);
		// Z轴向下运动
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
		delay_s(2);
		// Z轴向上运动
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
	
		// X轴向左运动
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
}


/********************************************
* 函数名: 		Stamper_Ctr
* 函数功能: 	电控主模块
* 输入: 		无
* 输出: 		无
*********************************************/
void Stamper_Ctr(void)
{
	delay_s(3);
	Motor_Reset();
	while(1)
	{
		//SimpleTest();
		if(FLAG == Start)
		{
				FLAG = 0;
				Paper_Move_1();
				EXTIX_DISABLE(EXTI9_5_IRQn);
				Stamped();
				EXTIX_ENABLE(EXTI9_5_IRQn);
				Paper_Move_2();
		}
	}
}




/********************************************
* 函数名: Servo_Config
* 函数功能: 舵机控制
* 输入: 无
* 输出: 无
*********************************************/

/*
void Servo_Config(void)
{

	TIM2_Init(arr, psc);
	TIM_SetAutoreload(TIM2, arr);
	TIM_SetCompare2(TIM2, arr/40);
	TIM_Cmd(TIM2, ENABLE);
	
	while(1)
	{
		if(FLAG == Start)
		{
			Servo_flag = 1;
			FLAG = 0;
		}else if(FLAG == 10)
		{
			Disable_TIMX_OCXInit(TIM2, TIM_OC2Init);
			FLAG = 0;
		}else if(FLAG == 11)
		{
			Enable_TIMX_OCXInit(TIM2, TIM_OC2Init);
			FLAG = 0;
		}
	}
}


void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(X_TIMx, TIM_IT_Update) != RESET)
    {
		TIM_ClearITPendingBit(X_TIMx, TIM_IT_Update);
		if(Servo_flag == 0)
		{
			TIM_SetAutoreload(TIM2, arr);
			TIM_SetCompare2(TIM2, (arr/40)*3);
			TIM_SetCompare3(TIM2, (arr/40)*3);
		}else if(Servo_flag == 1){
			TIM_SetAutoreload(TIM2, arr);
			TIM_SetCompare2(TIM2, (arr/40));
			TIM_SetCompare3(TIM2, (arr/40));
		}
		

    }
}
*/


