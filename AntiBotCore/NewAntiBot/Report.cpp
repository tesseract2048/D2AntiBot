#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include "Misc.h"
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI SendReportThread(LPVOID lpParam)
{
	int diff = (int)lpParam;
	SOCKET sockid;
	struct sockaddr_in srv_addr;
	if ((sockid = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET)
		return -1;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.S_un.S_addr = inet_addr("211.144.76.86");
	srv_addr.sin_port = htons(80);
	if (connect(sockid,(struct sockaddr*)&srv_addr,sizeof(struct sockaddr_in)) == SOCKET_ERROR)
		return -1;
	char postdata[16];
	memset(postdata, 0, 16);
	itoa(diff, postdata);
	for (int i = 0; i < 3; i ++)
	{
		postdata[i] = postdata[i] ^ ((i + 1) * 3);
	}
	char* data = "POST /ab_report.php HTTP/1.1\nHost: bbs.91d2.cn\nUser-Agent:\nAntiBot 0.2\nContent-Type: antibot/report\nContent-Length: 16\n\n";
	send(sockid, data, len(data), 0);
	send(sockid, postdata, 16, 0);
}

int SendReport(int diff)
{
	CreateThread(NULL, 0, SendReportThread, (LPVOID)diff, 0, NULL);
	return 0;
}