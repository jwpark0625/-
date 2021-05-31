#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

//아래 정의는 편의를 위해 지정함. 삭제해도 무방합니다.
#define SERVER_IP "127.0.0.1"	//루프백 서버 주소 - 나중에 아래 코드랑 교체
//#define SERVER_IP "192.168.0.2"	//다중 사용 시험을 위한 실제 서버 주소
#define PORTNUM		8088		//포트번호 정의

void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr, clntAddr;

	char message[30] = "";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_DGRAM, 0);
	if (hSocket == INVALID_SOCKET)
		ErrorHandling("hSocket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.s_addr = inet_addr("3.37.104.232");
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servAddr.sin_port = htons(atoi(PORTNUM));
	
	//서버에 메세지 전송
	sendto(hSocket, message, sizeof(message), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
	
	//서버에서 연결 및 데이터 수신시 상대 peer에 대한 정보(clntAddr)를 받아야함


	//서버와의 소켓 연결 확인. 최종 완성시에 제거해도 무방
	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		ErrorHandling("connect() error!");
	else
		puts("연결 성공");




	//소켓 해제
	closesocket(hSocket);
	WSACleanup();

	return 0;
}

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
