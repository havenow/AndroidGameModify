#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <jni.h>
#include <android/log.h>
#include <ctime>
#include <getopt.h>  
#include <time.h>

int g_socket = 0;
#define DEFAULT_PRESSURE 50

//./sendMsg -x 895 -y 1541 -c 10000 -t 1000		在坐标(895,1541) 周围+-50的地方 间隔0.1s点击一次 点击10000次
//./sendMsg -x 735 -y 1000 -c 100 -t 50000		点赞
//./sendMsg -x 540 -y 1123 -c 5000 -t 500		抢礼物（5s点击结束）	时间太短ui貌似会卡
int main(int argc, char** argv)
{
	struct option longopts[] = {
        {"x", 1, NULL, 'x'}, 
        {"y", 1, NULL, 'y'},  
		{"c", 1, NULL, 'c'},  //次数
        {"time", 1, NULL, 't'}, //间隔时间 毫秒
        {NULL, 0, NULL, 0},
    };
	
	int xIn = 0;
	int yIn = 0;
	int tIn = 0;
	int cIn = 1000;
	char *end;
	int optindex;
    /* process command line */
    while (true)
	{
		int c = getopt_long(argc, argv, "x:y:c:t:", longopts, &optindex);
		if(c == -1)
			break;
        switch (c) {
        case 'x':
            xIn = (int) strtoul(optarg, &end, 0);
			fprintf(stderr, "x: %d\n", xIn);
            break;
        case 'y':
           yIn = (int) strtoul(optarg, &end, 0);
		   fprintf(stderr, "y: %d\n", yIn);
		   break;
		case 'c':
           cIn = (int) strtoul(optarg, &end, 0);
		   fprintf(stderr, "c: %d\n", cIn);
		   break;
        case 't':
           tIn = (int) strtoul(optarg, &end, 0);
		   fprintf(stderr, "t: %d\n", tIn);
		   break;
        }
    }
	
	struct sockaddr_in addr;
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket < 0)
	{
		fprintf(stderr, "creating socket error\n");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET; //设置为IP通信  
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);

	if (connect(g_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		fprintf(stderr, "connect socket error %d\n", errno);
		close(g_socket);
		return 1;
	}
	
	
	for(int i=0; i< cIn; i++)
	{
		int contact = 0;
		int x = 895;
		int y = 1541;
		
		struct timeval tv;
		gettimeofday(&tv, 0);
		srand((unsigned)tv.tv_usec);
		int delta = rand();
		delta = rand()%100+1;//产生1~100之间的随机数
		x = xIn;
		y = yIn;
		x += delta;
		y += delta;
		
		char cmd[80] = { 0 };
		int io_length = snprintf(cmd, sizeof(cmd), "D %d %d %d %d\n", contact, x, y, DEFAULT_PRESSURE);
		//fprintf(stderr, "%s", cmd);
		send(g_socket, cmd, io_length, 0);
		io_length = snprintf(cmd, sizeof(cmd), "C\n");
		send(g_socket, cmd, io_length, 0);
		
		io_length = snprintf(cmd, sizeof(cmd), "U %d\n", contact);
		//fprintf(stderr, "%s", cmd);
		send(g_socket, cmd, io_length, 0);
		io_length = snprintf(cmd, sizeof(cmd), "C\n");
		send(g_socket, cmd, io_length, 0);
		
		usleep(tIn);//微秒
	}
	
	
	
	close(g_socket);
	
	return 0;
}
