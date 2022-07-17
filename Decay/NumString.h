// Copyright (c) 2017 SS

#pragma once

BOOL StringToInt(int& val, LPCTSTR str, int radix = 10);
BOOL StringToUInt(UINT& val, LPCTSTR str, int radix = 10);
BOOL StringToFloat(float& val, LPCTSTR str);
BOOL StringToDouble(double& val, LPCTSTR str);

