#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	const char* fifo_name = "../config/uart";
	int pipe_fd = open(fifo_name, O_WRONLY | O_NONBLOCK);
	char buff[1024] = { 0 };
	while(cin >> buff)
	{
		sleep(1);
		write(pipe_fd, buff, strlen(buff));
		memset(buff, 0, 1024);
		//write(pipe_fd, "test", 4);
	}

	//getchar();

	close(pipe_fd);
}
