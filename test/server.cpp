#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>			/* socket*/
#include <sys/epoll.h>			/* epoll */
#include <fcntl.h>			/* nonblocking */
#include <sys/resource.h>		/* set the max connection needs setrlimit */

#include <set>
#include <iterator>
#include <iostream>

using namespace std;
 
#define	MAXEPOLL	1000
#define	MAXLINE		1024
#define PORT		6000
#define	MAXBACK		1000
 
set<int> gLocalClients;

//set nonblock
int setnonblocking( int fd )
{
	if( fcntl( fd, F_SETFL, fcntl( fd, F_GETFD, 0 )|O_NONBLOCK ) == -1 )
	{
		printf("Set blocking error : %d\n", errno);
		return -1;
	}
	return 0;
}

//send data to all local clients
void SendToClients(char* data, int len)
{
	set<int>::iterator iter = gLocalClients.begin();
	for(;iter != gLocalClients.end(); iter++)
	{
		int sock = *iter;
		write(sock, data, len);
	}
}
 
int main( int argc, char ** argv )
{
	int 		listen_fd;
	int 		conn_fd;
	int 		epoll_fd;
	int 		nread;
	int 		cur_fds;				//!> 当前已经存在的数量
	int 		wait_fds;				//!> epoll_wait 的返回值
	int		i;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	struct 	epoll_event	ev;
	struct 	epoll_event	evs[MAXEPOLL];
	struct 	rlimit	rlt;		//!> 设置连接数所需
	char 	buf[MAXLINE];
	socklen_t	len = sizeof( struct sockaddr_in );
 
	//set the max fd of every process
	rlt.rlim_max = rlt.rlim_cur = MAXEPOLL;
	//if( setrlimit( RLIMIT_NOFILE, &rlt ) == -1 )	
	//{
	//	printf("Setrlimit Error : %d\n", errno);
	//	exit( EXIT_FAILURE );
	//}
	
	//server
	bzero( &servaddr, sizeof( servaddr ) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	servaddr.sin_port = htons( PORT );
	
	//create socket
	if( ( listen_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		printf("Socket Error...\n" , errno );
		exit( EXIT_FAILURE );
	}
	
	if( setnonblocking( listen_fd ) == -1 )
	{
		printf("Setnonblocking Error : %d\n", errno);
		exit( EXIT_FAILURE );
	}
	
	if( bind( listen_fd, ( struct sockaddr *)&servaddr, sizeof( struct sockaddr ) ) == -1 )
	{
		printf("Bind Error : %d\n", errno);
		exit( EXIT_FAILURE );
	}
 
	if( listen( listen_fd, MAXBACK ) == -1 )
	{
		printf("Listen Error : %d\n", errno);
		exit( EXIT_FAILURE );
	}

	printf("Create server success!\n");

	//open a named pipe for UART transmit
	const char *fifo_name = "./uart";
	if(access(fifo_name, F_OK) != -1)
	{
		printf("pipe file already exits\n");
	}
	else if(mkfifo(fifo_name, 0777) == -1)
	{
		printf("Make uart fifo error: %d\n", errno);
		exit( EXIT_FAILURE );
	}
	int pipe_fd = -1;
	pipe_fd = open(fifo_name, O_RDONLY | O_NONBLOCK);
	
	// create epoll 
	epoll_fd = epoll_create( MAXEPOLL );	// create
	ev.events = EPOLLIN | EPOLLET;		// accept Read!
	ev.data.fd = listen_fd;		 	// add listen_fd
	if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev ) < 0 )
	{
		printf("Epoll Error : %d\n", errno);
		exit( EXIT_FAILURE );
	}
	cur_fds = 1;

	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = pipe_fd;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pipe_fd, &ev) < 0)
	{
		printf("Epoll error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	cur_fds++;

	while( 1 )
	{
		if( ( wait_fds = epoll_wait( epoll_fd, evs, cur_fds, -1 ) ) == -1 )
		{
			printf( "Epoll Wait Error : %d\n", errno );
			exit( EXIT_FAILURE );
		}
 
		for( i = 0; i < wait_fds; i++ )
		{
			/*printf("evnets: %d\n", evs[i].events);
			printf("EPOLLIN %d\n", EPOLLIN);
			printf("EPOLLOUT %d\n", EPOLLOUT);
			printf("EPOLLET %d\n", EPOLLET);*/
			if( evs[i].data.fd == listen_fd && cur_fds < MAXEPOLL )	
			{
				if( ( conn_fd = accept( listen_fd, (struct sockaddr *)&cliaddr, &len ) ) == -1 )
				{
					printf("Accept Error : %d\n", errno);
					exit( EXIT_FAILURE );
				}
				
				printf( "Server get from client %d!\n", conn_fd/*,  inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port */);
				//add to local clients set
				gLocalClients.insert(conn_fd);

				ev.events = EPOLLIN | EPOLLET;		//accept Read!
				ev.data.fd = conn_fd;			//add conn_fd
				if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev ) < 0 )
				{
					printf("Epoll Error : %d\n", errno);
					exit( EXIT_FAILURE );
				}
				++cur_fds; 
				continue;		
			}
			else if(evs[i].data.fd == pipe_fd)
			{
				nread = read(pipe_fd, buf, sizeof(buf));
				if(nread < 0)
				{
					close(pipe_fd);
					printf("read UART fifo failed\n");
				}
				else
				{
					buf[nread] = 0;
					printf("Read from UART: %s\n", buf);
					SendToClients(buf, nread);
				}
				continue;
			}
			
			//recv data
			if(evs[i].events & EPOLLIN)
			{
				nread = read( evs[i].data.fd, buf, sizeof( buf ) );
				if( nread <= 0 )						//error
				{
					close( evs[i].data.fd );
					printf("close client %d\n", evs[i].data.fd);
					epoll_ctl( epoll_fd, EPOLL_CTL_DEL, evs[i].data.fd, &ev );	//delete the fd
					gLocalClients.erase(evs[i].data.fd);		//delete in the set

					--cur_fds;					//reduce the total counts
					continue;
				}
				cout << buf << endl;					//print on the screen
				write( evs[i].data.fd, buf, nread );			//send back
			}
			else if(evs[i].events & EPOLLERR)
			{
				int err_fd = evs[i].data.fd;
				close(err_fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, err_fd, &ev);
				gLocalClients.erase(err_fd);
			}
		}
	}
	
	close( listen_fd );
	close( epoll_fd );
	return 0;
}
