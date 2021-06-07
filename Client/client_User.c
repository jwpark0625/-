#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string.h>
#include <windows.h>

#define BUF_SIZE 30
#define PORT_SERVER 50000
#define PORT_ROBOT 50001
#define PORT_USER 50002

void ErrorHandling(char* message);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr, clntAddr;
    
    char message[BUF_SIZE];
    char myIP[BUF_SIZE], robotIP[BUF_SIZE];
    int ip_len;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("hSocket() error");

    // 클라이언트 소켓에 주소와 Port 번호 50002 초기화
    memset(&clntAddr, 0, sizeof(clntAddr));
    clntAddr.sin_family = AF_INET;
    clntAddr.sin_port = htons(PORT_USER);
    clntAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //3.37.104.232
    servAddr.sin_port = htons(atoi("50000"));

    // 유저의 Port 번호 할당
    if (bind(hSocket, (SOCKADDR*)&clntAddr, sizeof(clntAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    //handshake
    strcpy(message, "request_handshake");
    sendto(hSocket, message, sizeof(message), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
    // 서버로 부터 내 ip주소 받기
    ip_len = recv(hSocket, myIP, sizeof(myIP), 0);
    //myIP[ip_len] = 0;
    printf("My IP : %s\n", myIP);

    // 서버로 부터 robot ip주소 받기
    strcpy(message, "request_robotIP");
    sendto(hSocket, message, sizeof(message), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
    recv(hSocket, robotIP, sizeof(robotIP), 0);
    printf("Robot IP : %s\n", robotIP);

    //p2p 연결
 
    SOCKADDR_IN robotAddr;
    //로봇 연결 정보
    memset(&robotAddr, 0, sizeof(robotAddr));
    robotAddr.sin_family = AF_INET;
    robotAddr.sin_port = htons(PORT_ROBOT);
    robotAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int addr_size;
    addr_size = sizeof(robotAddr);

    /*keep listening to the data*/
    //테스트를 위한 채팅 시스템
    while (1)
    {
        /*B신호 대기*/
        if (recvfrom(hSocket, message, sizeof(message) / sizeof(message[0]), 0, (SOCKADDR*)&robotAddr, &addr_size) == -1)
        {
            ErrorHandling("Failed recieving message from Robot");
        }
        printf("Robot:%s\n", message);  //연결이 완료된 로봇에서 먼저 메세지가 옴
        /*peer A가 B에게 메세지 전송*/
        printf("User:");
        fgets(message, sizeof(message) / sizeof(message[0]), stdin);
        if (sendto(hSocket, message, sizeof(message) / sizeof(message[0]), 0, (SOCKADDR*)&robotAddr, sizeof(robotAddr)) == -1)
        {
            ErrorHandling("Failed sending message to Robot");
        }
    }


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
