#ifndef __DISP_H__
#define __DISP_H__

#include "stm32f10x.h"
#include "IPV.h"

// ������ ��������������� ������������. �������� ���������� �������� ��������� 
//�������.

// ��������� ������������ � ���������.
void HardwareInit(void);

// ���������� ��������� �������� DMA.
void DispDmaProc(void);

// ���������� ������� ������� ��������.
void DispBrightTimerProc(void);

// ���������� ������� ����� �������� �������� �������.
void DispTimerProc(void);

// ���������������� � �������� �������.
void DispRun(void);

// ���������� ������� �������� ������� (������ ������������ ������� ������� ��������).
void DispSetBright(unsigned short bt);

// ���������� ������ �� �������, ������� � ��������� �������.
// �������� ���������� ������� IpvStr() ��� ����������� ���������� �������.
unsigned long DispStr(unsigned long pos, const char *s, unsigned long n);

// ���������� ������ �� �������, ������� � ��������� ������� (�� X � Y).
// �������� ���������� ������� IpvStrXY() ��� ����������� ���������� �������.
unsigned long DispStrXY(unsigned long posX, unsigned long posY, const char *s, unsigned long n);

// �������� �������.
void DispClear(void);

// ��������, ��.
void DelayMs(unsigned short t);
#endif
