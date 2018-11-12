#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#include <iostream>

#define addr "192.168.1.123"
#define port 8787

using namespace std;

int main(int argc, char **argv)
{
	int cfd;
	int recbyte;
	int sin_size;
	char buffer[1024] = {0};
	struct sockaddr_in s_add, c_add;
	unsigned short portnum = 0x8787;
	
	printf("Hello,welcome to client!\r\n");

	/* 创建一个TCP 连接的socket */
	cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == cfd)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\r\n");

	/* 变量s_add 清零*/
	bzero(&s_add,sizeof(struct sockaddr_in));
	s_add.sin_family=AF_INET;
	s_add.sin_addr.s_addr= inet_addr(addr);
	s_add.sin_port=htons(port);
	printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);
	/* 连接服务器函数*/
	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
	{
		printf("connect fail !\r\n");
		return -1;
	}
	printf("connect ok !\r\n");

	while(1)
	{
		char buff[1024] = { 0 };
		cin.getline(buff, 1024);
		if(buff[0] == '/')
			continue;

		if(strlen(buff) == 0)
			break;

		cout << buff << endl;

		write(cfd, buff, strlen(buff));
		sleep(1);
		/* 接收服务器发过来的数据*/
		/*if(-1 == (recbyte = read(cfd, buffer, 1024)))
		{
			printf("read data fail !\r\n");
			return -1;
		}
		printf("read ok\r\nREC:\r\n");
		buffer[recbyte]='\0';
		printf("%s\r\n",buffer);*/
	}
	
	/* 关闭套接字*/
	close(cfd);
	return 0;
}
