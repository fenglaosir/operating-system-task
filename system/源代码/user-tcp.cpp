#include<bits/stdc++.h>
#include "winsock2.h"
 #include<cstdlib>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

SOCKET sock;
char buffer[1024];

/*void rec()
{
	while (true)
    {
		recv(sock,buffer,sizeof(buffer),0);
        cout  << buffer <<"\n";
    }
}

void startrec()
{
	thread recd(rec);
	recd.join();
}*/
//上面这些八成是测试的时候用的 

void connect()
{
	WSADATA wsaData;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
 
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("初始化Winsock失败");
        exit(0);
    }
 
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8888);//端口号
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//IP地址
 
    //创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(SOCKET_ERROR == sock){
        printf("Socket() error:%d", WSAGetLastError());
        exit(0);
    }
	if(connect(sock, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET){
        printf("连接失败:%d", WSAGetLastError());
        exit(0);
    }
}

string user;

// 用户界面（好像没必要开线程？算了开都开了） 
void sendt()
{
	while(true)
    {
    	//接收指令前缀 
		recv(sock,buffer,sizeof(buffer),0);
    	cout<<buffer;
		char msg[1024];
		string ms;
		//发送指令 
    	getline(cin,ms);
    	strcpy(msg,ms.c_str());
    	send(sock,msg,sizeof(msg),0);
    	// 接收反馈 
    	recv(sock,buffer,sizeof(buffer),0);
		string res=buffer;
        if(res!="") cout  << res <<"\n";
	}
}

void startsend()
{
	thread sted(sendt);
	sted.join();
}
// 登录 
void login()
{
	string username,password,bd;
	cout<<"用户名:";
	cin>>username;
	cout<<"密码:";
	cin>>password;
	getline(cin,bd);
	char na[1024],pass[1024];
	strcpy(na,username.c_str());
	strcpy(pass,password.c_str());
	// 发送用户名密码 
	send(sock,na,sizeof(na),0);
	Sleep(1);
	send(sock,pass,sizeof(pass),0);
	recv(sock,buffer,sizeof(buffer),0);
	string ren=buffer;
	if(ren=="ok")
	{
		cout<<"登录成功\n";
		user=username;
	}
	else
	{
		cout<<"登录失败\n";
		system("pause");
		exit(1);
	 }
}

int main()
{
	connect();
	login();
	startsend();
}
