#ifndef __IPV_H__
#define __IPV_H__
// ������ ������ ��� 1 ������ ����������.
// ������ ������� �� 4 ����������� ��������������� ��������.
enum {ONE_COL_BUF_SIZE = 7*4*2}; 
enum {Y_NUM = 5}; // ���������� �������� � ����������.
enum {CHARS_IN_IND = 4}; // ���������� �������� � ����� ����������.
enum {ONE_IND_BUF_SIZE = ONE_COL_BUF_SIZE * Y_NUM}; // ������ ������ ��� 1 ���������.

typedef struct
{
	unsigned char indInRow; // ���������� ����������� � ������.
	unsigned char rowNum; // ���������� �������� �����������.
	char *textBuf; // ��������� �����.
	unsigned char *cmdBuf; // ����� ������.
	const unsigned long *symbCodes; // ���� ��������. ������ 320 ����.
	unsigned char unusedPinsValue; // �������� �������������� � ������ ������ ����� �����.
	struct // ������� ��������. �� ������ ���������� �����. 
	{
		unsigned char y; // ����� ������������� �������.(0-4)
		unsigned char redraw; // ������������� �����������.
	} status;
} Ipv;

typedef struct
{
	unsigned long addr;
	unsigned long size;
} IpvCmd;


// ������������� �������.
// * disp - ����������� ��������� �������.
// � ������ ��������� ����� ������� IpvInit() ������ ���� ���������������� ����:
// indInRow, rowNum, textBuf, cmdBuf, symbCodes � unusedPinsValue ��� �������������.
void IpvInit(Ipv* disp);

// ��������� ��������� � ��������� ������ � �������� ����� ������ ��� �������������.
// ���������� ��������� � ������� � �������� ������ ��� �������� � ������� ��� �������� ������ �������.
// ������ ����� �������� ������� �� ���������.
// * disp - ����������� ��������� �������.
IpvCmd IpvUpdate(Ipv* disp);

// ���������� ����� �������� ������� (0..4).
unsigned char IpvY(const Ipv* disp);

// ���������� ������ �� �������, ������� � ��������� �������.
//    pos - ������� ������� �� ������� (����� �������, ������ ����);
//    *s - ������������ ������;
//     n - ���������� ������������ ��������;
//     ���������� ���������� ������������ ��������.

unsigned long IpvStr(Ipv* d, unsigned long pos, const char *s, unsigned long n);
// ���������� ������ �� �������, ������� � ��������� ������� (�� X � Y).
//    posX - ������� ������� �� ������� �� ��� X;
//    posY - ������� ������� �� ������� �� ��� Y;
//    *s - ������������ ������;
//     n - ���������� ������������ ��������.
//     ���������� ���������� ������������ ��������.
unsigned long IpvStrXY(Ipv* disp, unsigned long posX, unsigned long posY,
						const char *str, unsigned long n);

// �������� �������.
void IpvClear(Ipv* disp);

#endif
