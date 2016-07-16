#include "stdafx.h"


void OutputDebugPrintf(const wchar_t* strOutputString, ...)
{
	wchar_t strBuffer[40960] = {0};
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnwprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
}

void OutputDebugPrintf(const char* strOutputString, ...)
{
	char strBuffer[40960] = {0};
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
}