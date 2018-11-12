#include "uart.h"
#include <stdio.h>
#include <cstring>
#include <iostream>

using namespace std;

//int Uart::MAX_BUFFER_LEN = 1024;

Uart::Uart(char* uart)
{
	//intialize the mutex
	pthread_mutex_init(&mutex, 0);

	//open the serial port
	Open(uart);
}

Uart::~Uart()
{
	if(fd >= 0)
	{
		close(fd);
		pthread_cancel(tid);
		pthread_mutex_destroy(&mutex);
	}
}

int Uart::Open(const char* uart)
{
	fd = open(uart, O_RDWR|O_NOCTTY);
	if(fd < 0)
		printf("open %s:%d failed\n", uart, fd);
	else
		printf("open serial port success\n");

	return fd;
}

int Uart::SetOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits )
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 57600:
			cfsetispeed(&newtio, B57600);
			cfsetospeed(&newtio, B57600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}

	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;
		newtio.c_cc[VTIME]  = 0;
		newtio.c_cc[VMIN] = 0;
		tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
	
	return 0;
}

int Uart::Init(int BaudRate, void (*CallBack)(char* data, int len))
{
	if(fd < 0)
	{
		printf("uart is not open\n");
		return -1;
	}

	//save the callback function
	printf("save the callback function\n");
	UartCallBack = CallBack;

	//apply the set option
	printf("apply the set option\n");
	int ret = SetOpt(fd, BaudRate, 8, 'N', 1);

	//start listening the serial port 
	printf("start listening the serial port\n");
	Listening();

	return ret;
}

int Uart::Write(char* data, int len)
{
	if(data == NULL || fd == -1)
		return -1;

	cout << "write " << len << " bytes: " << data << " to " << fd << endl;
	return write(fd, data, len);
}

void Uart::Listening()
{
	cout << "listening..." << tid << endl;
	pthread_mutex_lock(&mutex);
	int ret = pthread_create(&tid, 0, ReadingThread, this);
	cout << "ret" << ret << endl;
	//pthread_join(tid, NULL);
}

void* ReadingThread(void* arg)
{
	cout << "reading thread start" << endl;
	Uart* uart = (Uart*)arg;
	if(uart->fd < 0)
	{
		printf("can not read the serial port\n");
		uart->tid = -1;
		pthread_mutex_unlock(&(uart->mutex));
		return NULL;
	}
	else
	{
		printf("start reading serial port %d\n", uart->fd);
	}

	while(1)
	{
		//cout << "clear the receive buffer" << endl;
		memset(uart->RecvBuffer, 0, uart->MAX_BUFFER_LEN);

		//cout << "read the data" << endl;
		int recvBytes = read(uart->fd, uart->RecvBuffer, uart->MAX_BUFFER_LEN);
		if(recvBytes > 0)
		{
			cout << "recv data: " << uart->RecvBuffer << endl;
			cout << "call the callback function to process the data" << endl;
			uart->UartCallBack(uart->RecvBuffer, recvBytes);
		}
		else if(recvBytes < 0)
		{
			cout << "reset the thread id and fd, recvBytes " << recvBytes  << endl;
			uart->tid = -1;
			uart->fd = -1;
			pthread_mutex_unlock(&(uart->mutex));
			break;
		}
	}
}
