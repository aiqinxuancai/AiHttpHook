// AiHttpHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AiHttpHook.h"
#include "DebugString.h"
#include "HookFun.h"
#include "HttpRecv.h"
#include "UnicodeConv.h"
#include "HookNameToHost.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//静态原函数
static HINTERNET (WINAPI * OLD_HttpOpenRequestW)(HINTERNET hConnect,LPCWSTR lpszVerb,LPCWSTR lpszObjectName,LPCWSTR lpszVersion,LPCWSTR lpszReferrer,LPCWSTR * lplpszAcceptTypes,DWORD dwFlags,DWORD_PTR dwContext
	) = HttpOpenRequestW;

static BOOL (WINAPI * OLD_HttpSendRequestW)(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength) = HttpSendRequestW;

static BOOL (WINAPI * OLD_InternetCloseHandle)(HINTERNET hInternet) = InternetCloseHandle;

static BOOL (WINAPI * OLD_InternetReadFile)( HINTERNET hFile,LPVOID lpBuffer,DWORD dwNumberOfBytesToRead,LPDWORD lpdwNumberOfBytesRead) = InternetReadFile;

static HINTERNET (WINAPI * OLD_InternetConnectW)(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword,
											 DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext) = InternetConnectW;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Hook回调函数

HINTERNET WINAPI New_InternetConnectW( HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword,
								 DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{

	HINTERNET ret = OLD_InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName,lpszPassword,dwService,dwFlags,dwContext);
	CHttpRecv::Instance().AddServerName((DWORD)ret, wstring(lpszServerName));
	
	return ret;
}

BOOL WINAPI New_InternetReadFile( HINTERNET hFile,LPVOID lpBuffer,DWORD dwNumberOfBytesToRead,LPDWORD lpdwNumberOfBytesRead)
{
	BOOL ret = OLD_InternetReadFile(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
	
	if (dwNumberOfBytesToRead > 0)
	{
		string readData = "";
		char * data = new char[dwNumberOfBytesToRead+2];
		memset(data,0,dwNumberOfBytesToRead+2);
		memcpy(data, lpBuffer, dwNumberOfBytesToRead);
		readData = data; //不能使用append 因为这个函数不会在0x00位置截断
		delete [] data;
		CHttpRecv::Instance().PushData((DWORD)hFile, readData);
	}
	return ret;
}

BOOL WINAPI New_InternetCloseHandle(HINTERNET hRequest)
{
	BOOL ret = OLD_InternetCloseHandle(hRequest);

	wstring file = L"";
	wstring serverName = L"";
	string send = "";
	string retByte = CHttpRecv::Instance().CloseId((DWORD)hRequest, file, send, serverName);
	if (retByte.empty() == false)
	{
		CHttpRecv::Instance().CheckIdTime();
		CHttpRecv::Instance().Call(ConvertUnicodeToMultiBytes(file).c_str(), retByte, send.c_str(),ConvertUnicodeToMultiBytes(serverName).c_str());
	}
	return ret;
}

BOOL WINAPI New_HttpSendRequestW(HINTERNET hRequest,LPCWSTR lpszHeaders,DWORD dwHeadersLength,LPVOID lpOptional,DWORD dwOptionalLength)
{
	BOOL ret = OLD_HttpSendRequestW(hRequest,lpszHeaders,dwHeadersLength,lpOptional,dwOptionalLength);
	string postData = "";
	if (dwOptionalLength > 0)
	{
		char * data = new char[dwOptionalLength+1];
		memset(data,0,dwOptionalLength+1);
		memcpy(data,lpOptional,dwOptionalLength);
		postData.append(data);
		delete [] data;
		CHttpRecv::Instance().PushSendData((DWORD)hRequest, postData);

	}
	

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
	HINTERNET ret = OLD_HttpOpenRequestW(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
	wstring file = L"";
	string send = "";
	wstring serverName = L"";
	string retByte = CHttpRecv::Instance().CloseId((DWORD)ret, file, send, serverName);
	if (retByte.empty() == false)
	{
		CHttpRecv::Instance().CheckIdTime();
		CHttpRecv::Instance().Call(ConvertUnicodeToMultiBytes(file).c_str(), retByte, send.c_str(),ConvertUnicodeToMultiBytes(serverName).c_str());
	}
	
	wstring ver = lpszVerb;
	if (ver == L"POST")
	{
		CHttpRecv::Instance().StartId((DWORD)ret, wstring(lpszObjectName) );
	}

	return ret;
}






// 开始 Hook 
AIHTTPHOOK_API int StartPageHook(PVOID _proc)
{
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:Start");
	CHttpRecv::Instance().SetCallback(_proc);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:#1");
	HookAPI(&(PVOID&)OLD_HttpOpenRequestW, New_HttpOpenRequestW);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:#2");
	HookAPI(&(PVOID&)OLD_HttpSendRequestW, New_HttpSendRequestW);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:#3");
	HookAPI(&(PVOID&)OLD_InternetCloseHandle, New_InternetCloseHandle);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:#4");
	HookAPI(&(PVOID&)OLD_InternetReadFile, New_InternetReadFile);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:#5");

	//HookAPI(&(PVOID&)OLD_connect, New_connect);

	//HookAPI(&(PVOID&)OLD_InternetConnectW, New_InternetConnectW);
	OutputDebugPrintf(L"AiHttpHook:StartPageHook:End");
	

	

	return 0;
}

