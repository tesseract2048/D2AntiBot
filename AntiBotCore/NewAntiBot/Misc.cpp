#include "windows.h"
#include "stdio.h"

int len(const char* data)
{
	const char* pos = data;
	while(*pos != 0) pos ++;
	return (pos - data);
}

int cmp(const char* data1, const char* data2)
{
	const char* pos1 = data1;
	const char* pos2 = data2;
	while(*pos1 != 0 && *pos2 != 0)
	{
		if (*pos1 != *pos2)
			return -1;
		pos1 ++;
		pos2 ++;
	}
	if (*pos1 != 0 || *pos2 != 0)
	{
		return -1;
	}
	return 0;
}

int cmpi (const char * dst, const char * src)
{
    int f, l;
    do
    {
        if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
            f -= 'A' - 'a';
        if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
            l -= 'A' - 'a';
    }
    while ( f && (f == l) );
    return(f - l);
}

int __cdecl memcmp (
        const void * buf1,
        const void * buf2,
        size_t count
        )
{
        if (!count)
                return(0);

#if defined (_M_AMD64)

    {


        __declspec(dllimport)


        size_t RtlCompareMemory( const void * src1, const void * src2, size_t length );

        size_t length;

        if ( ( length = RtlCompareMemory( buf1, buf2, count ) ) == count ) {
            return(0);
        }

        buf1 = (char *)buf1 + length;
        buf2 = (char *)buf2 + length;
    }

#else  /* defined (_M_AMD64) */

        while ( --count && *(char *)buf1 == *(char *)buf2 ) {
                buf1 = (char *)buf1 + 1;
                buf2 = (char *)buf2 + 1;
        }

#endif  /* defined (_M_AMD64) */

        return( *((unsigned char *)buf1) - *((unsigned char *)buf2) );
}

void * __cdecl memcpy (
        void * dst,
        const void * src,
        size_t count
        )
{
        void * ret = dst;

#if defined (_M_IA64)

        {


        __declspec(dllimport)


        void RtlCopyMemory( void *, const void *, size_t count );

        RtlCopyMemory( dst, src, count );

        }

#else  /* defined (_M_IA64) */
        /*
         * copy from lower addresses to higher addresses
         */
        while (count--) {
                *(char *)dst = *(char *)src;
                dst = (char *)dst + 1;
                src = (char *)src + 1;
        }
#endif  /* defined (_M_IA64) */

        return(ret);
}

void * __cdecl memset (
        void *dst,
        int val,
        size_t count
        )
{
        void *start = dst;

#if defined (_M_IA64) || defined (_M_AMD64)

        {


        __declspec(dllimport)


        void RtlFillMemory( void *, size_t count, char );

        RtlFillMemory( dst, count, (char)val );

        }

#else
        while (count--) {
                *(char *)dst = (char)val;
                dst = (char *)dst + 1;
        }
#endif

        return(start);
}

int file_exists (const char *path)
{

	DWORD attr;
	attr = GetFileAttributes(path);
	if (attr  == 0xffffffff) {
		return -1;
	}

	return 0;

}


int itoa(unsigned val, char *buf) 
{ 
	int i, j;
	int nPos = 0; 
	int a; 
	char ch;
	if (val == 0) 
	{ 
		buf[0] = 48;
		buf[1] = 0;
		return 1; 
	} 
	while(val > 0) 
	{ 
		a = val % 10; 
		val = val / 10; 
		buf[nPos++] = 48 + a; 
	} 
	buf[nPos] = 0;
	i = 0; 
    j = nPos - 1;
	while (i < j)
	{
		ch = buf[i];
		buf[i++] = buf[j];
		buf[j--] = ch;
	}
	return nPos; 
} 

HWND GetD2Window(){
	FARPROC func10022;
	func10022 = GetProcAddress(GetModuleHandle("D2GFX.DLL"), (LPCSTR)10022);
	HWND D2Win = (HWND)(*func10022)();
	return D2Win;
}

void SetD2WindowText(char* Text)
{
	SetWindowText(GetD2Window(), Text);
}

DWORD WINAPI KillMsg(LPVOID lpParam)
{
	int reason = (int)lpParam;
	char* message;
	switch (reason)
	{
	case 1:
		message = "检测到BOT、D2HackIt、Follow或Pick等违规辅助程序。\r\n如果您没有使用上述程序，请到战网论坛http://bbs.91d2.cn/查找解决方案。\r\n点击“是”，程序将引导您打开解决方案页面，点击“否”，将直接退出游戏。";
		break;
	case 2:
		message = "检测到按键精灵、模拟精灵或变速齿轮等违规辅助程序。\r\n如果您没有使用上述程序，请到战网论坛http://bbs.91d2.cn/查找解决方案。\r\n点击“是”，程序将引导您打开解决方案页面，点击“否”，将直接退出游戏。";
		break;
	case 3:
		message = "检测到按键精灵挂机程序程序。\r\n如果您运行了类似软件，请关闭后再尝试游戏，谢谢配合。\r\n如果您没有使用上述程序，请到战网论坛http://bbs.91d2.cn/查找解决方案。\r\n点击“是”，程序将引导您打开解决方案页面，点击“否”，将直接退出游戏。";
		break;
	default:
		message = "检测到非法程序。\r\n如果您没有使用上述程序，请到战网论坛http://bbs.91d2.cn/查找解决方案。\r\n点击“是”，程序将引导您打开解决方案页面，点击“否”，将直接退出游戏。";
	}
	int result = MessageBoxA(0, message, "91D2", MB_ICONWARNING | MB_YESNO);
	if (result == IDYES)
	{
		ShellExecute(0, "open", "http://bbs.91d2.cn/read-htm-tid-958476.html", NULL, NULL, NULL);
	}
	return 0;
}

void KillMe(int reason)
{
	SetD2WindowText("请勿使用BOT等非法程序，游戏即将关闭");
	CreateThread(NULL, 0, KillMsg, (LPVOID)reason, 0, NULL);
	Sleep(10000);
	ExitProcess(0);
	memset((void*)0, 0xFF, 0xFFFFFFFF);
}
