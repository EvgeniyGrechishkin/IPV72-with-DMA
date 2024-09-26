#include "IPV.h"

// Заполнить буфер команд (коды изображения) перед отправкой в дисплей.
// *disp - управляющая структура дисплея, y - текущий номер столбца.
static void FillCmdBuf(Ipv* disp, unsigned char y)
{
	unsigned char unusedPinsVal = disp->unusedPinsValue &
												(0xFF << (disp->rowNum + 1));
	unsigned char *buf = disp->cmdBuf + disp->indInRow * ONE_COL_BUF_SIZE * y;
	const char *symbColumnCode = 
					(const char *)(disp->symbCodes + y * 64);
	unsigned long byteNum = 0;
	const unsigned long rowLen = disp->indInRow * CHARS_IN_IND;

	for(unsigned long i = 1; i <= rowLen; i++)
	{
		unsigned long rowInd = rowLen - i;
		for(unsigned long bitNum = 0; bitNum < 7; bitNum++)
		{
			// Установка неиспользвемых выводов в предопределенное состояние.
			buf[byteNum] = unusedPinsVal; 
			for(unsigned long colInd = 0; colInd < disp->rowNum; colInd++)
			{
				unsigned long curIndex = colInd * rowLen + rowInd;
				char ch = disp->textBuf[curIndex];
				buf[byteNum] |= 
					(((symbColumnCode[(int)ch] >> bitNum) & 1) << (1 + colInd));
			}
			buf[byteNum+1] = buf[byteNum];
			buf[byteNum] |= 1; // Строб индикатора.
			byteNum +=2;
		}
	}
}

void IpvInit(Ipv* disp)
{
	disp->status.y = Y_NUM;
	disp->status.redraw = 1;
}

IpvCmd IpvUpdate(Ipv* disp)
{
	disp->status.y++;
	if(disp->status.y > 4)
	{
		disp->status.y = 0;
		if(disp->status.redraw) // Текст изменился. Требуется перерисовка.
		{
			for(unsigned char i = 0; i < Y_NUM; i++)
			{
				FillCmdBuf(disp, i);
			}
			disp->status.redraw = 0;
		}
	}

	IpvCmd cmd;
	cmd.size = disp->indInRow * ONE_COL_BUF_SIZE; 
	cmd.addr = (unsigned long)(disp->cmdBuf + cmd.size * disp->status.y);
	return cmd;
}

unsigned char IpvY(const Ipv* disp)
{
	return disp->status.y;
}

unsigned long IpvStr(Ipv* disp, unsigned long pos, 
						const char *str, unsigned long n)
{
	const unsigned long maxLen = disp->indInRow * CHARS_IN_IND * disp->rowNum;
	if(pos >= maxLen) { return 0; }
	unsigned long empty = maxLen - pos;
	n = (n > empty) ? empty : n;
	unsigned long i = 0;
	for(; i < n && str[i]; i++)
	{
		if(disp->textBuf[i+pos] != str[i])
		{
			disp->textBuf[i+pos] = str[i];
			disp->status.redraw = 1;
		}
	}
	return i;
}

unsigned long IpvStrXY(Ipv* disp, unsigned long posX, unsigned long posY,
							const char *str, unsigned long n)
{
	const unsigned long maxLen = disp->indInRow * CHARS_IN_IND;
	if(posX >= maxLen || posY > disp->rowNum - 1) { return 0; }
	unsigned long empty = maxLen - posX;
	n = (n > empty) ? empty : n;
	unsigned long i = 0;
	for(; i < n && str[i]; i++)
	{
		unsigned long ind = i+posX + posY * maxLen;
		if(disp->textBuf[ind] != str[i])
		{
			disp->textBuf[ind] = str[i];
			disp->status.redraw = 1;
		}
	}
	return i;
}

void IpvClear(Ipv* disp)
{
	const unsigned long maxLen = disp->indInRow * CHARS_IN_IND * disp->rowNum;
	for(unsigned int i = 0; i < maxLen; i++)
		disp->textBuf[i] = ' ';
	disp->status.redraw = 1;
}
