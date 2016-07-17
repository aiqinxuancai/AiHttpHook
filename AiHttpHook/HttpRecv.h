#pragma once

#include "Singleton.hpp"



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
	string send;
	BOOL end;
};

typedef vector<CPackData> PackList;
typedef void (_stdcall *DataCallback)(const char * file, const char * data, const char * send);//, DWORD data_len

class CHttpRecv : public Singleton<CHttpRecv>
{
	DECLARE_SINGLETON_CLASS(CHttpRecv)
public:
	CHttpRecv(void);
	~CHttpRecv(void);
	
	BOOL SetCallback(PVOID _proc);
	//BOOL Call(const char * file, const char * data, const char * send, DWORD _data_len);
	//BOOL Call(const char * file, const char * data, const char * send); //, DWORD _data_len;
	BOOL Call(const char * file, string & data, const char * send); //, DWORD _data_len;
	DWORD GetLocaleTimestampI();
	void StartId(DWORD _id, wstring & _file);
	void PushData(DWORD _id, string & _data);
	void PushSendData(DWORD _id, string & _data);
	bool IsIdExist(DWORD _id);
	string CloseId(DWORD _id, wstring & _file, string & _send);
	bool CheckIdTime();
private:
	PackList m_data;
	DataCallback m_callback;
	CRITICAL_SECTION m_cs; 
public:

};