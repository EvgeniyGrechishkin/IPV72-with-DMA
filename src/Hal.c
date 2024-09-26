#include "Hal.h"
#include "SymbCodes.h"
enum {CPU_CLK = 72000000};

// #define SYSCLK_TO_PA8 // Вывести SYSCLK на вывод MCO (PA8).ы

// Зажечь столбец индикатора с номером n (0..4)
#define ON_Y(n) MODIFY_REG(GPIOA->ODR, 0xE0U, ((n<<5)&0xE0U))
// Погасить все столбцы дисплея.
#define OFF_Y() MODIFY_REG(GPIOA->ODR, 0xE0U, 5U<<5)
// Разрешить работу дисплея (управление выводом en).
#define DISP_EN(val) MODIFY_REG(GPIOA->ODR, 1U<<8, ((val)&0x1U) << 8)

enum {IND_IN_ROW = 3}; // Количество индикаторов в строке.
enum {ROW_NUM = 4}; // Количество строк.

char txtBuf[CHARS_IN_IND * IND_IN_ROW * ROW_NUM]; // Текстовый буфер дисплея.
static unsigned char cmdBuf[ONE_IND_BUF_SIZE*3]; // Буфер с символами.
// Инициализация структуры управления дисплеем.
static Ipv ind = {IND_IN_ROW, ROW_NUM, txtBuf, cmdBuf, Codes, 0xFF};


static void Disp_StartDma(IpvCmd cmd)
{
	// DMA ch5
		DMA1_Channel5->CCR = 0;
		DMA1_Channel5->CNDTR = cmd.size; // Number of data items to transfer.
		DMA1_Channel5->CPAR = (uint32_t) (&(GPIOA->ODR)); // Peripheral address.
		DMA1_Channel5->CMAR = cmd.addr; // Memory address.
 		DMA1->IFCR = DMA_IFCR_CTCIF5; // Clear transfer complete interrupt flag.
   	DMA1_Channel5->CCR = DMA_CCR5_EN | // Channel enabled.
											 DMA_CCR5_MINC | // Memory increment mode enabled.
											 DMA_CCR5_DIR  | // Read from memory.
											 DMA_CCR5_TCIE;  // Transfer complete interrupt enable.
	// Tim
	enum {TIM1_CLK = 1000000};
	TIM1->CR1 = 0; // Off timer. Reset settings.
	TIM1->PSC = 0;
	TIM1->ARR = CPU_CLK / TIM1_CLK-1;
	TIM1->CR1 |= TIM_CR1_CEN; // Counter enable.
	TIM1->EGR |= TIM_EGR_UG; // Generate an update of the registers.
	TIM1->SR = 0; // Clear all flags.
	TIM1->DIER = TIM_DIER_UDE; // Update DMA request enable.
}

static void Disp_StartTimer(void)
{
	TIM1->CR1 = TIM_CR1_OPM; // Reset settings and set one pulse mode.
	TIM1->PSC = 1;
	TIM1->ARR = 65535;
	TIM1->EGR |= TIM_EGR_UG; // Generate an update of the registers.
	TIM1->SR = 0; // Clear all flags.
	TIM1->DIER = TIM_DIER_UIE |  // Update interrupt enable.
				TIM_DIER_CC1IE; // Capture/Compare 1 interrupt enable.
	TIM1->CR1 |= TIM_CR1_CEN; // Counter enable.
}

void HardwareInit(void)
{
#ifdef SYSCLK_TO_PA8
		// Debug. SYSCLK to MCO.
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_CNF8, GPIO_CRH_CNF8_1); // Alternate function output Push-pull
	MODIFY_REG(GPIOA->CRH, GPIO_CRH_MODE8, GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1); // PA8 50MHz max
	MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO_0 | RCC_CFGR_MCO_1 | RCC_CFGR_MCO_2,RCC_CFGR_MCO_2); // SYSCLK to MCO
#endif
	
	/* Enable Prefetch Buffer */
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);

	/* Flash 2 wait state */
		MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
	
	// Переключить источник тактирования на HSI.
	SET_BIT(RCC->CR, RCC_CR_HSION); // Вкл. внутр. генератор 0.
	while(!READ_BIT(RCC->CR, RCC_CR_HSIRDY)){};
	CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW); // Тактирование с  
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);

		// 72MHz
	CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Откл. байпас для кварца.        
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLMULL, RCC_CFGR_PLLMULL9); // PLLMUL = 9

	SET_BIT(RCC->CR, RCC_CR_HSEON); // Вкл. HSE.     
	while(!READ_BIT(RCC->CR, RCC_CR_HSERDY)){}; // Ожидание готовности HSE.
	SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC_HSE); // HSE->PLL

	SET_BIT(RCC->CR, RCC_CR_PLLON); // PLL on.
	while(!READ_BIT(RCC->CR, RCC_CR_PLLRDY)){}; // Ожидание готовности PLL.
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4); // APB1 = AHB/4
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_1); // PLL->SYSCLK


	// GPIOA - disp
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	// GPIOA.0..7 - General output push-pull mode, max speed 50MHz.
	WRITE_REG(GPIOA->CRL, 
							GPIO_CRL_MODE0_1 | GPIO_CRL_MODE0_0 |
							GPIO_CRL_MODE1_1 | GPIO_CRL_MODE1_0 |
							GPIO_CRL_MODE2_1 | GPIO_CRL_MODE1_0 |
							GPIO_CRL_MODE3_1 | GPIO_CRL_MODE3_0 |
							GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0 |
							GPIO_CRL_MODE5_1 | GPIO_CRL_MODE5_0 |
							GPIO_CRL_MODE6_1 | GPIO_CRL_MODE6_0 |
							GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0
			);

	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);

	SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	NVIC_EnableIRQ(TIM1_UP_IRQn);
	NVIC_EnableIRQ(TIM1_CC_IRQn);
}

void DispDmaProc(void)
{
	ON_Y(IpvY(&ind));
	Disp_StartTimer();
}

void DispTimerProc(void)
{
	OFF_Y();
	Disp_StartDma(IpvUpdate(&ind));
}

void DispRun(void)
{
	IpvInit(&ind);
	DISP_EN(1);
	Disp_StartDma(IpvUpdate(&ind));
}

void DispBrightTimerProc(void)
{
	OFF_Y();
}

void DispSetBright(unsigned short bt)
{
	TIM1->CCR1 = bt;
}

unsigned long DispStr(unsigned long pos, const char *s, unsigned long n)
{
	return IpvStr(&ind, pos, s, n);
}

unsigned long DispStrXY(unsigned long posX, unsigned long posY, const char *s, unsigned long n)
{
	return IpvStrXY(&ind, posX, posY, s, n);
}

void DispClear(void)
{
	IpvClear(&ind);
}

void DelayMs(unsigned short t)
{
	TIM3->PSC = 35000-1;
	TIM3->ARR = t;
	TIM3->EGR |= TIM_EGR_UG; // Generate an update of the registers.
	TIM3->SR = 0; // Clear all flags.
	TIM3->CR1 = TIM_CR1_CEN; // Counter enable.
	while(!(TIM3->SR & TIM_SR_UIF)){__ASM("nop");}
}
