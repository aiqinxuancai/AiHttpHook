#include "stdafx.h"
#include "HttpRecv.h"
#include "UnicodeConv.h"
#include "DebugString.h"
#include <time.h>


CHttpRecv::CHttpRecv(void)
{
	InitializeCriticalSection(&m_cs);
}

CHttpRecv::~CHttpRecv(void)
{
}

BOOL CHttpRecv::SetCallback(PVOID callback)
{
	//typedef int(FUNC1)(int);  
	//int (*func)(int,int);  
	m_callback = (DataCallback)callback;
	return TRUE;
}

BOOL CHttpRecv::Call(const char * path, string & responseData, const char * postData, const char * hostName) //, DWORD _data_len
{
	//为了避免其他程序读到非0x00结尾 所以先copy一下
	size_t len = responseData.length() + 2;
	char * dataUse = new char[len];
	memset(dataUse, 0, len);
	memcpy(dataUse, responseData.c_str(), responseData.length());
	m_callback(path, dataUse, postData, hostName);
	delete[]dataUse;
	return TRUE;
}


DWORD CHttpRecv::GetLocaleTimestampI()
{
	__time32_t long_time;
	_time32(&long_time);
	return long_time;
}

void CHttpRecv::StartId(HINTERNET id, wstring & path)
{
	if (IsIdExist(id) == false)
	{
		CPackData data;
		data.id = id;
		data.path = path;
		data.time = GetLocaleTimestampI();
		data.hostName = GetHostName(id);
		//OutputDebugPrintf(L"AiHttpHook:StartId:%d,%s",data.id, data.file.c_str());
		EnterCriticalSection(&m_cs);
		m_data.push_back(data);
		LeaveCriticalSection(&m_cs);  
	}
}

void CHttpRecv::AddhostName(HINTERNET id, wstring & hostName)
{
	IshostNameIdExist(id); //清除之前有的id
	CPackhostName data;
	data.id = id;
	data.hostName.append(hostName);
	//EnterCriticalSection(&m_cs);
	//m_hostName.push_back(data);
	//LeaveCriticalSection(&m_cs); 

	//PrintFDbg(L"AiHttpHook:AddhostName:(%d)%d -> %s", m_hostName.size(), data.id, data.hostName);
}

wstring CHttpRecv::GetHostName(HINTERNET id)
{
	EnterCriticalSection(&m_cs);
	for(PackHostNameList::iterator it = m_hostName.begin(); it != m_hostName.end();)
	{
		if (it->id == id)
		{
			wstring name = it->hostName;
			m_hostName.erase(it);
			LeaveCriticalSection(&m_cs);  
			return name;
		}		
		else
		{
			++it;
		}
	}
	LeaveCriticalSection(&m_cs);  
	return L"";
}


bool CHttpRecv::IshostNameIdExist(HINTERNET id)
{
	EnterCriticalSection(&m_cs);
	for(PackHostNameList::iterator it = m_hostName.begin(); it != m_hostName.end();)
	{
		if (it->id == id)
		{
			m_hostName.erase(it);
			LeaveCriticalSection(&m_cs);  
		}		
		else
		{
			++it;
		}
	}
	LeaveCriticalSection(&m_cs);  
	return false;
}

void CHttpRecv::PushData(HINTERNET id, string & _data)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			m_data[i].responseData = m_data[i].responseData + _data;
		}
	}
	LeaveCriticalSection(&m_cs);  
}

void CHttpRecv::PushSendData(HINTERNET id, string & _data)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			m_data[i].postData = _data;
		}
	}
	LeaveCriticalSection(&m_cs);  
}

bool CHttpRecv::IsIdExist(HINTERNET id)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			LeaveCriticalSection(&m_cs);  
			return true;
		}
	}
	LeaveCriticalSection(&m_cs);  
	return false;
}

string CHttpRecv::CloseId(HINTERNET id, wstring & _file, string & _send, wstring & _hostName)
{
	string dat = "";
	EnterCriticalSection(&m_cs);
	for(PackList::iterator it = m_data.begin(); it != m_data.end();)
	{
		if (it->id == id)
		{
			dat = it->responseData;
			_file = it->path;
			_send = it->postData;
			_hostName = it->hostName;
			if (_file.length() > 128)
			{
				_file = L"LongData";
			}

			m_data.erase(it);

			LeaveCriticalSection(&m_cs);  
			return dat;
		}		
		else
		{
			++it;
		}
	}
	LeaveCriticalSection(&m_cs);  

	return dat;
}


bool CHttpRecv::CheckIdTime()
{
	DWORD time = GetLocaleTimestampI();
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (time - m_data[i].time > 30) //>12sec
		{
			m_data[i].isEnd = TRUE;
			Call(ConvertUnicodeToMultiBytes(m_data[i].path).c_str(), 
				m_data[i].responseData, 
				m_data[i].postData.c_str(),
				ConvertUnicodeToMultiBytes(m_data[i].hostName).c_str());
		}
	}

	for(PackList::iterator it = m_data.begin(); it != m_data.end();)
	{
		if (it->isEnd)
		{
			m_data.erase(it);
		}		
		else
		{
			++it;
		}
	}
	LeaveCriticalSection(&m_cs);  
	return true;
}


CPackData::CPackData(void)
{

}

CPackData::~CPackData(void)
{

}

CPackhostName::CPackhostName(void)
{

}

CPackhostName::~CPackhostName(void)
{

}
