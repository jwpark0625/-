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
    char myIP[BUF_SIZE], userIP[BUF_SIZE];
    int ip_len;
    
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling("hSocket() error");

    // 클라이언트 소켓 Port 번호 50001 초기화
    memset(&clntAddr, 0, sizeof(clntAddr));
    clntAddr.sin_family = AF_INET;
    clntAddr.sin_port = htons(PORT_ROBOT);
    clntAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //추가됨

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //원래 주소 --> "3.37.104.232"
    servAddr.sin_port = htons(atoi("50000"));

    // Port 번호 할당
    if (bind(hSocket, (SOCKADDR*)&clntAddr, sizeof(clntAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    // handshake
    strcpy(message, "request_handshake");
    sendto(hSocket, message, sizeof(message), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));

    // 서버로 부터 내 ip주소 받기
    ip_len = recv(hSocket, myIP, sizeof(myIP), 0);
    //myIP[ip_len] = 0;
    printf("My IP : %s\n", myIP);

    while (1) {
        Sleep(3000);
        strcpy(message, "request_list");
        sendto(hSocket, message, sizeof(message), 0, (SOCKADDR*)&servAddr, sizeof(servAddr));
        recv(hSocket, message, sizeof(message), 0);
        if (!strcmp(message, "wait")) {   // 유저 접속 전
            printf("%s\n", message);
           continue;
        }
        else {   // 유저 접속 완료, 유저 ip 주소 받기
            strcpy(userIP, message);
            //userIP[ip_len] = 0;
            printf("User IP : %s\n", userIP);
           break;
       }
    }
    

     //p2p 연결

    SOCKADDR_IN userAddr;
    //유저 연결 정보
    memset(&userAddr, 0, sizeof(userAddr));
    userAddr.sin_family = AF_INET;
    userAddr.sin_port = htons(PORT_USER);
    userAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int addr_size;
    addr_size = sizeof(userAddr);

    /*keep listening to the data*/
    //테스트를 위한 채팅 시스템
    while (1)
    {
        //유저에게서 연결 확인 -> 확인차 메세지 전송
        printf("Robot:");
        fgets(message, sizeof(message) / sizeof(message[0]), stdin);
        if (sendto(hSocket, (char*)message, sizeof(message) / sizeof(message[0]), 0, (SOCKADDR*)&userAddr, sizeof(userAddr)) == -1)
        {
            ErrorHandling("Failed sending message to User");
        }
        /*Wait for the peer A to sent the data*/
        if (recvfrom(hSocket, (char*)message, sizeof(message) / sizeof(message[0]), 0, (struct sockaddr*)&userAddr, &addr_size) == -1)
        {
            ErrorHandling("Failed recieving message from User");
        }
        printf("User:%s\n", message);
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
