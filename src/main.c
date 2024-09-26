#include "stm32f10x.h"
#include "Hal.h"

void TIM1_UP_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void TIM1_CC_IRQHandler(void);

typedef struct
{
	const char *str;
	unsigned short val;
} Bright;

const Bright brtArr[] = {
							{"MININUM", 1},
							{"  10%  ", 500},
							{"  20%  ", 2000},
							{"  30%  ", 4500},
							{"  40%  ", 6500},
							{"  50%  ", 8000},
							{"  60%  ", 10000},
							{"  70%  ", 25000},
							{"  80%  ", 35000},
							{"  90%  ", 45000},
							{"MAXIMUM", 0xFFFF/100*100}
						};

enum {PAUSE = 1000}; // Пауза между изображениями.

int main(void)
{
	char str[48];
	unsigned char ind = 0;

	HardwareInit();
	DispRun();
	DispSetBright(65535 / 2); // Среднее значение яркости.

	while(1)
	{
		DispStr(0,  "************"
					"DISPLAY DEMO"
					"    MODE    "
					"************", 48);
		DelayMs(PAUSE);

		DispStr(0,  "************"
					"  SYMBOLS   "
					"            "
					"************", 48);
		DelayMs(PAUSE);

		// Отобразить все печатыемые символы на нескольких изображениях.
		for(unsigned long i = 48; i < 256; i++)
		{
			str[ind++] = (char)i;
			if(ind == 48 || i == 256-1)
			{
				// Изображение заполнено. Отобразить.
				DispStr(0, str, ind);
				DelayMs(PAUSE);
				DispClear();
				ind = 0;
			}
		}

		DispStr(0,  "************"
					"   BRIGHT   "
					"            "
					"************", 48);
		DelayMs(PAUSE);
 
		// Изменение яркости. 
		for(unsigned char i = 0; i < sizeof(brtArr) / sizeof(*brtArr); i++)
		{
			DispSetBright(brtArr[i].val);
			DispStrXY(2,2, brtArr[i].str, 7);
			DelayMs(PAUSE);
		}

		// Засветить все точки дисплея. Максимальное потребление.
		for(unsigned long i = 0; i < 48; i++)
			str[i] = 166; // Символ со всеми засвеченными точками.
		DispStr(0, str, 48);
		DelayMs(PAUSE);
		DelayMs(PAUSE);
	}
}


// Окончание передачи DMA. Передан текущий столбец.
void DMA1_Channel5_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF5)
	{
		DMA1->IFCR = DMA_IFCR_CTCIF5;
		DispDmaProc(); // Зажечь текущий столбец и запустить таймер яркости.
	}
}

// Таймер яркости. Определяет яркость свечения дисплея.
void TIM1_CC_IRQHandler(void)
{
	if(TIM1->SR & TIM_SR_CC1IF)
	{
		TIM1->SR &=~TIM_SR_CC1IF;
		DispBrightTimerProc(); // Погасить текущий столбец.
	}
}

// Таймер цикла свечения текущего столбца. Должен сработать позже таймера яркости.
void TIM1_UP_IRQHandler(void)
{
  if(TIM1->SR & TIM_SR_UIF)
	{
		TIM1->SR = 0;
		DispTimerProc(); // Запустить передачу следующего столбца через DMA.
	}
}