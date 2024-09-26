#ifndef __IPV_H__
#define __IPV_H__
// Размер буфера под 1 строку индикатора.
// Строка состоит из 4 соединенных последовательно столбцов.
enum {ONE_COL_BUF_SIZE = 7*4*2}; 
enum {Y_NUM = 5}; // Количество столюцов в индикаторе.
enum {CHARS_IN_IND = 4}; // Количество символов в одном индикаторе.
enum {ONE_IND_BUF_SIZE = ONE_COL_BUF_SIZE * Y_NUM}; // Размер буфера под 1 индикатор.

typedef struct
{
	unsigned char indInRow; // Количество индикаторов в строке.
	unsigned char rowNum; // Количество столбцов индикаторов.
	char *textBuf; // Текстовый буфер.
	unsigned char *cmdBuf; // Буфер команд.
	const unsigned long *symbCodes; // Коды символов. Размер 320 слов.
	unsigned char unusedPinsValue; // Значение неиспользуемых в данном модуле пинов порта.
	struct // Текущие значения. Не должны изменяться извне. 
	{
		unsigned char y; // Номер отображаемого столбца.(0-4)
		unsigned char redraw; // Необходимость перерисовки.
	} status;
} Ipv;

typedef struct
{
	unsigned long addr;
	unsigned long size;
} IpvCmd;


// Инициализация дисплея.
// * disp - управляющая структура дисплея.
// В данной структуре перед вызовом IpvInit() должны быть инициализированы поля:
// indInRow, rowNum, textBuf, cmdBuf, symbCodes и unusedPinsValue при необходимости.
void IpvInit(Ipv* disp);

// Проверить изменения в текстовом буфере и обновить буфер команд при необходимости.
// Возвращает стркутуру с адресом и размером данных для передачи в дисплей для текущего номера столбца.
// Меняет номер текущего столбца на следующий.
// * disp - управляющая структура дисплея.
IpvCmd IpvUpdate(Ipv* disp);

// Возвращает номер текущего столбца (0..4).
unsigned char IpvY(const Ipv* disp);

// Отобразить сторку на дисплее, начиная с указанной позиции.
//    pos - позиция символа на дисплее (слева направо, сверху вниз);
//    *s - отображаемая строка;
//     n - количество отображаемых символов;
//     Возвращает количество напечатанных символов.

unsigned long IpvStr(Ipv* d, unsigned long pos, const char *s, unsigned long n);
// Отобразить сторку на дисплее, начиная с указанной позиции (по X и Y).
//    posX - позиция символа на дисплее по оси X;
//    posY - позиция символа на дисплее по оси Y;
//    *s - отображаемая строка;
//     n - количество отображаемых символов.
//     Возвращает количество напечатанных символов.
unsigned long IpvStrXY(Ipv* disp, unsigned long posX, unsigned long posY,
						const char *str, unsigned long n);

// Очистить дисплей.
void IpvClear(Ipv* disp);

#endif
