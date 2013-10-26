#include <winsock2.h>
#include <windows.h>
#include <psapi.h>
#include "HookHelper.h"
#include "Misc.h"
#define GSKey_Length 9
#pragma comment(lib,"ws2_32.lib")

struct HookStorage
{
	DWORD Entry;
	DWORD HookFunc;
	char OriginalStub[5];
	char FixedStub[5];
	CRITICAL_SECTION HookSection;
};

HookStorage WS2Connect;
/*HookStorage WS2Send;
HookStorage WS2Recv;
static SOCKET GSSocket = 0;
static unsigned char GSSeed;
static unsigned char GSKey[GSKey_Length];
static unsigned int GSSeqRX = 0;
static unsigned int GSSeqTX = 0;
*/


typedef int (__stdcall *ConnectEntry)(SOCKET, sockaddr *, int);

/*typedef int (__stdcall *SendEntry)(SOCKET, const char *, int, int);
typedef int (__stdcall *RecvEntry)(SOCKET, char *, int, int);

MODULEINFO mi;
*/

void SetupHook(HookStorage *Hook)
{
	DWORD dwProtect;
	BYTE asm_jmp = 0xE9;
	DWORD Offset = Hook->HookFunc - Hook->Entry - 5;
	InitializeCriticalSection(&Hook->HookSection);
	VirtualProtect((void*)Hook->Entry, 5, PAGE_EXECUTE_READWRITE, &dwProtect);
	memcpy(Hook->OriginalStub, (void*)Hook->Entry, 5);
	memcpy((void*)Hook->Entry, &asm_jmp, 1);
	memcpy((void*)(Hook->Entry + 1), &Offset, 4);
	memcpy(Hook->FixedStub, (void*)Hook->Entry, 5);
	VirtualProtect((void*)Hook->Entry, 5, dwProtect, &dwProtect);
}

void RestoreHook(HookStorage *Hook)
{
	DWORD dwProtect;
	VirtualProtect((void*)Hook->Entry, 5, PAGE_EXECUTE_READWRITE, &dwProtect);
	memcpy((void*)Hook->Entry, Hook->OriginalStub, 5);
	VirtualProtect((void*)Hook->Entry, 5, dwProtect, &dwProtect);
}

void ResetHook(HookStorage *Hook)
{
	DWORD dwProtect;
	VirtualProtect((void*)Hook->Entry, 5, PAGE_EXECUTE_READWRITE, &dwProtect);
	memcpy((void*)Hook->Entry, Hook->FixedStub, 5);
	VirtualProtect((void*)Hook->Entry, 5, dwProtect, &dwProtect);
}

int __stdcall FuncWS2Connect(SOCKET s, sockaddr *name, int namelen){
	int rtn;
	ConnectEntry Entry = (ConnectEntry)WS2Connect.Entry;
	EnterCriticalSection(&WS2Connect.HookSection);
	RestoreHook(&WS2Connect);
	rtn = (*Entry)(s, name, namelen);
	ResetHook(&WS2Connect);
	LeaveCriticalSection(&WS2Connect.HookSection);
	if (rtn == 0)
	{
		sockaddr_in Addr;
		int AddrLen = sizeof(sockaddr_in);
		getpeername(s, (sockaddr*)&Addr, &AddrLen);
		if (Addr.sin_port == htons(4000))
		{
			char nodelay = 1;
			int timeout = 4000;
			char GSSeed;
			char GSKey[GSKey_Length];
			char buf[10];
			unsigned char HSAuth[] = {0x13, 0x49, 0x03, 0xE8, 0x9C, 0x21, 0x65, 0x77, 0x8A, 0xBE};
			setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(char));
			//Connecting to D2GS; Handshake here
			int len = 0;
			while (len < 10)
			{
				int readlen = recv(s, &buf[len], 10 - len, 0);
				if (readlen > -1)
				{
					len += readlen;
				}
				else
				{
					return -1;
				}
			}
			GSSeed = buf[0];
			memcpy(GSKey, &buf[1], GSKey_Length);
			for (int i = 0; i < 10; i ++)
			{
				buf[i] = HSAuth[i] ^ (char)((GSKey[i % GSKey_Length] + i * i * GSSeed + 0x03) & 0xFF);
			}
			send(s, buf, 10, 0);
		}
	}
	return rtn;
}
/*
int __stdcall FuncWS2Send(SOCKET s, const char *buf, int len, int flags){
	int rtn;
	char* _buf = (char*)buf;
	SendEntry Entry = (SendEntry)WS2Send.Entry;
	if (s == GSSocket)
	{
		_buf = (char*)VirtualAlloc(0, len, MEM_COMMIT, PAGE_READWRITE);
		for (int i = 0; i < len; i ++)
		{
			_buf[i] = buf[i] ^ GSSeed; //(char)(GSKey[GSSeqTX % GSKey_Length] + GSSeqTX * GSSeqTX * GSSeed);
			GSSeqTX = (GSSeqTX + 1) & 0xFF;
		}
	}

	EnterCriticalSection(&WS2Send.HookSection);
	//RestoreHook(&WS2Send);
	rtn = (*Entry)(s, _buf, len, flags);
	//ResetHook(&WS2Send);
	LeaveCriticalSection(&WS2Send.HookSection);

	if (s == GSSocket)
	{
		VirtualFree(_buf, len, MEM_DECOMMIT);
	}
	return rtn;
}


int __stdcall FuncWS2Recv(SOCKET s, char *buf, int len, int flags){
	int rtn;
	RecvEntry Entry = (RecvEntry)WS2Recv.Entry;
	EnterCriticalSection(&WS2Recv.HookSection);
	//RestoreHook(&WS2Recv);
	rtn = (*Entry)(s, buf, len, flags);
	//ResetHook(&WS2Recv);
	LeaveCriticalSection(&WS2Recv.HookSection);
	
	if (s == GSSocket)
	{
		for (int i = 0; i < rtn; i ++)
		{
			buf[i] = buf[i] ^ GSSeed; //(char)(GSKey[GSSeqRX % GSKey_Length] + GSSeqRX * GSSeqRX * GSSeed);
			GSSeqRX = (GSSeqRX + 1) & 0xFF;
		}
	}

	return rtn;
}
*/
int HookInit()
{
/*
	char buf[255];

	DWORD oldAddress = (DWORD)GetModuleHandle("ws2_32.dll");
	char *newBase;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)oldAddress, &mi, sizeof(MODULEINFO));
	newBase = (char*)VirtualAlloc(NULL, mi.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(newBase, (void*)oldAddress, mi.SizeOfImage);
*/
	WS2Connect.Entry = (DWORD)GetProcAddress(GetModuleHandle("ws2_32.dll"), "connect");
	WS2Connect.HookFunc = (DWORD)FuncWS2Connect;
	SetupHook(&WS2Connect);
/*
	WS2Send.Entry = (DWORD)GetProcAddress(GetModuleHandle("wsock32.dll"), "send");
	WS2Send.HookFunc = (DWORD)FuncWS2Send;
	SetupHook(&WS2Send);

	WS2Recv.Entry = (DWORD)GetProcAddress(GetModuleHandle("wsock32.dll"), "recv");
	WS2Recv.HookFunc = (DWORD)FuncWS2Recv;
	SetupHook(&WS2Recv);
	
	WS2Send.Entry = WS2Send.Entry - oldAddress + (DWORD)newBase;
	WS2Recv.Entry = WS2Recv.Entry - oldAddress + (DWORD)newBase;
*/
/*
	char xbuf[32];
	itoa(WS2Send.Entry, xbuf);
	SetD2WindowText(xbuf);
*/
	return 0;
}