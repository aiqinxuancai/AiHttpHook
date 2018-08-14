#pragma once

#include "Singleton.hpp"
#include "AiBytes.h"

class CPackhostName
{
public:
	CPackhostName(void);
	~CPackhostName(void);

public:
	DWORD id ;
	wstring hostName; //服务器域名
};

class CPackData
{
public:
	CPackData(void);
	~CPackData(void);

public:
	DWORD id ;
	DWORD time;
	wstring path; 
	string responseData;
	string postData;
	BOOL isEnd;
	wstring hostName; //服务器域名
};

typedef vector<CPackData> PackList;
typedef vector<CPackhostName> PackHostNameList;

typedef void (_stdcall *DataCallback)(const char * path, 
	const char * responseData, 
	const char * postData, 
	const char * hostName);//, DWORD data_len

class CHttpRecv : public Singleton<CHttpRecv>
{
	DECLARE_SINGLETON_CLASS(CHttpRecv)
public:
	CHttpRecv(void);
	~CHttpRecv(void);
	
	BOOL SetCallback(PVOID _proc);
	//BOOL Call(const char * file, const char * data, const char * send, DWORD _data_len);
	//BOOL Call(const char * file, const char * data, const char * send); //, DWORD _data_len;
	BOOL Call(const char * path, string & responseData, const char * postData, const char * hostName); //, DWORD _data_len;
	DWORD GetLocaleTimestampI();
	void StartId(DWORD id, wstring & file);
	void AddhostName(DWORD id, wstring &  hostName);
	wstring GetHostName(DWORD id);
	bool IshostNameIdExist(DWORD id);
	void PushData(DWORD id, string & data);
	void PushSendData(DWORD id, string & data);
	bool IsIdExist(DWORD id);
	string CloseId(DWORD id, wstring & file, string & send, wstring & hostName);
	bool CheckIdTime();
private:
	PackList m_data;
	PackHostNameList m_hostName;
	DataCallback m_callback;
	CRITICAL_SECTION m_cs; 
public:

};