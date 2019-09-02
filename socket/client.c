#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>

#define SERV_PORT 8000

// argv[]里存的第一个默认是'filename'，第二个元素是输入的小端模式IP
int main(int argc, char *argv[])
{
	int sfd, len;
	struct sockaddr_in serv_addr;
	char buf[128];

	if (argc < 2){
		printf("./client serv_ip\n");
		return 1;
	}

	// IPv4, TCP
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	// 将argv[1]中的IP地址以网络序存到后面的参数里
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);

	connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	while(fgets(buf, sizeof(buf), stdin)) {
		write(sfd, buf, strlen(buf));	//sizeof会有一串乱码
		len = read(sfd, buf, sizeof(buf));	//为什么这里用sizeof，不用strlen
		write(STDOUT_FILENO, buf, len);
	}

	return 0;
}
