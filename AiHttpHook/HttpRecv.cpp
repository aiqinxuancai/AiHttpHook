#include "stdafx.h"
#include "HttpRecv.h"
#include "UnicodeConv.h"
#include "DebugString.h"
#include <time.h>


// request max wait time
CONST int REQUESTTIMEOUT = 30;

CHttpRecv::CHttpRecv(void)
{
	InitializeCriticalSection(&m_cs);
}

CHttpRecv::~CHttpRecv(void)
{
}

BOOL CHttpRecv::SetCallback(PVOID callback)
{
	m_callback = (DataCallback)callback;
	return TRUE;
}

BOOL CHttpRecv::Call(const char * path, 
	string & responseData, 
	const char * postData, 
	const char * hostName, 
	const char * responseHeaders) //, DWORD _data_len
{
	//为了避免其他程序读到非0x00结尾 所以先copy一下
	size_t len = responseData.length() + 2;
	char * dataUse = new char[len];
	memset(dataUse, 0, len);
	memcpy(dataUse, responseData.c_str(), responseData.length());

	m_callback(path, dataUse, postData, hostName, responseHeaders);
	delete[]dataUse;
	return TRUE;
}


DWORD CHttpRecv::GetLocaleTimestampI()
{
	__time32_t long_time;
	_time32(&long_time);
	return long_time;
}

void CHttpRecv::StartId(HINTERNET id, HINTERNET connectId, wstring & path)
{
	if (IsIdExist(id) == false)
	{
		CPackData data;
		data.connectId = connectId;
		data.id = id;
		data.path = path;
		data.startTime = GetLocaleTimestampI();
		data.hostName = GetHostName(connectId);

		PrintFDbg(L"AiHttpHookHostName#StartId#:%d - %d - %s", connectId, id, data.hostName.c_str());

		EnterCriticalSection(&m_cs);
		m_data.push_back(data);
		LeaveCriticalSection(&m_cs);  
	}
}

void CHttpRecv::SetHostName(HINTERNET id, wstring & hostName)
{
	EnterCriticalSection(&m_cs);
	m_mapHostName[id] = hostName;
	//PrintFDbg(L"AiHttpHookHostNameCount:%d", m_mapHostName.size());
	LeaveCriticalSection(&m_cs);
}

wstring CHttpRecv::GetHostName(HINTERNET id)
{
	EnterCriticalSection(&m_cs);
	if (m_mapHostName.count(id))
	{
		wstring host = m_mapHostName[id];
		LeaveCriticalSection(&m_cs);
		return host;
	} 
	LeaveCriticalSection(&m_cs);
	return L"";
}

void CHttpRecv::PushData(HINTERNET id, string & data)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			m_data[i].responseData = m_data[i].responseData + data;
		}
	}
	LeaveCriticalSection(&m_cs);  
}

void CHttpRecv::SetPostData(HINTERNET id, string & data)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			m_data[i].postData = data;
		}
	}
	LeaveCriticalSection(&m_cs);  
}

void CHttpRecv::SetResponseHeaders(HINTERNET id, string & responseHeaders)
{
	EnterCriticalSection(&m_cs);
	for (UINT i = 0; i < m_data.size(); i++)
	{
		if (m_data[i].id == id)
		{
			PrintFDbg(L"AiHttpHook#2:%s", string(responseHeaders).c_str());
			m_data[i].responseHeaders = responseHeaders;
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

void CHttpRecv::CloseRequestAndCall(HINTERNET id)
{
	wstring path = L"";
	wstring hostName = L"";
	string postData = "";
	string requestHeaders = "";
	string retByte = CHttpRecv::Instance().CloseId((HINTERNET)id, path, postData, hostName, requestHeaders);

	if (retByte.empty() == false)
	{
		CHttpRecv::Instance().CheckIdTime();
		CHttpRecv::Instance().Call(ConvertUnicodeToMultiBytes(path).c_str(),
			retByte,
			postData.c_str(),
			ConvertUnicodeToMultiBytes(hostName).c_str(),
			requestHeaders.c_str()); //发送到Call一个完整的包
	}
}


string CHttpRecv::CloseId(HINTERNET id, wstring & path, string & postData, wstring & hostName, string & responseHeaders)
{
	string dat = "";
	EnterCriticalSection(&m_cs);
	for(PackList::iterator it = m_data.begin(); it != m_data.end();)
	{
		if (it->id == id)
		{
			dat = it->responseData;
			path = it->path;
			postData = it->postData;
			hostName = it->hostName;
			responseHeaders = it->responseHeaders;
			//if (path.length() > 128)
			//{
			//	path = L"LongData";
			//}

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
		if (time - m_data[i].startTime > REQUESTTIMEOUT) //>12sec
		{
			m_data[i].isEnd = TRUE;
			Call(ConvertUnicodeToMultiBytes(m_data[i].path).c_str(), 
				m_data[i].responseData, 
				m_data[i].postData.c_str(),
				ConvertUnicodeToMultiBytes(m_data[i].hostName).c_str(),
				m_data[i].responseHeaders.c_str());
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