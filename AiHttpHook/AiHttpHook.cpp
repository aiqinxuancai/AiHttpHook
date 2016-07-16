// AiHttpHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AiHttpHook.h"
#include "DebugString.h"
#include "HookFun.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//静态原函数
static HINTERNET (WINAPI * OLD_HttpOpenRequestW)(HINTERNET hConnect,LPCWSTR lpszVerb,LPCWSTR lpszObjectName,LPCWSTR lpszVersion,LPCWSTR lpszReferrer,LPCWSTR * lplpszAcceptTypes,DWORD dwFlags,DWORD_PTR dwContext
	) = HttpOpenRequestW;

static BOOL (WINAPI * OLD_HttpSendRequestW)(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength
	) = HttpSendRequestW;

static BOOL (WINAPI * OLD_InternetCloseHandle)(HINTERNET hInternet) = InternetCloseHandle;

static BOOL (WINAPI * OLD_InternetReadFile)( HINTERNET hFile,LPVOID lpBuffer,DWORD dwNumberOfBytesToRead,LPDWORD lpdwNumberOfBytesRead) = InternetReadFile;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI New_InternetReadFile( HINTERNET hFile,LPVOID lpBuffer,DWORD dwNumberOfBytesToRead,LPDWORD lpdwNumberOfBytesRead)
{
	BOOL ret = OLD_InternetReadFile(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
	return ret;
}

BOOL WINAPI New_InternetCloseHandle(HINTERNET hRequest)
{
	BOOL ret = OLD_InternetCloseHandle(hRequest);
	return ret;
}

BOOL WINAPI New_HttpSendRequestW(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength)
{

	BOOL ret = OLD_HttpSendRequestW(hRequest,lpszHeaders,dwHeadersLength,lpOptional,dwOptionalLength);
	return ret;
}

HINTERNET WINAPI New_HttpOpenRequestW(
	 HINTERNET hConnect,
	 LPCWSTR lpszVerb,
	 LPCWSTR lpszObjectName,
	 LPCWSTR lpszVersion,
	 LPCWSTR lpszReferrer,
	 LPCWSTR * lplpszAcceptTypes,
	 DWORD dwFlags,
	 DWORD_PTR dwContext
	)
{

	OutputDebugPrintf(L"AiHttpHook:%s", lpszObjectName);

	HINTERNET ret = OLD_HttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion,
		lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);

	OutputDebugPrintf(L"AiHttpHook:%s", lpszVersion);
	return ret;
}






// 开始 Hook 
AIHTTPHOOK_API int StartPageHook(void)
{
	
	HookAPI(&(PVOID&)OLD_HttpOpenRequestW, New_HttpOpenRequestW);
	HookAPI(&(PVOID&)OLD_HttpSendRequestW, New_HttpSendRequestW);
	HookAPI(&(PVOID&)OLD_InternetCloseHandle, New_InternetCloseHandle);
	HookAPI(&(PVOID&)OLD_InternetReadFile, New_InternetReadFile);

	return 0;
}

