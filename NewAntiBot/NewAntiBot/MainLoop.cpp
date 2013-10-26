#include <windows.h>
#include <stdlib.h>
#include "Misc.h"
#include "MainLoop.h"
#include "RawCheck.h"
#include "ScanProcess.h"
#include "HookHelper.h"

void PatchD2()
{
	DWORD protect;
	unsigned char C91 = 0x91;
	void* Patch3A = (void*)0x6FF34710;
	BOOL prot = VirtualProtect(Patch3A, 1, PAGE_EXECUTE_READWRITE, &protect);
	if (prot)
	{
		memcpy(Patch3A, &C91, 1);
		VirtualProtect(Patch3A, 1, protect, &protect);
	}
}

DWORD WINAPI AntiBotLoop(LPVOID lpParam)
{
	//Finally, we can do things here
	PatchD2();
	IScanInit();
	HookInit();
	Sleep(20000);
	while (IScanInterval() == 0)
	{
		ScanQMarco();
		Sleep(30000);
	}
	return 0;
}

DWORD WINAPI ChildMain(LPVOID lpParam)
{
	DWORD lpBaseOffset = (DWORD)lpParam;
	CreateThread(NULL, 0, AntiBotLoop, NULL, 0, NULL);
	return 0;
}
