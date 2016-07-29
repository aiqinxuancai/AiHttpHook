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

BOOL CHttpRecv::SetCallback(PVOID _proc)
{
	//typedef int(FUNC1)(int);  
	//int (*func)(int,int);  
	m_callback = (DataCallback)_proc;
	return TRUE;
}

BOOL CHttpRecv::Call(const char * file, string & data, const char * send, const char * serverName) //, DWORD _data_len
{
	//为了避免其他程序读到非0x00结尾 所以先copy一下
	int len = data.length() + 2;
	char * dataUse = new char[len];
	memset(dataUse, 0, len);
	memcpy(dataUse, data.c_str(), data.length());
	m_callback(file, dataUse, send, serverName);
	delete[]dataUse;
	return TRUE;
}


DWORD CHttpRecv::GetLocaleTimestampI()
{
	__time32_t long_time;
	_time32(&long_time);
	return long_time;
}

void CHttpRecv::StartId(DWORD _id, wstring & _file)
{
	if (IsIdExist(_id) == false)
	{
		CPackData data;
		data.id = _id;
		data.file = _file;
		data.time = GetLocaleTimestampI();
		data.serverName = GetServerName(_id);
		//OutputDebugPrintf(L"AiHttpHook:StartId:%d,%s",data.id, data.file.c_str());
		EnterCriticalSection(&m_cs);
		m_data.push_back(data);
		LeaveCriticalSection(&m_cs);  
	}
}

void CHttpRecv::AddServerName(DWORD _id, wstring & _serverName)
{
	IsServerNameIdExist(_id); //清除之前有的_id
	CPackServerName data;
	data.id = _id;
	data.serverName.append(_serverName);
	//EnterCriticalSection(&m_cs);
	//m_serverName.push_back(data);
	//LeaveCriticalSection(&m_cs); 

	//OutputDebugPrintf(L"AiHttpHook:AddServerName:(%d)%d -> %s", m_serverName.size(), data.id, data.serverName);
}

wstring CHttpRecv::GetServerName(DWORD _id)
{
	EnterCriticalSection(&m_cs);
	//OutputDebugPrintf(L"AiHttpHook:GetServerNameId:%d", _id);
	for(PackServerNameList::iterator it = m_serverName.begin(); it != m_serverName.end();)
	{
		if (it->id == _id)
		{
			wstring name = it->serverName;
			m_serverName.erase(it);
			LeaveCriticalSection(&m_cs);  
			//OutputDebugPrintf(L"AiHttpHook:GetServerName:%s", name);
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


bool CHttpRecv::IsServerNameIdExist(DWORD _id)
{
	EnterCriticalSection(&m_cs);
	for(PackServerNameList::iterator it = m_serverName.begin(); it != m_serverName.end();)
	{
		if (it->id == _id)
		{
			//OutputDebugPrintf(L"AiHttpHook:IsServerNameIdExist:Delete:%d -> %s", _id, it->serverName.c_str() );
			m_serverName.erase(it);
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

void CHttpRecv::PushData(DWORD _id, string & _data)
{
	EnterCriticalSection(&m_cs);
	for (int i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == _id)
		{
			m_data[i].dat = m_data[i].dat + _data;
			//m_data[i].dat.append(_data);
		}
	}
	LeaveCriticalSection(&m_cs);  
}

void CHttpRecv::PushSendData(DWORD _id, string & _data)
{
	EnterCriticalSection(&m_cs);
	for (int i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == _id)
		{
			m_data[i].send = _data;
		}
	}
	LeaveCriticalSection(&m_cs);  
}

bool CHttpRecv::IsIdExist(DWORD _id)
{
	EnterCriticalSection(&m_cs);
	for (int i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == _id)
		{
			LeaveCriticalSection(&m_cs);  
			return true;
		}
	}
	LeaveCriticalSection(&m_cs);  
	return false;
}

string CHttpRecv::CloseId(DWORD _id, wstring & _file, string & _send, wstring & _serverName)
{
	string dat = "";
	EnterCriticalSection(&m_cs);
	for(PackList::iterator it = m_data.begin(); it != m_data.end();)
	{
		if (it->id == _id)
		{
			dat = it->dat;
			_file = it->file;
			_send = it->send;
			_serverName = it->serverName;
			if (_file.length() > 128)
			{
				_file = L"LongData";
			}
			//OutputDebugPrintf(L"AiHttpHook:CloseId1：%s",_file.c_str());
			//OutputDebugPrintf("AiHttpHook:CloseId2：%s",_send.c_str());
			//OutputDebugPrintf("AiHttpHook:CloseId3：%s",dat.c_str());
			//dat.append(0x00);

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
	//OutputDebugPrintf(L"AiHttpHook:CheckIdTime:%d", time);
	//OutputDebugPrintf(L"AiHttpHook:CheckIdTimeCount:%d", m_data.size());
	EnterCriticalSection(&m_cs);
	for (int i = 0; i < m_data.size(); i++)
	{
		if (time - m_data[i].time > 12) //>12sec
		{
			//OutputDebugPrintf(L"AiHttpHook:CheckIdTimeOutClose:%d,%s", m_data[i].id, m_data[i].file);
			m_data[i].end = TRUE;
			Call(ConvertUnicodeToMultiBytes(m_data[i].file).c_str(), m_data[i].dat, m_data[i].send.c_str(),ConvertUnicodeToMultiBytes(m_data[i].serverName).c_str());
		}
	}

	for(PackList::iterator it = m_data.begin(); it != m_data.end();)
	{
		if (it->end)
		{
			//OutputDebugPrintf(L"AiHttpHook:CheckIdTimeOutErase:%d,%s", it->id, it->file.c_str());
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

CPackServerName::CPackServerName(void)
{

}

CPackServerName::~CPackServerName(void)
{

}
