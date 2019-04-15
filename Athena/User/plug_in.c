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

	//TIM2_Init(9999, 8);      
}


/********************************************
* 函数名: Stamper_Ctr
* 函数功能: 电控主模块
* 输入: 无
* 输出: 无
*********************************************/
void Stamper_Ctr(void)
{
	while(1)
	{
		if(FLAG == Start)
		{
			printf("Start\r\n");
            Enable_TIMX_OCXInit(TIM2, TIM_OC2Init);
			Motor_Move(3200, 6400, 800);
			FLAG = 0;
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


