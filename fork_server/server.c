#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

#include "wrap.h"

#define SERV_PORT 8000

void do_sig(int num) {
	// 不断地回收子进程
	while (waitpid(0, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[]) {
	// lfd是listen文件描述符
	int lfd, cfd, len, i;
	int serv_port = SERV_PORT;
	char buf[1024], client_ip[128];
	struct sockaddr_in serv_addr, client_addr;
	socklen_t client_len;
	pid_t pid;

	// sigaction结构体
	struct sigaction act;
	act.sa_handler = do_sig;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction(SIGCHLD, &act, NULL);

	// argv[1]存放着自定义的端口号，由于默认是字符串，需要转成int类型
	if (argc == 2) {
		serv_port = atoi(argv[1]); // 字符串转int
	}

	lfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	// 将int强制转换为short，因为htons接收的是16位short，防止报错。
	serv_addr.sin_port = htons((short)serv_port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	Listen(lfd, 128);

	printf("wait for connecting...\n");

	while(1){
		client_len = sizeof(client_addr);
		cfd = Accept(lfd, (struct sockaddr *)&client_addr, &client_len);
		// 在服务器端打印所连接的IP信息
		printf("client IP:%s\t%d\n",
				inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip,sizeof(client_ip)), 
				ntohs(client_addr.sin_port));
		pid = fork();
		if (pid == 0){
			// in child
			// 不再需要lfd
			Close(lfd);
			while(1){
				len = Read(cfd, buf, sizeof(buf));
				if (len <= 0)
					break;
				Write(STDOUT_FILENO, buf, len);
				for(i = 0; i < len; i++)
					buf[i] = toupper(buf[i]);
				Write(cfd, buf,len);
			}
			Close(cfd);
			return 0;
		} else if (pid > 0) {
			// in parent
			// 不再需要cfd，继续创建新的套接字描述符
			Close(cfd);
		} else {
			perror("fork");
			exit(1);
		}

	}

	Close(lfd);

	return 0;
}
