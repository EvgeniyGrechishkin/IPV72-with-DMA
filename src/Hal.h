#ifndef __DISP_H__
#define __DISP_H__

#include "stm32f10x.h"
#include "IPV.h"

// Модуль абстрагирования оборудования. Содержит реализацию аппарато зависимых 
//функций.

// Настройка тактирования и переферии.
void HardwareInit(void);

// Обработчик окончания передачи DMA.
void DispDmaProc(void);

// Обработчик таймера яркости свечения.
void DispBrightTimerProc(void);

// Обработчик таймера цикла свечения текущего столбца.
void DispTimerProc(void);

// Инициализировать и включить дисплей.
void DispRun(void);

// Установить яркость свечения дисплея (задать длительность таймера яркости свечения).
void DispSetBright(unsigned short bt);

// Отобразить сторку на дисплее, начиная с указанной позиции.
// Содержит реализацию функции IpvStr() для конкретного экземпляра дисплея.
unsigned long DispStr(unsigned long pos, const char *s, unsigned long n);

// Отобразить сторку на дисплее, начиная с указанной позиции (по X и Y).
// Содержит реализацию функции IpvStrXY() для конкретного экземпляра дисплея.
unsigned long DispStrXY(unsigned long posX, unsigned long posY, const char *s, unsigned long n);

// Очистить дисплей.
void DispClear(void);

// Задержка, мс.
void DelayMs(unsigned short t);
#endif
