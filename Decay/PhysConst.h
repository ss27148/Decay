// Copyright (c) 2016 SS

#ifndef PHYSCONST_H
#define PHYSCONST_H

#define _USE_MATH_DEFINES
#include <math.h>

namespace PhysConst
{
	const double qe		= 1.602176462E-19;	// ����� ���������
	const double me		= 9.10938188E-31;	// ����� ���������
	const double mp		= 1.672621637E-27;	// ����� �������
	const double malpha = 6.6446562E-27;	// ����� �����-�������
	const double AEM	= 1.66053904E-27;	// ������� ������� �����
	const double u0		= (4E-7 * M_PI);	// ��������� ����������
	const double e0		= 8.854187817E-12;	// ������������� ����������
	const double h		= 6.62606876E-34;	// ���������� ������
	const double h1		= 1.054571596E-34;	// ���������� ������
	const double k		= 1.3806503E-23;	// ���������� ���������
	const double NA		= 6.02214199E23;	// ����� ��������
};
#endif // PHYSCONST_H
