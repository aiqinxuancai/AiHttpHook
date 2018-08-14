#pragma once

#include "Singleton.hpp"
#include "AiBytes.h"

class CPackHostName
{
public:
	CPackHostName(void);
	~CPackHostName(void);

public:
	HINTERNET id ;
	wstring hostName; //服务器域名
};

class CPackData
{
public:
	CPackData(void);
	~CPackData(void);

public:
	HINTERNET id ;
	DWORD startTime;
	wstring path; 
	string responseData;
	string postData;
	wstring requestHeaders;
	BOOL isEnd;
	wstring hostName; //服务器域名
};

typedef vector<CPackData> PackList;
typedef vector<CPackHostName> PackHostNameList;

typedef void (_stdcall *DataCallback)(const char * path, 
	const char * responseData, 
	const char * postData, 
	const char * hostName,
	const char * requestHeaders);

class CHttpRecv : public Singleton<CHttpRecv>
{
	DECLARE_SINGLETON_CLASS(CHttpRecv)
public:
	CHttpRecv(void);
	~CHttpRecv(void);
	
	BOOL SetCallback(PVOID _proc);
	//BOOL Call(const char * file, const char * data, const char * send, DWORD _data_len);
	//BOOL Call(const char * file, const char * data, const char * send); //, DWORD _data_len;
	BOOL Call(const char * path, string & responseData, const char * postData, const char * hostName, const char * requestHeaders); //, DWORD _data_len;
	DWORD GetLocaleTimestampI();
	void StartId(HINTERNET id, wstring & file);
	void AddHostName(HINTERNET id, wstring &  hostName);
	wstring GetHostName(HINTERNET id);
	bool IsHostNameIdExist(HINTERNET id);
	void PushData(HINTERNET id, string & data);
	void SetPostData(HINTERNET id, string & data);
	void SetRequestHeaders(HINTERNET id, wstring & headers);
	bool IsIdExist(HINTERNET id);
	void CloseRequestAndCall(HINTERNET id);
	string CloseId(HINTERNET id, wstring & path, string & postData, wstring & hostName, wstring & requestHeaders);
	bool CheckIdTime();
private:
	PackList m_data;
	PackHostNameList m_hostName;
	DataCallback m_callback;
	CRITICAL_SECTION m_cs; 
public:

};