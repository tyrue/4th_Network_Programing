#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING = "exit";			// 클라이언트의 종료 문자열
char *HELP_STRING = "help";			// 클라이언트의 도움말 문자열
char *IP_LIST_STRING = "iplist";		// 클라이언트의 ip리스트 문자열
char *TIME_LIST_STRING = "time";		// 클라이언트의 접속시간 리스트 문자열
char *ALL_MSG_COUNT_STRING = "count";		// 클라이언트의 총 메시지 수 문자열
char *START_STRING = "Connected to chat_server \n"; // 클라이언트 환영 메시지

int maxfdp1;						// 최대 소켓번호 + 1
int num_chat = 0;					// 채팅 참가자 수
int clisock_list[MAX_SOCK];			// 참가자 소켓번호 목록
char cliIP_list[MAX_SOCK][20];		// 참가자 IP목록
char cliTime_list[MAX_SOCK][25];	// 참가자 접속 시간 목록
char cliName_list[MAX_SOCK][20];	// 참가자 이름 목록
int listen_sock;					// 서버의 리슨 소켓

time_t timer;		// 시간측정 
struct tm *t;		// 시간 정보를 저장하는 구조체 
char time_m[20];	// 현재 시간

// 새로운 채팅 참가자 처리
void addClient(int s, struct sockaddr_in *newcliaddr);
int getmax();			// 최대 소켓번호 찾기
void removeClient(int s);	// 채팅 탈퇴 처리 함수
int tcp_listen(int host, int port, int backlog); // 소켓 생성 및 listen
void errquit(char *mesg) { perror(mesg); exit(1);}
void setTime(); 	// 현재 시간을 표시하는 함수
FILE *fp;		// 읽고 쓰는 모드로 파일을 만듦

int main(int argc, char *argv[])
{
	fp = fopen("log.txt", "w+");
	struct sockaddr_in cliaddr;
	char buf[MAXLINE + 1];
	int i, j, nbyte, accp_sock, addrlen = sizeof(struct sockaddr_in), msg_count = 0;
	fd_set read_fds; // 읽기를 감지할 fd_set 구조체
	char bufmsg[20];

	if(argc != 2)
	{
		printf("사용법 : %s port\n", argv[0]);
		exit(0);
	}

	if(fp == NULL)
	{
		printf("파일을 열지 못했습니다.\n");
		exit(1);
	}
	setTime();
	printf("%sServer start\n",time_m); 
	fprintf(fp, "%sServer start\n",time_m); 

	// tcp_listen(host, port, backlog) 함수 호출
	listen_sock = tcp_listen(INADDR_ANY, atoi(argv[1]), MAX_SOCK);
	while(1)
	{
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds); 
		FD_SET(0, &read_fds);			// 키보드 입력용 파일 디스크립터 세트
		for(i = 0; i < num_chat; i++)
			FD_SET(clisock_list[i], &read_fds);		
	
		maxfdp1 = getmax() + 1; // maxfdp1 재 계산
		if(select(maxfdp1, &read_fds, NULL, NULL, NULL) < 0)
			errquit("select fail");

		if(FD_ISSET(listen_sock, &read_fds)) // 새로운 참여자
		{
			accp_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &addrlen); // 클라이언트의 IP주소를 알 수 있다
			if(accp_sock == -1)
				errquit("accept fail");
			addClient(accp_sock, &cliaddr);
			send(accp_sock, START_STRING, strlen(START_STRING), 0); // 참여한 사용자에게 환영 메시지를 보낸다
		}

		if(FD_ISSET(0, &read_fds))
		{
			if(fgets(bufmsg, MAXLINE, stdin))
			{
				if(strstr(bufmsg, EXIT_STRING) != NULL)
				{
					setTime();
					printf("%sServer close\n", time_m);
					fprintf(fp,"%sServer close\n", time_m);
					fclose(fp);
					exit(0);
				}
				else if(strstr(bufmsg, HELP_STRING) != NULL)
				{
					puts("command list : exit - 서버 종료, help - 서버 명령어 목록, iplist - 접속자 IP주소 출력");
					puts("time - 접속자 접속시간 출력, count - 총 메시지의 수 출력");
				}
				else if(strstr(bufmsg, IP_LIST_STRING) != NULL)
				{
					printf("접속자 IP주소 출력\n");
					if(num_chat == 0) 
						printf("접속자가 없습니다.\n");
					else
					{
						for(int i = 0; i < num_chat; i++)
						{
							printf("[%d]%s : %s\n", i + 1, cliName_list[i], cliIP_list[i]);
						}
					}
				}

				else if(strstr(bufmsg, TIME_LIST_STRING) != NULL)
				{
					printf("접속자 접속 시간 출력\n");
					if(num_chat == 0) 
						printf("접속자가 없습니다.\n");
					else
					{
						for(int i = 0; i < num_chat; i++)
						{
							printf("[%d]%s : %s\n", i + 1, cliName_list[i], cliTime_list[i]);
						}
					}
				}

				else if(strstr(bufmsg, ALL_MSG_COUNT_STRING) != NULL)
				{
					printf("총 대화 수 : %d\n", msg_count);
				}
			}
		}
		// 클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
		for(i = 0; i < num_chat; i++)
		{
			if(FD_ISSET(clisock_list[i], &read_fds))
			{
				nbyte = recv(clisock_list[i], buf, MAXLINE, 0);
				if(nbyte <= 0)
				{
					removeClient(i); // 클라이언트의 종료
					continue;
				}
				buf[nbyte-1] = 0;	// 줄바꿈 문자 없앰

				// 종료문자 처리
				if(strstr(buf, EXIT_STRING) != NULL)
				{
					removeClient(i);
					continue;
				}
				setTime();	// 메시지를 보낸 시간
				char *msg;
				msg = strchr(buf, ']') + 1;
				strncpy(cliName_list[i], buf, nbyte - strlen(msg) - 1);
	
				// 메시지와 메시지의 문자수를 출력
				printf("%s(%s)%s : %s \t[문자수 : %ld]\n",time_m, cliIP_list[i], cliName_list[i], msg, strlen(msg)); 
				fprintf(fp, "%s(%s)%s : %s \t[문자수 : %ld]\n",time_m, cliIP_list[i], cliName_list[i], msg, strlen(msg)); 
				// 모든 채팅 참가자에게 메시지 방송
				for(j = 0; j < num_chat; j++)
					send(clisock_list[j], buf, nbyte, 0);	

				msg_count++;
			}
		}
	}	// end of while
	fclose(fp);
	return 0;
}

// 새로운 채팅 참가자 관리
void addClient(int s, struct sockaddr_in *newcliaddr)
{
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr -> sin_addr, buf, sizeof(buf));
	setTime();

	// 채팅 클라이언트 목록에 추가
	clisock_list[num_chat] = s;
	strcpy(cliIP_list[num_chat], buf);
	strcpy(cliTime_list[num_chat], time_m);

	num_chat++;
	printf("%snew client: %s, %d번째 사용자 추가.\n", time_m, buf, num_chat);
	fprintf(fp, "%snew client: %s, %d번째 사용자 추가.\n", time_m, buf, num_chat);	
}

void removeClient(int s)
{
	close(clisock_list[s]);
	setTime();
	if(strcmp(cliName_list[s], "") == 0)
	{
		printf("%s채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", time_m, num_chat - 1);
		fprintf(fp, "%s채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n", time_m, num_chat - 1);
	}
	else
	{
		printf("%s채팅 참가자 %s 탈퇴. 현재 참가자 수 = %d\n", time_m, cliName_list[s], num_chat - 1);
		fprintf(fp, "%s채팅 참가자 %s 탈퇴. 현재 참가자 수 = %d\n", time_m, cliName_list[s], num_chat - 1);
	}

	for(int i = s; i < num_chat; i++)
	{
		clisock_list[i] = clisock_list[i + 1];
		strcpy(cliIP_list[i], cliIP_list[i + 1]);
		strcpy(cliName_list[i], cliName_list[i + 1]);
		strcpy(cliTime_list[i], cliTime_list[i + 1]);
	}
	num_chat--;
}

// 최대 소캣번호 찾기
int getmax()
{
	// Minimun 소켓 번호는 가장 먼저 생성된 listen_sock
	int max = listen_sock;
	int i;
	for(i = 0; i < num_chat; i++)
		if(clisock_list[i] > max)
			max = clisock_list[i];
	return max;
}

// listen 소켓 생성 및 listen
int tcp_listen(int host, int port, int backlog)
{
	int sd;
	struct sockaddr_in servaddr;

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd == -1)
	{
		perror("socket fail");
		exit(1);
	}
	// servaddr 구조체의 내용 세팅
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(host);
	servaddr.sin_port = htons(port);
	if(bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind fail"); 
		exit(1);
	}
	// 클라이언트로부터 연결 요청을 기다림
	listen(sd, backlog);
	return sd;
}

void setTime()
{
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	sprintf(time_m, "<%d-%d-%d %d:%d:%d> ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}


