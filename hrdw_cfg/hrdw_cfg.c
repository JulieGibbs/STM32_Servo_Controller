#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "./hrdw_cfg/hrdw_cfg.h"

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void DMA_Configuration(void);
void USART_Configuration(void);
void TIM_Configuration(void);

void Hardware_Configuration (void)
{
	/*!< At this stage the microcontroller clock setting is already configured,
	 * this is done through SystemInit() function which is called from startup
	 * file (startup_stm32f10x_md.c) before to branch to application main.
	 * To reconfigure the default setting of SystemInit() function, refer to
	 * system_stm32f10x.c file
	 * */
	RCC_Configuration();
	GPIO_Configuration();
	NVIC_Configuration();
	DMA_Configuration();
	USART_Configuration();
	TIM_Configuration();
}

void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA |
			RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO |
			RCC_APB2Periph_GPIOC |
			RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB1Periph_TIM2 |
			RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Green LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//Blue LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//USART3 - RX - PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3 - TX - PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Encoders:
	 * TIM2, TIM2_REMAP[1:0] = �11� (full remap), CH1 - PA15, CH2 - PB3
	 * TIM3, TIM3_REMAP[1:0] = �00� (no remap)CH1 - PA6, CH2 - PA7
	 * TIM4, TIM4_REMAP = 0, CH1 - PB6, CH2 - PB7
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*PWM Generation:
	 * TIM1, TIM1_REMAP[1:0] = �00� (no remap)
	 * CH1 - CH2 - CH3  - CH4
	 * PA8 - PA9 - PA10 - PA11
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 |
			GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_Configuration(void)
{
	/* 1 bit for pre-emption priority, 3 bits for subpriority */
	NVIC_SetPriorityGrouping(6);

	/* Configure USART3 interrupt */
	NVIC_SetPriority(USART3_IRQn, 0x00);
	NVIC_EnableIRQ(USART3_IRQn);
}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* USART3_Rx_DMA_Channel (triggered by USART3 Rx event) Config */
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = 0x40004804;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBufferCirc;//0x40012C24;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 200;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	DMA_ITConfig (DMA1_Channel3, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA1_Channel3, ENABLE);
}

void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
}

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* Encoders:
	 * TIM2, TIM2_REMAP[1:0] = �11� (full remap), CH1 - PA15, CH2 - PB3
	 * TIM3, TIM3_REMAP[1:0] = �00� (no remap)CH1 - PA6, CH2 - PA7
	 * TIM4, TIM4_REMAP = 0, CH1 - PB6, CH2 - PB7
	 */
	TIM_SetAutoreload(TIM2, 0xFFFF);
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM2, ENABLE);

	TIM_SetAutoreload(TIM3, 0xFFFF);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM3, ENABLE);

	TIM_SetAutoreload(TIM4, 0xFFFF);
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Rising);
	TIM_Cmd(TIM4, ENABLE);

	/*PWM Generation:
	 * TIM1, TIM1_REMAP[1:0] = �00� (no remap)
	 * CH1 - CH2 - CH3  - CH4
	 * PA8 - PA9 - PA10 - PA11
	 */
    uint16_t TimerFrequency = 20000;
	uint16_t PrescalerValue = 0;//(uint16_t) (SystemCoreClock / 20000000) - 1; //1us?
    uint16_t CCR1_Val0 = 151;
    uint16_t CCR1_Val1 = 190;
    uint16_t CCR1_Val2 = 250;
    uint16_t CCR1_Val3 = 1030;


    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = (uint16_t) (SystemCoreClock/TimerFrequency);
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* PWM2 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val1;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val2;
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_Pulse = CCR1_Val3;
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    /* TIM3 enable counter */
    TIM_Cmd(TIM1, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}