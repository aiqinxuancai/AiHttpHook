#include "stdafx.h"

bool HookAPI(PVOID *ppPointer,PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	LONG ret = DetourAttach(ppPointer, pDetour);
	DetourTransactionCommit();
	return ret;
}
