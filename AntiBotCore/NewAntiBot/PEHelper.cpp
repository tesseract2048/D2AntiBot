#include <windows.h>
#include "Misc.h"
#include "PEHelper.h"

void FixRelocation(void* pDest, DWORD NewImageBase, DWORD ImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pDest;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((int)pDest + (pDosHeader->e_lfanew));
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((int)pNTHeader + sizeof(IMAGE_NT_HEADERS));
	DWORD Delta = NewImageBase - ImageBase;
	PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)((unsigned long)pDest + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	while((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0)
	{
		WORD *pLocData = (WORD *)((int)pLoc + sizeof(IMAGE_BASE_RELOCATION));
		int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
		for( int i=0 ; i < NumberOfReloc; i++)
		{
			if( (DWORD)(pLocData[i] & 0xF000) == 0x00003000)
			{
				DWORD * pAddress = (DWORD *)((unsigned long)pDest + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				*pAddress += Delta;
			}
		}
		pLoc = (PIMAGE_BASE_RELOCATION)((DWORD)pLoc + pLoc->SizeOfBlock);
	}
}

BOOL FixRavAddr(void* pDest, DWORD ImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pDest;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((int)pDest + (pDosHeader->e_lfanew));
	PIMAGE_SECTION_HEADER pSectionHeader = (PIMAGE_SECTION_HEADER)((int)pNTHeader + sizeof(IMAGE_NT_HEADERS));
	unsigned long Offset = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	int i;
	if(Offset == 0) return TRUE;
	PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR)((unsigned long)pDest + Offset);
	while(pID->Characteristics != 0 )
	{
		PIMAGE_THUNK_DATA pRealIAT = (PIMAGE_THUNK_DATA)((unsigned long)pDest + pID->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalIAT = (PIMAGE_THUNK_DATA)((unsigned long)pDest + pID->OriginalFirstThunk);
		char buf[256]; //DLL name;
		BYTE* pName = (BYTE*)((unsigned long)pDest + pID->Name);
		for(i=0;i<256;i++)
		{
			if(pName[i] == 0) break;
			buf[i] = pName[i];
		}
		buf[i] = 0;
		HMODULE hDLL = LoadLibrary(buf);
		for(i=0; ;i++)
		{
			if(pOriginalIAT[i].u1.Function == 0) break;
			FARPROC lpFunction = NULL;
			if(pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				lpFunction = GetProcAddress(hDLL, (LPCSTR)(pOriginalIAT[i].u1.Ordinal & 0x0000FFFF));
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)
					((DWORD)pDest + (DWORD)(pOriginalIAT[i].u1.AddressOfData));
				lpFunction = GetProcAddress(hDLL, (char *)pByName->Name);
			}
			if(lpFunction != NULL)
			{
				pRealIAT[i].u1.Function = (DWORD)lpFunction;
			}
			else
			{
				return FALSE;
			}
		}
		pID = (PIMAGE_IMPORT_DESCRIPTOR)( (DWORD)pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	return TRUE;
}