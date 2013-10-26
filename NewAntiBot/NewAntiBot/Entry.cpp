#include <windows.h>
#include <psapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tlhelp32.h>
#include "Entry.h"
#include "Misc.h"
#include "PEHelper.h"
#include "HookHelper.h"
#include "MainLoop.h"
#include "ScanProcess.h"

#pragma comment(lib,"psapi.lib")

static const char* REG_D2 = "Software\\Blizzard Entertainment\\Diablo II";
static const char* REG_SILENT = "91_SILENT";
static const char* REG_POPUP = "91_POPUP";
static const char* REG_GUID = "91_GUID";
char EQN_HASHKEY[16];

static HMODULE instance;

BOOL HashEQN(char *buffer, char const *eqn){
	int buf;
	int i;
	for(i = 0; i < (int)len(eqn) ; i ++){
		buf = (eqn[i] - 65) ^ (EQN_HASHKEY[i] - 65);
		if(buf > 25) buf = buf + 10;
		buffer[i] = buf + 65;
	}
	buffer[i] = 0;
	return TRUE;
}

BOOL DoMutex()
{
	char MutexName[255];
	MutexName[0] = 'D';
	MutexName[1] = '2';
	MutexName[2] = '_';
	MutexName[3] = 'A';
	MutexName[4] = 'n';
	MutexName[5] = 't';
	MutexName[6] = 'i';
	MutexName[7] = '_';
	MutexName[8] = 'B';
	MutexName[9] = 'o';
	MutexName[10] = 't';
	MutexName[11] = '_';
	itoa(GetCurrentProcessId(), &MutexName[12]);
	HANDLE hMutex = CreateMutex(NULL, FALSE, MutexName);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		return FALSE;
	}
	return TRUE;
}

void Birth()
{
	if (DoMutex() == FALSE)
	{
		return;
	}
	void *lpNewBase = NULL;
	DWORD dwThreadId;
	MODULEINFO mi;
	GetModuleInformation(GetCurrentProcess(), instance, &mi, sizeof(MODULEINFO));
	lpNewBase = VirtualAlloc(NULL, mi.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(lpNewBase, mi.lpBaseOfDll, mi.SizeOfImage);
	FixRelocation(lpNewBase, (DWORD)lpNewBase, (DWORD)mi.lpBaseOfDll);
	FixRavAddr(lpNewBase, (DWORD)mi.lpBaseOfDll);
	DWORD NewChild = (DWORD)ChildMain - (DWORD)mi.lpBaseOfDll + (DWORD)lpNewBase;
	
	HANDLE th = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)((DWORD)ChildMain - (DWORD)mi.lpBaseOfDll + (DWORD)lpNewBase), (LPVOID)((DWORD)lpNewBase - (DWORD)mi.lpBaseOfDll), 0, &dwThreadId);

	//Disable FreeLibrary
	//VirtualProtect((void*)0x6FF2CDBB, 6, PAGE_EXECUTE_READWRITE, NULL);
	//memset((void*)0x6FF2CDBB, 0x90, 6);
}


BOOL WINAPI DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		instance = hModule;
		EQN_HASHKEY[0] = 'E';
		EQN_HASHKEY[1] = 'L';
		EQN_HASHKEY[2] = 'B';
		EQN_HASHKEY[3] = 'I';
		EQN_HASHKEY[4] = 'S';
		EQN_HASHKEY[5] = 'N';
		EQN_HASHKEY[6] = 'T';
		EQN_HASHKEY[7] = 'G';
		EQN_HASHKEY[8] = 'C';
		EQN_HASHKEY[9] = 'K';
		EQN_HASHKEY[10] = 'R';
		EQN_HASHKEY[11] = 'U';
		EQN_HASHKEY[12] = 'W';
		EQN_HASHKEY[13] = 'R';
		EQN_HASHKEY[14] = 'F';
		EQN_HASHKEY[15] = 0;
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

BOOL EnterNotice(){
	char buf[255];
	QueryReg(buf, REG_D2, REG_SILENT);
	if(cmp(buf, "YES")!=0){
		if(MessageBox(0, "欢迎进入91D2暗黑1.11b战网。\r\n本战网禁止除maphack外的任何外挂程序，任何违规软件的使用将被记录。\r\n\r\n游戏前请仔细阅读BBS中的战网管理条例和免责相关条款。\r\n点击“是”表示您同意上述规则，点击“否”将退出战网。\r\n\r\n战网论坛：http://bbs.91d2.cn/", "提示", MB_YESNO) == IDYES){
			SetReg(REG_D2, REG_SILENT,"YES");
			return true;
		}else{
			return false;
		}
	}else{
		return true;
	}
}

BOOL WINAPI CheckRevision(char file1[], char file2[], char file3[], unsigned char eqnstr[], unsigned int *versionid, int *checksum, char exeinfo[])
{
	if (!EnterNotice())
	{
		return FALSE;
	}
	Birth();
	*versionid = 37330;
	*checksum = 37330;
	HashEQN(exeinfo, (char*)eqnstr);
	return TRUE;
}
