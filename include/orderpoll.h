#ifndef ORDER_POLL_H_
#define ORDER_POLL_H_
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <set>

#include "uart.h"
#include "httpclient.h"

#define IPADDRESS   "192.168.1.123"//"127.0.0.1"
#define PORT        8787
#define LISTENQ     5
#define FDSIZE      1000
#define MAXSIZE 	1024
#define EPOLLEVENTS 100
#define MASTER_CODE "AA00FF01"
#define UART_FILE	"/dev/ttyUSB0"
//#define UART_FILE	"/dev/ttySAC3"

class OrderPoll
{
public:
	OrderPoll();
	~OrderPoll();

	Uart uart;

private:
	//创建套接字并进行绑定
	int socket_bind(const char* ip,int port);
	//IO多路复用epoll
	void do_epoll();
	//事件处理函数
	void handle_events(int num);
	//处理接收到的连接
	void handle_accpet();
	//handle the uart data
	void handle_uart();
	//读处理
	void do_read(int fd);
	//写处理
	void do_write(int fd);
	//添加事件
	void add_event(int fd, int state);
	//修改事件
	void modify_event(int fd, int state);
	//删除事件
	void delete_event(int fd, int state);

private:
	HttpClient hc;
	int _listenfd;
	int _epollfd;
	int _pipefd;
	std::set<int> _clients;
	struct epoll_event _events[EPOLLEVENTS];
	char _buff[MAXSIZE];
};
#endif
