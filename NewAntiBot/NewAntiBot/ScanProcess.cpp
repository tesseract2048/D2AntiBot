#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <io.h>
#include "ScanProcess.h"
#include "Misc.h"
#include "Report.h"

#pragma comment(lib,"psapi.lib")

BOOL CheckProcess(){
	char ModulePath[MAX_PATH];
	PROCESSENTRY32 ME32;
	HANDLE hProcessSnap;
	int ProcLoop;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	ME32.dwSize = sizeof(ME32);
	ProcLoop = Process32First(hProcessSnap, &ME32);
	while(ProcLoop){
		ProcLoop = Process32Next(hProcessSnap, &ME32);
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ME32.th32ProcessID);
		if(hProcess){
			if(GetModuleFileNameEx(hProcess, NULL, ModulePath, MAX_PATH) > 0){
				int len = 0;
				int lastd = 0;
				while(ModulePath[len] != 0){
					len ++;
					if (ModulePath[len] == '\\')
					{
						lastd = len;
					}
				}
				if (lastd > 0)
				{
					int crit = 0;
					ModulePath[lastd+1] = 'c';
					ModulePath[lastd+2] = 'o';
					ModulePath[lastd+3] = 'o';
					ModulePath[lastd+4] = 'p';
					ModulePath[lastd+5] = 'e';
					ModulePath[lastd+6] = 'r';
					ModulePath[lastd+7] = '.';
					ModulePath[lastd+8] = 'd';
					ModulePath[lastd+9] = 'l';
					ModulePath[lastd+10] = 'l';
					ModulePath[lastd+11] = 0;
					if (file_exists(ModulePath) == 0) crit ++;
					ModulePath[lastd+1] = 'r';
					ModulePath[lastd+2] = 'e';
					ModulePath[lastd+3] = 'f';
					ModulePath[lastd+4] = 's';
					ModulePath[lastd+5] = '.';
					ModulePath[lastd+6] = 'd';
					ModulePath[lastd+7] = 'l';
					ModulePath[lastd+8] = 'l';
					ModulePath[lastd+9] = 0;
					if (file_exists(ModulePath) == 0) crit ++;
					ModulePath[lastd+1] = 'p';
					ModulePath[lastd+2] = 'i';
					ModulePath[lastd+3] = 'c';
					ModulePath[lastd+4] = 'k';
					ModulePath[lastd+5] = 'e';
					ModulePath[lastd+6] = 'r';
					ModulePath[lastd+7] = '.';
					ModulePath[lastd+8] = 'e';
					ModulePath[lastd+9] = 'x';
					ModulePath[lastd+10] = 'e';
					ModulePath[lastd+11] = 0;
					if (file_exists(ModulePath) == 0) crit ++;
					if (crit >= 2)
					{
						SendReport(256);
						KillMe(2);
					}
				}
			}
			CloseHandle(hProcess);
		}
		Sleep(1);
	}
	CloseHandle(hProcessSnap);
	return TRUE;
}

void CheckModule()
{
	MODULEENTRY32 ME32;
	HANDLE hProcessSnap;
	int ProcLoop;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	ME32.dwSize = sizeof(ME32);
	ProcLoop = Module32First(hProcessSnap, &ME32);
	while(ProcLoop){
		ProcLoop = Module32Next(hProcessSnap, &ME32);
		char ModulePath[MAX_PATH];
		int len = GetModuleFileName(ME32.hModule, ModulePath, MAX_PATH);
		if(len != 0){
			for(int i = len - 1; i >= 0; i --)
			{
				if (ModulePath[i] == '\\')
				{
					if (cmpi(&ModulePath[i+1], "cfgdll.dll") == 0)
					{
						KillMe(3);
					}
					break;
				}
			}
		}
	}
	CloseHandle(hProcessSnap);
}

void ScanQMarco()
{
	CheckModule();
	CheckProcess();
	//KillMe();
}