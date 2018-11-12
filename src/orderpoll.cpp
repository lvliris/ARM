#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <vector>

#include "orderpoll.h"
#include "converter.h"

using namespace std;

const char* UART_PIPE = "./config/uart";

vector<string> split(string &s, const string &seperator)
{
	vector<string> result;
	string::size_type pos = s.find(seperator);
	while(pos != string::npos)
	{
		string ss = s.substr(0, pos);
		result.push_back(ss);
		s = s.substr(pos + 1);
		pos = s.find(seperator);
	}
	result.push_back(s);

	return result;
}


static void uartcallback(char* data, int len)
{
	int fd = open(UART_PIPE, O_RDONLY | O_NONBLOCK);
	write(fd, data, len);
	close(fd);
}


static size_t httpcallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	rapidjson::Document res;
	res.Parse((char*)ptr);

	Uart* uart = (Uart*)stream;

	//std::cout << "http callback\nresult: " << res["result"].GetString() << endl;
	if(res["result"] == "none")
		return nmemb;

	string orders((char*)res["result"].GetString());
	vector<string> order = split(orders, "|");
	for(int i = 0; i < order.size(); i++)
	{
		cout << order[i] << endl;
		rapidjson::Document d;
		Str2Json((char*)order[i].c_str(), d);
		char temp[1024] = {0};
		DocumentSerialize(d, temp, 1024);
		cout << temp << endl;
		ParseOrder(temp, *uart);
	}
	
	return nmemb;
}


OrderPoll::OrderPoll():
	uart(UART_FILE),
	hc(MASTER_CODE)
{
	//create listen fd
	_listenfd = socket_bind(IPADDRESS, PORT);
	listen(_listenfd, LISTENQ);

	//open the pipe
	if(access(UART_PIPE, F_OK) != -1)
    {
       	printf("pipe file already exits\n");
    }
    else if(mkfifo(UART_PIPE, 0777) == -1)
    {
       	printf("Make uart fifo error: %d\n", errno);
       	exit( EXIT_FAILURE );
    }

	_pipefd = open(UART_PIPE, O_RDONLY | O_NONBLOCK);
	cout << "open uart pipe success" << endl;

	//set uart
	uart.Init(57600, uartcallback);

	//set http client
	string url = "http://101.201.211.87:8080/zfzn02/servlet/ElectricOrderServlet?masterCode=";
	hc.SetURL((char*)url.c_str());
	hc.SetCallBack(httpcallback, &uart);
	hc.GetOrder();

	//start polling
	do_epoll();
}


OrderPoll::~OrderPoll()
{
	close(_listenfd);
}


int OrderPoll::socket_bind(const char* ip,int port)
{
    int  listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET,SOCK_STREAM,0);
    if (listenfd == -1)
    {
        perror("socket error:");
        exit(1);
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&servaddr.sin_addr);
    servaddr.sin_port = htons(port);
    if (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
    {
        perror("bind error: ");
        exit(1);
    }
    return listenfd;
}


void OrderPoll::do_epoll()
{
    int ret;

    memset(_buff, 0, MAXSIZE);
    //create a listening fd
    _epollfd = epoll_create(FDSIZE);
    //add listen clients event
    add_event(_listenfd, EPOLLIN);
	//add listen uart event
    add_event(_pipefd, EPOLLIN);
    for ( ; ; )
    {
        //get ready events
        ret = epoll_wait(_epollfd, _events, EPOLLEVENTS, -1);
        handle_events(ret);
    }
    close(_epollfd);
}


void OrderPoll::handle_events(int num)
{
    int i;
    int fd;
	int ev;
    //进行选好遍历
    for (i = 0;i < num;i++)
    {
        fd = _events[i].data.fd;
		ev = _events[i].events;
        //根据描述符的类型和事件类型进行处理
        if ((fd == _listenfd) && (ev & EPOLLIN))
		{
            handle_accpet();
		}
		else if ((fd == _pipefd) && (ev & EPOLLIN))
		{
            handle_uart();
		}
        else if (ev & EPOLLIN)
		{
            do_read(fd);
		}
        else if (_events[i].events & EPOLLOUT)
		{
            do_write(fd);
		}
    }
}


void OrderPoll::handle_accpet()
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t  cliaddrlen = sizeof(struct sockaddr_in);
    clifd = accept(_listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    if (clifd == -1)
        perror("accpet error:");
    else
    {
        printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
        //添加一个客户描述符和事件
        add_event(clifd, EPOLLIN);
		_clients.insert(clifd);
    }
}


void OrderPoll::handle_uart()
{
	int nread;
	nread = read(_pipefd, _buff, MAXSIZE);
	if(nread < 0)
	{
		close(_pipefd);
		printf("read UART fifo failed\n");
	}
	else
	{
		_buff[nread] = 0;
		//send to all clients
		std::set<int>::iterator iter = _clients.begin();
		for(; iter != _clients.end(); iter++)
		{
			int sock = *iter;
			write(sock, _buff, nread);
		}

		//send to server
		hc.SendState(_buff);
		cout << "UART: " << _buff << endl;

		rapidjson::Document d;
		if(Str2Json(_buff, d) > 0)
		{
			char temp[1024] = {0};
			DocumentSerialize(d, temp, 1024);
			ParseOrder(temp, uart);
        	//modify_event(fd, EPOLLOUT);
		}
	}
}


void OrderPoll::do_read(int fd)
{
    int nread;
    nread = read(fd, _buff, MAXSIZE);
    if (nread == -1)
    {
        perror("read error");
        close(fd);
        delete_event(fd, EPOLLIN);
		_clients.erase(fd);
    }
    else if (nread == 0)
    {
		//fprintf(stderr,"client close.\n");
        printf("client close.\n");
        close(fd);
        delete_event(fd, EPOLLIN);
		_clients.erase(fd);
    }
    else
    {
        printf("read message is : %s",_buff);
        //修改描述符对应的事件，由读改为写
		// TODO:process socket data
		rapidjson::Document d;
		if(Str2Json(_buff, d) > 0)
		{
			char temp[1024] = {0};
			DocumentSerialize(d, temp, 1024);
			ParseOrder(temp, uart);
        	//modify_event(fd, EPOLLOUT);
		}
    }
}

                 
void OrderPoll::do_write(int fd)
{
    int nwrite;
    nwrite = write(fd, _buff, strlen(_buff));
    if (nwrite == -1)
    {
        perror("write error:");
        close(fd);
        delete_event(fd, EPOLLOUT);
		_clients.erase(fd);
    }
    else
        modify_event(fd, EPOLLIN);

    memset(_buff, 0, MAXSIZE);
}


void OrderPoll::add_event(int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev);
}


void OrderPoll::delete_event(int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, &ev);
}


void OrderPoll::modify_event(int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev);
}
