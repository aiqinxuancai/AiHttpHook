#pragma once

#include "Singleton.hpp"
#include "AiBytes.h"

class CPackServerName
{
public:
	CPackServerName(void);
	~CPackServerName(void);

public:
	DWORD id ;
	wstring serverName; //服务器域名
};

class CPackData
{
public:
	CPackData(void);
	~CPackData(void);

public:
	DWORD id ;
	DWORD time ;
	wstring file;
	string dat;
	AiBytes data;
	string send;
	BOOL end;
	wstring serverName; //服务器域名
};

typedef vector<CPackData> PackList;
typedef vector<CPackServerName> PackServerNameList;
typedef void (_stdcall *DataCallback)(const char * file, const char * data, const char * send, const char * serverName);//, DWORD data_len

class CHttpRecv : public Singleton<CHttpRecv>
{
	DECLARE_SINGLETON_CLASS(CHttpRecv)
public:
	CHttpRecv(void);
	~CHttpRecv(void);
	
	BOOL SetCallback(PVOID _proc);
	//BOOL Call(const char * file, const char * data, const char * send, DWORD _data_len);
	//BOOL Call(const char * file, const char * data, const char * send); //, DWORD _data_len;
	BOOL Call(const char * file, string & data, const char * send, const char * serverName); //, DWORD _data_len;
	DWORD GetLocaleTimestampI();
	void StartId(DWORD _id, wstring & _file);
	void AddServerName(DWORD _id, wstring &  _serverName);
	wstring GetServerName(DWORD _id);
	bool IsServerNameIdExist(DWORD _id);
	void PushData(DWORD _id, string & _data);
	void PushData(DWORD _id, char * _data, UINT _len);
	void PushSendData(DWORD _id, string & _data);
	bool IsIdExist(DWORD _id);
	string CloseId(DWORD _id, wstring & _file, string & _send, wstring & _serverName);
	bool CheckIdTime();
private:
	PackList m_data;
	PackServerNameList m_serverName;
	DataCallback m_callback;
	CRITICAL_SECTION m_cs; 
public:

};