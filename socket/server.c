#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<strings.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>

#define SERV_PORT 8000

int main(void)
{
	int sfd, cfd, len, i;
	struct sockaddr_in serv_addr, client_addr;
	char buf[128], client_ip[128];
	socklen_t addr_len;

	// AF_INET:ipv4	SOCK_STREAM:流协议	0:默认协议(tcp,udp)
	sfd	= socket(AF_INET, SOCK_STREAM, 0);

	// 初始化服务器地址
	bzero(&serv_addr, sizeof(serv_addr));

	// 结构体serv_addr
	serv_addr.sin_family = AF_INET;
	// 网络字节序，端口号是16位，所以用htons(short)
	serv_addr.sin_port = htons(SERV_PORT);
	// INADDR_ANY主机所有IP,IPv4是32位，所以用htonl(long)
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// 服务器能接收并发链接的能力
	// 这里的backlog:128，指的是在同时处在三次握手的链接数
	listen(sfd, 128);

	printf("wait for connecting...\n");
	
	//初始化长度
	addr_len = sizeof(client_addr);
	// 阻塞，等待客户端链接，成功则返回新的文件描述符，用于和客户端通信
	// 这里的&client_addr是传出参数，&addr_len是传入传出参数
	cfd = accept(sfd, (struct sockaddr *)&client_addr, &addr_len);

	// 打印客户端IP
	printf("client IP:%s\t%d\n",
		   	inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip)), 
			ntohs(client_addr.sin_port));

	// 阻塞接收客户端数据
	while(1){
		len = read(cfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, len);

		// 处理业务(小写转大写功能实现，调用现成的函数)
		for (i = 0; i < len; i++)
			buf[i] = toupper(buf[i]);
		// 返回给客户端结果
		write(cfd, buf, len);
	}
	
	close(cfd);
	close(sfd);

	return 0;
}
