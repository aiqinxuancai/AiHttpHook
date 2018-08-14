#include "stdafx.h"
#include "DebugString.h"
#include "HookFun.h"
#include "UnicodeConv.h"
#include "HookNameToHost.h"

struct hostent * WINAPI New_gethostbyname(const char * name)
{
	//OutputDebugPrintf(L"AiHttpHook:New_gethostbyname:%s", name);
	struct hostent *remoteHost;
	remoteHost = OLD_gethostbyname(name);
	return remoteHost;
}



int PASCAL New_connect (SOCKET s, const struct sockaddr *name, int namelen)
{
	sockaddr_in sin;
	memcpy(&sin, &name, sizeof(sin));
	char * ip = new char[24];

	sprintf_s(ip,24, inet_ntoa(sin.sin_addr));
	//OutputDebugPrintf(L"AiHttpHook:New_connect:%s", ip);
	
	int ret = OLD_connect(s, name, namelen);
	return ret;
}