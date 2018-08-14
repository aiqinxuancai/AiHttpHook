#pragma once

#include "Singleton.hpp"



class CPackData
{
public:
	CPackData(void);
	~CPackData(void);

public:
	HINTERNET connectId; 
	HINTERNET id ;
	DWORD startTime;
	wstring path; 
	string responseData;
	string postData;
	string responseHeaders;
	BOOL isEnd;
	wstring hostName; //·þÎñÆ÷ÓòÃû
};



typedef vector<CPackData> PackList;

typedef void (_stdcall *DataCallback)(const char * path, 
	const char * responseData, 
	const char * postData, 
	const char * hostName,
	const char * responseHeaders);

class CHttpRecv : public Singleton<CHttpRecv>
{
	DECLARE_SINGLETON_CLASS(CHttpRecv)
public:
	CHttpRecv(void);
	~CHttpRecv(void);
	
	BOOL SetCallback(PVOID _proc);
	//BOOL Call(const char * file, const char * data, const char * send, DWORD _data_len);
	//BOOL Call(const char * file, const char * data, const char * send); //, DWORD _data_len;
	BOOL Call(const char * path, string & responseData, const char * postData, const char * hostName, const char * responseHeaders); //, DWORD _data_len;
	DWORD GetLocaleTimestampI();
	void StartId(HINTERNET id, HINTERNET connectId, wstring & path);
	void SetHostName(HINTERNET id, wstring &  hostName);
	wstring GetHostName(HINTERNET id);
	void PushData(HINTERNET id, string & data);
	void SetPostData(HINTERNET id, string & data);
	void SetResponseHeaders(HINTERNET id, string & headers);
	bool IsIdExist(HINTERNET id);
	void CloseRequestAndCall(HINTERNET id);
	string CloseId(HINTERNET id, wstring & path, string & postData, wstring & hostName, string & responseHeaders);
	bool CheckIdTime();
private:
	PackList m_data;
	DataCallback m_callback;
	CRITICAL_SECTION m_cs; 
	map<HINTERNET, wstring> m_mapHostName;
public:

};