#ifndef UART_H_
#define UART_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
///#include <stdio.h>

//const char* UART3 = "/dev/ttySAC3";

void* ReadingThread(void* arg);

class Uart
{
public:
	int fd;
	pthread_t tid;
	pthread_mutex_t mutex;
	static const int MAX_BUFFER_LEN = 1024;
	char RecvBuffer[MAX_BUFFER_LEN];

	void (*UartCallBack)(char*, int);

	Uart(char* uart);

	~Uart();

	int Open(const char* uart);

	int Init(int BaudRate, void (*CallBack)(char* data, int len));
	
	int Write(char* send_buffer, int len);

private:
	void Listening();
	int SetOpt(int fd, int BaudRate, int DataBit, char Parity, int StopBit);
};
#endif
