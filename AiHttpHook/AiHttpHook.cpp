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
static HINTERNET (WINAPI * OLD_HttpOpenRequestW)(HINTERNET hConnect, 
	LPCWSTR lpszVerb, 
	LPCWSTR lpszObjectName, 
	LPCWSTR lpszVersion, 
	LPCWSTR lpszReferrer, 
	LPCWSTR * lplpszAcceptTypes, 
	DWORD dwFlags, 
	DWORD_PTR dwContext) = HttpOpenRequestW;

static BOOL (WINAPI * OLD_HttpSendRequestW)(HINTERNET hRequest, 
	LPCWSTR lpszHeaders, 
	DWORD dwHeadersLength, 
	LPVOID lpOptional, 
	DWORD dwOptionalLength) = HttpSendRequestW;

static BOOL (WINAPI * OLD_InternetCloseHandle)(HINTERNET hInternet) = InternetCloseHandle;

static BOOL (WINAPI * OLD_InternetReadFile)( HINTERNET hFile,
	LPVOID lpBuffer, 
	DWORD dwNumberOfBytesToRead, 
	LPDWORD lpdwNumberOfBytesRead) = InternetReadFile;

static HINTERNET (WINAPI * OLD_InternetConnectW)(HINTERNET hInternet, 
	LPCWSTR lpszhostName, 
	INTERNET_PORT nServerPort, 
	LPCWSTR lpszUserName, 
	LPCWSTR lpszPassword,							 
	DWORD dwService, 
	DWORD dwFlags, 
	DWORD_PTR dwContext) = InternetConnectW;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Hook回调函数

HINTERNET WINAPI New_InternetConnectW( HINTERNET hInternet, 
	LPCWSTR lpszhostName, 
	INTERNET_PORT nServerPort, 
	LPCWSTR lpszUserName, 
	LPCWSTR lpszPassword,
	DWORD dwService, 
	DWORD dwFlags, 
	DWORD_PTR dwContext)
{
	HINTERNET ret = OLD_InternetConnectW(hInternet, lpszhostName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
	CHttpRecv::Instance().AddHostName((HINTERNET)ret, wstring(lpszhostName)); //为ID添加Host
	
	return ret;
}

BOOL WINAPI New_InternetReadFile( HINTERNET hFile, 
	LPVOID lpBuffer, 
	DWORD dwNumberOfBytesToRead, 
	LPDWORD lpdwNumberOfBytesRead)
{
	BOOL ret = OLD_InternetReadFile(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);
	
	if (dwNumberOfBytesToRead > 0)
	{
		string readData = "";
		char * data = new char[dwNumberOfBytesToRead + 2];
		memset(data, 0, dwNumberOfBytesToRead + 2);
		memcpy(data, lpBuffer, dwNumberOfBytesToRead);
		readData = data; //不能使用append 因为这个函数不会在0x00位置截断
		delete [] data;
		CHttpRecv::Instance().PushData((HINTERNET)hFile, readData); //为ID添加返回的数据
		//CHttpRecv::Instance().PushData((DWORD)hFile, (char*)lpBuffer, dwNumberOfBytesToRead); //为ID添加返回的数据
	}
	return ret;
}


//void erase(string &s, char ch) {
//	for (int i = 0; i < s.length(); i++) {
//		if (s[i] == ch) {
//			s.erase(i, 1);
//			i--;
//		}
//	}
//}
//
//void ShowQueryData(HINTERNET hRequest, DWORD dwInfoLevel)
//{
//	DWORD len = 10240;
//	char * cache = new char[len];
//	HttpQueryInfoA(hRequest, dwInfoLevel, cache, &len, 0);
//	string hostString(cache);
//	PrintFDbg("AiHttpHookHost:%d - %s", dwInfoLevel, hostString.c_str());
//	delete[]cache;
//}



BOOL WINAPI New_InternetCloseHandle(HINTERNET hRequest)
{

	//if (CHttpRecv::Instance().IsIdExist(hRequest))
	//{
	//	ShowQueryData(hRequest, HTTP_QUERY_FLAG_REQUEST_HEADERS);
	//	ShowQueryData(hRequest, HTTP_QUERY_URI);
	//	ShowQueryData(hRequest, HTTP_QUERY_SERVER);
	//	ShowQueryData(hRequest, HTTP_QUERY_REFERER);
	//	ShowQueryData(hRequest, HTTP_QUERY_HOST);
	//	ShowQueryData(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF);
	//}

	BOOL ret = OLD_InternetCloseHandle(hRequest);
	CHttpRecv::Instance().CloseRequestAndCall(hRequest);
	return ret;
}

BOOL WINAPI New_HttpSendRequestW(HINTERNET hRequest, 
	LPCWSTR lpszHeaders, 
	DWORD dwHeadersLength, 
	LPVOID lpOptional, 
	DWORD dwOptionalLength)
{
	BOOL ret = OLD_HttpSendRequestW(hRequest, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength);
	string postData = "";
	if (dwOptionalLength > 0)
	{
		char * data = new char[dwOptionalLength + 1];
		memset(data, 0, dwOptionalLength + 1);
		memcpy(data, lpOptional, dwOptionalLength);
		postData.append(data);
		delete [] data;
		CHttpRecv::Instance().SetPostData((HINTERNET)hRequest, postData); //为ID添加请求的地址
	}

	
	//if (dwHeadersLength > 0)
	//{
	//	CHttpRecv::Instance().SetRequestHeaders((HINTERNET)hRequest, wstring(lpszHeaders)); //为ID添加请求的地址
	//}

	//if (CHttpRecv::Instance().IsIdExist(hRequest))
	//{
	//	PrintFDbg(L"AiHttpHook#4:%d", dwHeadersLength);
	//	//PrintFDbg(L"AiHttpHook#4:%s", wstring(lpszHeaders).c_str());
	//}

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

	//避免被复用 所以提前检测并关闭
	CHttpRecv::Instance().CloseRequestAndCall(hConnect);
	
	wstring mode = lpszVerb;
	if (mode == L"POST")
	{
		CHttpRecv::Instance().StartId((HINTERNET)ret, wstring(lpszObjectName) ); //开始记录一个包
	}

	return ret;
}

// 开始 Hook 
AIHTTPHOOK_API int StartHook(PVOID callback)
{
	PrintFDbg(L"AiHttpHook:StartPageHook:Start");
	CHttpRecv::Instance().SetCallback(callback);

	HookAPI(&(PVOID&)OLD_HttpOpenRequestW, New_HttpOpenRequestW);
	HookAPI(&(PVOID&)OLD_HttpSendRequestW, New_HttpSendRequestW);
	HookAPI(&(PVOID&)OLD_InternetCloseHandle, New_InternetCloseHandle);
	HookAPI(&(PVOID&)OLD_InternetReadFile, New_InternetReadFile);

	//HookAPI(&(PVOID&)OLD_connect, New_connect);
	//HookAPI(&(PVOID&)OLD_InternetConnectW, New_InternetConnectW);

	PrintFDbg(L"AiHttpHook:StartPageHook:End");
	
	return 0;
}

