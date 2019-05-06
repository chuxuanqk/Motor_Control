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
* 函数名:		Usart_Size
* 函数功能:	计算usart的信息size
* 输入:			*buf:uart的缓存区
*	输出:			count:计算值
**********************************/
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
* 函数名:		Recv_Int
*	函数功能: 	串口接收值转为INT
* 输入:			无
* 输出:			Value:
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
						//printf("Buf:%d\r\n", USART_RX_BUF[i]);
            temp = (USART_RX_BUF[i] - 48)*Power(10, i);
            Value += temp;
        }
				if(12 == i)
				{
						printf("Recv finish\r\n");
				}
				USART_RX_STA=0;
				i=0;
    }
    
    return Value;
}

/****************************************
* 函数名:		Decode
* 函数功能:	解析串口传输的命令
* 输入:			无
* 输出:			无
 **************************************/
int16_t Decode(void)
{
    //int16_t i=0;

    if(USART_RX_STA&0x8000)
    {
				//LED = ~LED;
        if((USART_RX_STA&0x3FFF) != 10){
            return -1;
        }else{
						//printf("usart:%d\r\n", (USART_RX_STA&0x3FFF));
						cmd.X_MM =  (USART_RX_BUF[0]-48)*100+(USART_RX_BUF[1]-48)*10+(USART_RX_BUF[2]-48);
						cmd.Y_1_MM =  (USART_RX_BUF[3]-48)*100+(USART_RX_BUF[4]-48)*10+(USART_RX_BUF[5]-48);
						cmd.Y_2_MM=  (USART_RX_BUF[6]-48)*100+(USART_RX_BUF[7]-48)*10+(USART_RX_BUF[8]-48);
						//cmd.Y_2_MM = cmd.Y_2_MM*1.5;
						cmd.SEAL_ID =  (USART_RX_BUF[9]-48);
						
						printf("X_mm:%d\r\n", cmd.X_MM);
						printf("Y_1_mm:%d\r\n", cmd.Y_1_MM);
						printf("Y_2_mm:%d\r\n", cmd.Y_2_MM);
						printf("Z_mm:%d\r\n", cmd.SEAL_ID);
          }

				printf("usart:%d\r\n", (USART_RX_STA&0x3FFF));

					
        USART_RX_STA=0;
    }
    return 0;
}


/*************************************
* 函数名:		Stamper_Init
* 函数功能:	印章机配置初始化
* 输入:			无
* 输出:			无
* ***********************************/
void Stamper_Init(void)
{
		delay_init();
		NVIC_Configuration();

		uart_init(9600);
		LED_Init();
		EXTIX_Config();
		DIR_ENA_GPIO_Config();

		TIM2_Init(TIM_PRELOAD, TIM_PRESCALER);
		TIM3_Init(TIM_PRELOAD, TIM_PRESCALER);
		//TIM3_Init(SERVO_PRELOAD, SERVO_PRESCALER);
		TIM4_Init(TIM_PRELOAD, TIM_PRESCALER);	
}


/********************************************
* 函数名: 		SimpleTest
* 函数功能: 	电控测试模块
* 输入: 		无
* 输出: 		无
*********************************************/
void SimpleTest(void)
{	
		int16_t Servo_cc=5;
		Servo_Config();
		while(1)
		{
			if(FLAG == Start)
			{
					FLAG = 0;
					printf("Start\r\n");
					Servo_Open();
			}		
			
			if(USART_RX_STA&0x8000)
			{
					Servo_cc =  (USART_RX_BUF[0]-48)*10+(USART_RX_BUF[1]-48);
					Set_TIM3_CC(Servo_cc);
					printf("CC:%d\r\n", Servo_cc);
					USART_RX_STA=0;
			}
		}
}	


/********************************************
* 函数名: 			Motor_Reset
* 函数功能: 		X/Z/抓手复位
* 输入: 			无
* 输出: 			无
*********************************************/
void Motor_Reset(void)
{
		int32_t x_rad = 800*8;
		int32_t z_rad = 800*3;
		int32_t tp_rad = 200;
		int16_t x_maxfre = 800*5;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;
	
		FLAG = 0;
		// 转盘复位
		Motor_Move(tp_rad, tp_maxfre, tp_minfre, TP_MOTOR);
		while(Status != 0);

		Motor_Move(-(tp_rad*4), tp_maxfre, tp_minfre, TP_MOTOR);
		while(Status != 0);
			
		// X轴复位
		EXTIX_DISABLE(EXTI9_5_IRQn);
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);       	// X向左运动
		while(Status != 0);
	
		// Z轴复位动作
		Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);				
		while(Status != 0);
		Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
		while(Status != 0);
	
		EXTIX_ENABLE(EXTI9_5_IRQn);
		Motor_Move((8*x_rad), x_maxfre, x_minfre, X_MOTOR);     // X向右运动
		while(Status != 0);

	
		// 舵机抓手复位
		Servo_Config();
		delay_s(1);
		TIM_Cmd(Servo_TIMx, DISABLE);
		Servo_TIM_DisableOC;
}	

	
/********************************************
* 函数名: 			Paper_Move_In
* 函数功能: 		走纸第一阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_In(void)
{
		uint16_t Y1_arr = 39999;    	 // 9999/   (8*10^6)/2*10^4 = 200    
		uint16_t Y2_arr = 19999;      	 // 19999/   (8*10^6)/10^4 = 400	Y2周长:78.5mm, (78.5/2)=39.25mm/s		
		
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);
	
		while(K1==1);

		printf("In FLAG:%d\r\n", FLAG);
		if(FLAG == PTE1)
		{
				FLAG = 0;
				delay_s(2);
				printf("Y1停止\r\n");
				Y1_TIM_DisableOC;
				EXTIX_ENABLE(EXTI9_5_IRQn);
		}
		while(K2==1);
		if(K2 == 0) 
		{
			delay_ms(300);
			Y2_TIM_DisableOC;
		}
}


/********************************************
* 函数名: 			Paper_Move_In2
* 函数功能: 		走纸第一阶段Y1和Y2走纸距离控制, Y1:Y2 = 1:1.68
* 输入: 			Y1_speed  Y1轴速度  			(mm/s)
*             Y2_speed  Y2轴速度  			(mm/s)
*							y_coord   Y轴盖章标点		(mm)
* 输出: 			无
*********************************************/
void Paper_Move_In2(uint16_t Y1_speed, uint16_t Y2_speed,uint16_t Y1_coord, uint16_t Y2_coord)
{
		uint16_t Y1_arr = ARR_Y1;    	 									//  9999/   (8*10^6)/2*10^4 = 200    
		uint16_t Y2_arr = ARR_Y2;      	 								// 19999/  (8*10^6)/10^4 = 400	Y2周长:78.5mm, (78.5/2)=39.25mm/s		
		uint16_t Y1_delay_time = ((Y1_coord)/Y1_speed);								// 延时 Y1_delay_time ms
		uint16_t Y2_delay_time = ((Y2_coord*500)/Y2_speed);   							// 延时 Y2_delay_time ms

		EXTIX_DISABLE(EXTI9_5_IRQn);
		Motor_Y1_Init(Y1_arr, (Y1_arr/2), CCW);
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);

		while(K1==1);

		printf("delay_time:%d\r\n", Y1_delay_time);
		delay_s(Y1_delay_time);
		delay_ms(100);
		Y1_TIM_DisableOC;
		printf("Y1停止\r\n");
		
		printf("delay_time:%d\r\n", Y2_delay_time);
		delay_ms(Y2_delay_time);
		delay_ms(Y2_delay_time);
	
		Y2_TIM_DisableOC;
		FLAG = 0;
		EXTIX_ENABLE(EXTI9_5_IRQn);
}
	

/********************************************
* 函数名: 			Paper_Move_Out
* 函数功能: 		走纸第二阶段
* 输入: 			无	
* 输出: 			无
*********************************************/
void Paper_Move_Out(void)
{
		uint16_t Y2_arr = 4999;
		FLAG = 0;
		Motor_Y2_Init(Y2_arr, (Y2_arr/2), CCW);

		while(K2 == 0);
		delay_s(2);
		Y2_TIM_DisableOC;
}


/********************************************
* 函数名: 			Cover_Seal
* 函数功能: 		盖章模块
* 输入: 			X_mm:X轴坐标, Z_mm:Z轴坐标
* 输出: 			无
*********************************************/
void Cover_Seal(int32_t X_mm, int32_t Z_mm)
{
//		int32_t x_rad = 800*45;      // 180mm
//		int32_t z_rad = 800*10;      // 40mm
		int32_t x_rad = MM2Step(X_mm);
		int32_t z_rad = MM2Step(Z_mm);
	
		// 电机速度参数
		int16_t x_maxfre = 800*10;
		int16_t x_minfre = 800;
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		
		// X轴向右运动
		delay_ms(1000);
		Motor_Move(-x_rad, x_maxfre, x_minfre, X_MOTOR);
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
		Motor_Move(x_rad, x_maxfre, x_minfre, X_MOTOR);
		while(Status != 0);
}


/***********************************************
* 函数名: 		Exchange_Seal
* 函数功能:	更换印章
* 输入:			Z_mm:Z轴坐标(下降的高度)
						Seal_id:印章id
* 输出:			无
************************************************/
void Exchange_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// 电机速度参数
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;

		// 更换印章
		if(Seal_id != 0)
		{
				Motor_Move(Seal_id, tp_maxfre, tp_minfre, TP_MOTOR);
				while(Status != 0);
			
				Servo_Open();
			
				printf("向下运动\r\n");
				// Z轴向下运动
				Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);

				Servo_Close();
			
				printf("向上运动\r\n");
				// Z轴向上运动
				Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);	
		}else{
				Servo_Open();
			
				printf("向下运动\r\n");
				// Z轴向下运动
				Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);

				Servo_Close();
			
				printf("向上运动\r\n");
				// Z轴向上运动
				Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);	
		}
}


/***********************************************
* 函数名: 		Reset_Seal
* 函数功能:	复位印章
* 输入:			Z_mm:Z轴坐标(下降的高度)
						Seal_id:印章id
* 输出:			无
************************************************/
void Reset_Seal(int32_t Z_mm, int32_t Seal_id)
{
		int32_t z_rad = MM2Step(Z_mm);
	
		// 电机速度参数
		int16_t z_maxfre = 800*4;
		int16_t z_minfre = 800;
		int16_t tp_maxfre = 800;
		int16_t tp_minfre = 400;
	
		if(Seal_id != 0)
		{
				// Z轴向下运动
				Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);

				Servo_Open();
				
				// Z轴向上运动
				Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);	
			
				Servo_Close();
				
				// 复位印章位置
				Motor_Move(-Seal_id, tp_maxfre, tp_minfre, TP_MOTOR);
				while(Status != 0);
		}else{
				// Z轴向下运动
				Motor_Move(-z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);

				Servo_Open();
				
				// Z轴向上运动
				Motor_Move(z_rad, z_maxfre, z_minfre, Z_MOTOR);
				while(Status != 0);	
			
				Servo_Close();
		}
}	



/********************************************
* 函数名: 		Stamper_Ctr
* 函数功能: 	电控主模块
* 输入: 		无
* 输出: 		无
*********************************************/
void Stamper_Ctr(void)
{	
		int16_t ret = 0;									
		int32_t z_mm = 20;                  // 设置取印章的行程
		int32_t z_down_mm = 40;							// 盖章z轴行程
		int32_t seal_id = SEAL_ID[2];

		// 1.电机复位初始化
		//Motor_Reset(); 
		while(1)
		{
				if(USART_RX_STA&0x8000)
				{
						ret = Decode();
						if(ret !=-1)
						{
								FLAG = 0;
					
								// 1.1 进纸
								printf("usrat start\r\n");
								//Paper_Move_In();
								Paper_Move_In2(Y1_SPEED, Y2_SPEED, cmd.Y_1_MM, cmd.Y_2_MM);
							
								EXTIX_DISABLE(EXTI9_5_IRQn);
							
								// 1.2 选取印章
								Exchange_Seal(z_mm, SEAL_ID[cmd.SEAL_ID]);	
								// 1.3 设置盖章坐标
								Cover_Seal(cmd.X_MM, z_down_mm);   
								// 1.4 印章复位
								Reset_Seal(z_mm, SEAL_ID[cmd.SEAL_ID]);
								EXTIX_ENABLE(EXTI9_5_IRQn);
							
								// 1.5 出纸
								Paper_Move_Out();
							
								FLAG = 0;
								printf("finish\r\n");
						}
				}
			
				// 1.开启盖章
				if(FLAG == Start)
				{
						FLAG = 0;
					
						// 1.1 进纸
						Paper_Move_In();
						EXTIX_DISABLE(EXTI9_5_IRQn);
					
						// 1.2 选取印章
						Exchange_Seal(z_mm, seal_id);	
						// 1.3 设置盖章坐标
						Cover_Seal(190, z_down_mm);   
						// 1.4 印章复位
						Reset_Seal(z_mm, seal_id);
						EXTIX_ENABLE(EXTI9_5_IRQn);
					
						// 1.5 出纸
						Paper_Move_Out();
						FLAG = 0;
						printf("finish\r\n");
				}
				
				// 2. 启动复位(更换印章) 
				if(FLAG == S2_KEY)
				{
							Motor_Reset();
							//Paper_Move_In2(Y1_SPEED, Y2_SPEED, 60, 270);
					
//						FLAG = 0;
//					
//						EXTIX_DISABLE(EXTI9_5_IRQn);
//						// 1.2 选取印章
//						Exchange_Seal(z_mm, seal_id);	
//						// 1.4 印章复位
//						Reset_Seal(z_mm, seal_id);
//						EXTIX_ENABLE(EXTI9_5_IRQn);
//						Paper_Move_In();
//						FLAG = 0;
				}
		}
}





