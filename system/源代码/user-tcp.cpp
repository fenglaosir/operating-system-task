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
//������Щ�˳��ǲ��Ե�ʱ���õ� 

void connect()
{
	WSADATA wsaData;
    char buff[1024];
    memset(buff, 0, sizeof(buff));
 
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("��ʼ��Winsockʧ��");
        exit(0);
    }
 
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8888);//�˿ں�
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//IP��ַ
 
    //�����׽���
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(SOCKET_ERROR == sock){
        printf("Socket() error:%d", WSAGetLastError());
        exit(0);
    }
	if(connect(sock, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET){
        printf("����ʧ��:%d", WSAGetLastError());
        exit(0);
    }
}

string user;

// �û����棨����û��Ҫ���̣߳����˿������ˣ� 
void sendt()
{
	while(true)
    {
    	//����ָ��ǰ׺ 
		recv(sock,buffer,sizeof(buffer),0);
    	cout<<buffer;
		char msg[1024];
		string ms;
		//����ָ�� 
    	getline(cin,ms);
    	strcpy(msg,ms.c_str());
    	send(sock,msg,sizeof(msg),0);
    	// ���շ��� 
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
// ��¼ 
void login()
{
	string username,password,bd;
	cout<<"�û���:";
	cin>>username;
	cout<<"����:";
	cin>>password;
	getline(cin,bd);
	char na[1024],pass[1024];
	strcpy(na,username.c_str());
	strcpy(pass,password.c_str());
	// �����û������� 
	send(sock,na,sizeof(na),0);
	Sleep(1);
	send(sock,pass,sizeof(pass),0);
	recv(sock,buffer,sizeof(buffer),0);
	string ren=buffer;
	if(ren=="ok")
	{
		cout<<"��¼�ɹ�\n";
		user=username;
	}
	else
	{
		cout<<"��¼ʧ��\n";
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
