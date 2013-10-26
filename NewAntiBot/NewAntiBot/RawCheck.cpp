#include <windows.h>
#include "PEHelper.h"
#include "RawCheck.h"
#include "Misc.h"
#include "Report.h"

//#define MAKE_DIFF

struct RawStorage
{
	char ImageName[64];
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS ntHeader;
	PIMAGE_SECTION_HEADER secHeader;
	HANDLE hModule;
	LPVOID bVImage;
};

struct WhiteListItem
{
	unsigned int Offset;
	char Len;
};

struct WhiteList
{
	int Count;
	WhiteListItem *List;
};

RawStorage D2Game;
RawStorage D2Client;
RawStorage D2Net;
WhiteList D2GameWL;
WhiteList D2NetWL;
WhiteList D2ClientWL;

WhiteListItem WLD2Game[] = {
	{152555, 6},
	{152693, 4},
	{161623, 5},
	{161997, 6},
	{169581, 1},
	{182131, 6},
	{196195, 4},
	{200936, 4},
	{278283, 6},
	{303061, 4},
	{383848, 1},
	{459737, 7},
	{460122, 3},
	{600153, 4},
	{785842, 4},
	{801873, 5},
	{801969, 5},
	{808577, 3},
	{813618, 5},
	{944978, 4},
	{945539, 5},
	{970512, 4}
};

WhiteListItem WLD2Net[] = {
	{27842, 8},
	{28232, 10}
};

WhiteListItem WLD2Client[] = {
	{0xBDFCF,6},
	{0x52A64,2},
	{0x52AAA,2},
	{0x521D1,6},
	{0x51410,5},
	{0x52310,6},
	{0x51624,7},
	{0x52223,6},
	{0x506AA,5},
	{0x35ED7,6},
	{0x32BB2,5},
	{0x9349F,2},
	{0x1BDFF,2},
	{0x933C6,15},
	{0x933D5,2},
	{0x4FE60,0x37},
	{0x76B66,6},
	{0x6324F,2},
	{0x78C83,5},
	{0x54ED3,7},
	{0x51BFD,6},
	{0x5200C,5},
	{0x51B28,5},
	{0x51FD3,5},
	{0x5214B,5},
	{0x51FE9,5},
	{0x51F64,6},
	{0x50EA8,0x3B},
	{0x510E6,6},
	{0x8279E,5},
	{0x8185B,5},
	{0x81945,5},
	{0x63B01,6},
	{0x79B72,6},
	{0x7EBC5,5},
	{0x79244,6},
	{0x7826F,5},
	{0x89326,5},
	{0x89BE3,5},
	{0x3DFC3,6},
	{0x89023,5},
	{0x88A04,6},
	{0x7D650,5},
	{0x77C50,5},
	{0x77BE2,5},
	{0x222FD,5},
	{0x1FF65,6},
	{0x52B9E,5},
	{0xA8DC9,6},
	{0x7D6A4,6},
	{0x7B8BB,6},
	{0x4CFD1,6},
	{0x4D07E,6},
	{0x63EBA,6},
	{0x66D5E,6},
	{0x4D233,6},
	{0xC2E2C,6},
	{0x148BD,6},
	{0x91D9C,5},
	{0x914AB,5},
	{0x24605,3},
	{0x24585,3},
	{0x32B40,7},
	{0x33883,5},
	{0x66EC5,7},
	{0x918B7,4},
	{0x916CF,4},
	{0x8A0BA,6},
	{0xB5DF3,5},
	{0xB5EDB,0x0B},
	{0xB5EE6,6},
	{0xB6D1B,5},
	{0xB6B8F,6},
	{0x167D8,5},
	{0x1654F,5},
	{0x94704,9},
	{0x39E96,5},
	{0x39FAF,5},
	{0x14835,1},
	{0x91E93,5},
	{0x14BFE,5},
	{0x91535,5},
	{0x83854,5},
	{0xDC86C,4},
	{91555, 5},
	{92125, 6},
	{129849, 8},
	{206832, 5},
	{336234, 5},
	{420775, 4},
	{783316, 6},
	{787628, 5},
	{89445, 2},
	{97655, 6},
	{126264, 1},
	{134456, 6},
	{139992, 4},
	{140426, 5},
	{202304, 5},
	{325152, 6},
	{334637, 1},
	{335879, 5},
	{336243, 11},
	{363167, 5},
	{365958, 8},
	{379319, 6},
	{383482, 6},
	{384499, 6},
	{404166, 6},
	{408160, 4},
	{409257, 4},
	{418779, 5},
	{421282, 4},
	{421361, 6},
	{439410, 7},
	{441760, 6},
	{458855, 4},
	{469053, 6},
	{472197, 6},
	{489076, 9},
	{490326, 4},
	{492515, 4},
	{492555, 4},
	{494346, 6},
	{505310, 6},
	{513467, 7},
	{521468, 6},
	{522245, 4},
	{523562, 4},
	{524615, 4},
	{527984, 4},
	{565430, 4},
	{593911, 5},
	{719056, 5},
	{779248, 5},
	{780720, 5},
	{781089, 6},
	{781168, 6},
	{782673, 9},
	{782740, 9},
	{783057, 9},
	{787633, 5},
	{789088, 5},
	{204985, 1},
	{335497, 5},
	{365988, 4},
	{379295, 4},
	{458854, 1},
	{211274, 2},
	{211290, 2},
	{225306, 2},
	{225321, 2},
	{328232, 1},
	{328647, 1},
	{420385, 12},
	{438442, 1},
	{766058, 1},
	{767839, 6},
	{767850, 1},
	{767903, 1},
	{4526, 2},
	{6046, 2},
	{335495, 2},
	{338862, 2},
	{398206, 2},
	{420561, 4},
	{204918, 5},
	{311908, 4},
	{384988, 4},
	{466788, 4},
	{466939, 4},
	{477628, 5},
	{477658, 6},
	{515939, 4},
	{515974, 4},
	{523273, 4},
	{523734, 4},
	{561753, 2},
	{561860, 8},
	{562746, 4},
	{562900, 4},
	{565993, 2},
	{566171, 5},
	{566238, 5},
	{567744, 5},
	{568436, 5},
	{573560, 4},
	{575202, 7},
	{597014, 5},
	{743437, 5},
	{747489, 5},
	{776178, 6},
	{786054, 4},
	{420697, 4},
	{436474, 5},
	{489331, 5},
	{494042, 5},
	{561590, 6},
	{561736, 5},
	{565593, 5},
	{565971, 5},
	{587582, 5},
	{744006, 5}
};

unsigned long GetAlignedSize(unsigned long Origin, unsigned long Alignment)   
{   
	return (Origin + Alignment - 1) / Alignment * Alignment;   
}
int CalcImageSize(RawStorage *s)
{
	int Size;
	int nAlign = s->ntHeader->OptionalHeader.SectionAlignment;
	Size = GetAlignedSize(s->ntHeader->OptionalHeader.SizeOfHeaders, nAlign);
	for(int i=0; i < s->ntHeader->FileHeader.NumberOfSections; ++i)
	{
		int CodeSize = s->secHeader[i].Misc.VirtualSize ;
		int LoadSize = s->secHeader[i].SizeOfRawData;
		int MaxSize = (LoadSize > CodeSize)?(LoadSize):(CodeSize);
		int SectionSize = GetAlignedSize(s->secHeader[i].VirtualAddress + MaxSize, nAlign);
		if(Size < SectionSize)
			Size = SectionSize;
	}
	return Size;
}

void CopyRawImage(RawStorage *s, void* pDest, void* pSrc)
{
	int HeaderSize = s->ntHeader->OptionalHeader.SizeOfHeaders;
	int SectionSize = s->ntHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	int MoveSize = HeaderSize + SectionSize;
	memcpy(pDest, pSrc, MoveSize);
	for(int i=0; i < s->ntHeader->FileHeader.NumberOfSections; ++i)
	{
		if(s->secHeader[i].VirtualAddress == 0 || s->secHeader[i].SizeOfRawData == 0) continue;
		void *pSectionAddress = (void *)((unsigned long)pDest + s->secHeader[i].VirtualAddress);
		memcpy((void *)pSectionAddress, (void *)((DWORD)pSrc + s->secHeader[i].PointerToRawData), s->secHeader[i].SizeOfRawData);
	}
	s->dosHeader = (PIMAGE_DOS_HEADER)pDest;
	s->ntHeader = (PIMAGE_NT_HEADERS)((int)pDest + (s->dosHeader->e_lfanew));
	s->secHeader = (PIMAGE_SECTION_HEADER)((int)s->ntHeader + sizeof(IMAGE_NT_HEADERS));
}

void LoadImage(RawStorage *s, char* ModuleName)
{
	HANDLE hFile = NULL;
	int slen = len(ModuleName) - 3;
	memcpy(s->ImageName, ModuleName, slen);
	s->ImageName[slen++] = 'l';
	s->ImageName[slen++] = 'o';
	s->ImageName[slen++] = 'g';
	s->ImageName[slen++] = 0;
	s->hModule = LoadLibrary(ModuleName);
	hFile = CreateFile(ModuleName, FILE_GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);   
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	DWORD dwNumberOfBytesRead = 0;
	LPVOID bRawImage = VirtualAlloc(NULL, dwFileSize, MEM_COMMIT, PAGE_READWRITE);
	if (!bRawImage)
	{
		return;
	}
	ReadFile(hFile, bRawImage, dwFileSize, &dwNumberOfBytesRead, NULL);
	CloseHandle(hFile);
	if (dwNumberOfBytesRead != dwFileSize)
	{
		return;
	}
	s->dosHeader = (PIMAGE_DOS_HEADER)bRawImage;
	s->ntHeader = (PIMAGE_NT_HEADERS)((DWORD)bRawImage + s->dosHeader->e_lfanew);
	s->secHeader = (PIMAGE_SECTION_HEADER)((DWORD)bRawImage + s->dosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	int VSize = CalcImageSize(s);
	s->bVImage = VirtualAlloc(NULL, VSize, MEM_COMMIT, PAGE_READWRITE);
	CopyRawImage(s, s->bVImage, bRawImage);
	VirtualFree(bRawImage, dwFileSize, MEM_DECOMMIT);
	FixRelocation(s->bVImage, (DWORD)s->hModule, s->ntHeader->OptionalHeader.ImageBase);
	FixRavAddr(s->bVImage, s->ntHeader->OptionalHeader.ImageBase);
}

int Interval(RawStorage *s, WhiteList *wl)
{
	PIMAGE_SECTION_HEADER h = s->secHeader;
#ifdef MAKE_DIFF
	HANDLE hFile = NULL;
	hFile = CreateFile(s->ImageName, FILE_GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
#endif
	int diff = 0;
	int i, j;
	DWORD dwProtect;
	for (i = 0; i < s->ntHeader->FileHeader.NumberOfSections; i ++)
	{
		if (h->VirtualAddress == 0 || h->SizeOfRawData == 0)
		{
			continue;
		}
		char* srcData = (char*)((DWORD)s->bVImage + h->VirtualAddress);
		char* dstData = (char*)((DWORD)s->hModule + h->VirtualAddress);
		VirtualProtect(dstData, h->SizeOfRawData, PAGE_EXECUTE_READWRITE, &dwProtect);
#ifdef MAKE_DIFF
		int xlen;
		int lastStart = 0;
		int lastEnd = 0;
		char buf[60];
		DWORD written;
#endif
		for (j = 0; j < h->SizeOfRawData; j ++)
		{
			if (j % 512 == 0)
			{
				Sleep(1);
			}
			if (srcData[j] != dstData[j])
			{
				unsigned int Offset = h->VirtualAddress + j;
				int found = 0;
				for(int k = 0; k < wl->Count; k ++)
				{
					if (Offset >= wl->List[k].Offset && Offset < wl->List[k].Offset + wl->List[k].Len)
					{
						found = 1;
						break;
					}
				}
				if (found == 0)
				{
#ifdef MAKE_DIFF
					if (lastEnd == Offset - 1)
					{
						lastEnd ++;
					}
					else
					{
						buf[0] = '{';
						itoa(lastStart, &buf[1]);
						xlen = len(buf);
						buf[xlen++] = ',';
						buf[xlen++] = ' ';
						itoa(lastEnd - lastStart + 1, &buf[xlen]);
						xlen = len(buf);
						buf[xlen++] = '}';
						buf[xlen++] = ',';
						buf[xlen++] = '\r';
						buf[xlen++] = '\n';
						buf[xlen++] = 0;
						WriteFile(hFile, buf, xlen-1, &written, NULL);
						SetFilePointer(hFile, 0, NULL, FILE_END);
						lastStart = lastEnd = Offset;
					}
#endif
					diff ++;
					//WE DONT RECOVER MEMORY NOW
					//dstData[j] = srcData[j];
				}
			}
		}
#ifdef MAKE_DIFF
		buf[0] = '{';
		itoa(lastStart, &buf[1]);
		xlen = len(buf);
		buf[xlen++] = ',';
		buf[xlen++] = ' ';
		itoa(lastEnd - lastStart + 1, &buf[xlen]);
		xlen = len(buf);
		buf[xlen++] = '}';
		buf[xlen++] = ',';
		buf[xlen++] = '\r';
		buf[xlen++] = '\n';
		buf[xlen++] = 0;
		WriteFile(hFile, buf, xlen-1, &written, NULL);
#endif
		
		VirtualProtect(dstData, h->SizeOfRawData, dwProtect, &dwProtect);
		h += sizeof(PIMAGE_SECTION_HEADER);
	}
#ifdef MAKE_DIFF
	CloseHandle(hFile);
#endif
	return diff;
}

int IScanInit()
{
	D2ClientWL.Count = 203;
	D2ClientWL.List = WLD2Client;
	D2NetWL.Count = 2;
	D2NetWL.List = WLD2Net;
	D2GameWL.Count = 22;
	D2GameWL.List = WLD2Game;
	LoadImage(&D2Client, "D2Client.Dll");
	LoadImage(&D2Game, "D2Game.Dll");
	LoadImage(&D2Net, "D2Net.Dll");
	return 0;
}


int IScanInterval()
{
	int diff = 0;
	diff += Interval(&D2Game, &D2GameWL);
	diff += Interval(&D2Net, &D2NetWL);

	//FIXME: We should not check .data & .rdata section
	diff += Interval(&D2Client, &D2ClientWL);
	if (diff > 64)
	{
		SendReport(diff);
		KillMe(1);
	}
	return 0;
}
