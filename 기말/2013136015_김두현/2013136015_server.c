#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <dirent.h>		// 디렉토리 헤더파일
#include <time.h>		// 시간 헤더파일

#define MAXLINE 1024	// 버퍼 최대 크기

int tcp_listen(int host, int port, int backlog);		// 리슨소켓 생성
void addClient(int s, struct sockaddr_in *newcliaddr);	// 클라이언트 저장
void removeClient(int s);								// 클라이언트 제거
void setTime();											// 현재시간 저장
int getmax();				// 최대 소켓번호 찾는 함수

void errquit(char *mesg)	// 에러 출력 
{ 
	perror(mesg); 
	exit(1); 
}

time_t timer;		// 시간측정 
struct tm *t;		// 시간 정보를 저장하는 구조체 
char time_m[30];	// 현재 시간
int listen_sock;	// 리슨 소켓
int num_chat = 0;		// 참가자 수
int clisock_list[MAXLINE];			// 참가자 소켓번호 목록

int main(int argc, char *argv[])
{
	int length;					// 파일 내용 길이
	DIR *dp;					// 디렉토리 포인터
	struct dirent *entry;		// 디렉토리 구조체
	struct stat statbuf;		// 파일 상태
	struct sockaddr_in cliaddr;	// 클라이언트 주소 구조체

	int addrlen = sizeof(struct sockaddr_in);	// 주소 길이
	int accp_sock;			// 반응이 온 소켓
	fd_set read_fds;		
	FILE *fp;				// 파일 객체 포인터
	struct sockaddr_in servaddr;

	// 서버 명령어 
	char* CLOSE_MSG = "Good Bye";
	char* EXIT_MSG = "exit";

	listen_sock = tcp_listen(INADDR_ANY , atoi(argv[1]), MAXLINE);
	while(1)
	{
		char file_address[MAXLINE] = {0};	// 파일의 주소 
		char file_put[MAXLINE] = {0};		// 클라이언트가 보내는 파일
		char file_get[MAXLINE] = {0};		// 서버가 보내는 파일
		char file_name[MAXLINE] = {0};		// 파일 이름
		char buf[MAXLINE] = {0};			// 터미널에서 입력받은 명령어

		// 소켓 select
		FD_ZERO(&read_fds);
		FD_SET(listen_sock, &read_fds);
		for(int i = 0; i < num_chat; i++)	// 접속한 클라이언트 소켓을 엶
			FD_SET(clisock_list[i], &read_fds);
		FD_SET(0, &read_fds);			// 키보드 입력 소켓
		int max = getmax() + 1;			// 최대 소켓 번호 + 1

		printf("wait for client\n");
		if(select(max, &read_fds, NULL, NULL, NULL) < 0)	
			errquit("select fail");

		if(FD_ISSET(listen_sock, &read_fds)) // 새로운 참여자
		{
			accp_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &addrlen);	
			addClient(accp_sock, &cliaddr);	// 참여자 추가
		}

		if(FD_ISSET(0, &read_fds))		// 키보드 입력 받음
		{
			if(fgets(buf, MAXLINE, stdin))
			{
				if(strstr(buf, EXIT_MSG) != NULL) // 종료 명령어(exit)를 입력하면 서버 닫음
				{
					setTime();
					printf("%sServer close\n", time_m);
					exit(0);
				}
			}
		}
	
		// 어떤 클라이언트가 메시지를 보냈는지 확인
		for(int i = 0; i < num_chat; i++)
		{
			if(FD_ISSET(clisock_list[i], &read_fds))
			{
				int clisock = clisock_list[i];	// 메시지를 보낸 클라이언트 소켓 저장
				if(recv(clisock, buf, MAXLINE, 0) < 0)
				{
					removeClient(clisock); // 클라이언트의 종료
					continue;
				}

				if(strchr(buf, '0') != NULL) // get mode, 서버에서 클라이언트로 파일 보내는 모드
				{
					printf("GET Mode : %d\n", clisock);
					recv(clisock, file_address, MAXLINE, 0); // 디렉토리 주소 받음

					if(strchr(file_address, '/') == NULL)		// 만약 디렉토리 주소를 입력하지 않으면
						getcwd(file_address, MAXLINE);			// 서버의 현재 디렉토리 경로를 저장
					printf("file addrss : %s\n", file_address); 

					if((dp = opendir(file_address)) == NULL) // 파일 경로 열기
					{
						fprintf(stderr, "파일경로 찾기 실패, file_address : %s\n", file_address);
						return 0;
					}
				
					int j = 0;	// 반복 변수 초기화
					while((entry = readdir(dp)) != NULL) // 해당 파일 경로에 있는 모든 파일 열기
					{
						lstat(entry->d_name, &statbuf); // 파일의 상태를 읽음
			
						if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
						{
							strcat(file_name, entry->d_name);
							strcat(file_name, "  ");
							j++;
							if(j == 5) // 보기 편하게 한줄에 5개씩 보이게 한다
							{
								strcat(file_name, "\n\t");
								j = 0;
							}
						}
					}

					printf("file list : %s\n", file_name);
					send(clisock, file_name, strlen(file_name), 0); // 클라이언트에게 파일 목록을 보냄
					memset(file_name, 0, MAXLINE);				// 버퍼 초기화

					recv(clisock, file_name, MAXLINE, 0);	// 클라이언트로부터 파일 이름을 받음
					printf("file_name : %s\n", file_name);
					if(file_address[strlen(file_address) - 1] != '/')
						file_address[strlen(file_address)] = '/';

					strcat(file_address, file_name);	// 해당 파일이 있는 주소
		
					fp = fopen(file_address, "r");	// 파일을 읽기 모드로 연다
					if(fp == NULL)
					{
						printf("파일을 열지 못했습니다.\n");
						exit(1);
					}
					
					j = 0;				// 반복 변수 초기화
					while(!feof(fp))	// 파일 끝까지 읽는다.
				  	{
						fgets(file_get, MAXLINE - 1, fp);	// 파일 내용읽기
						write(clisock, file_get, strlen(file_get)); // 파일 내용을 클라이언트에게 보낸다.
						printf("파일 송신 중.. 크기 : %ldbyte\n", strlen(file_get));

						j += strlen(file_get);
						memset(file_get, 0, MAXLINE);		// 버퍼 초기화
					}

					setTime();	// 현재 시간 저장
					printf("%s파일 송신 완료! 파일 총 크기 : %dbyte\n", time_m, j);
					fclose(fp);
					close(clisock);
					removeClient(clisock); // 해당 클라이언트 제거
				}

				else if(strchr(buf, '1') != NULL) // put mode, 클라이언트에서 서버로 파일 보내는 모드
				{   
					printf("PUT Mode : %d\n", clisock);           
					recv(clisock, file_name, MAXLINE, 0); // 파일 이름 알아오기..
					fp = fopen(file_name, "w");

					//파일 내용  읽고 쓰기       
					int j = 0; // 파일 크기를 저장하는 변수
					while((length = read(clisock, file_put, MAXLINE - 1)) > 0)    
					{           
						printf("파일 받는 중.. 크기 : %dbyte\n", length);        
						j += length;
						fputs(file_put, fp);
						memset(file_put, 0, MAXLINE); // 버퍼 초기화
					}       
					printf("%s파일 받기 완료! 파일 총 크기 : %dbyte\n", time_m, j);   
					fclose(fp);
					close(clisock);
					removeClient(clisock); // 해당 클라이언트 제거
				}

				else if(strchr(buf, '2') != NULL) // 종료 모드
				{
					printf("%s\n", CLOSE_MSG);
					send(clisock, CLOSE_MSG, strlen(CLOSE_MSG), 0);
					removeClient(clisock);
				}
				else
				{
					continue;
				}
			}
		}
	}
	return 0;		
}

int tcp_listen(int host, int port, int backlog) // 서버 소켓 초기화하는 함수
{
	int sd;
	struct sockaddr_in servaddr;
	sd = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성
	
	if(sd == -1) // 소켓 생성 실패
	{
		perror("socket fail");
		exit(1);
	}

	bzero((char *)&servaddr, sizeof(servaddr)); // 주소 초기화
	servaddr.sin_family = AF_INET;		// 인터넷 
	servaddr.sin_addr.s_addr = htonl(host); // 아이피 주소 
	servaddr.sin_port = htons(port);	// 포트번호
	
	bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
	listen(sd, backlog);
		
	return sd;
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

// 클라이언트 추가
void addClient(int s, struct sockaddr_in *newcliaddr)
{
	char buf[20];
	inet_ntop(AF_INET, &newcliaddr->sin_addr, buf, sizeof(buf)); // 클라이언트 IP 주소 저장

	// 채팅 클라이언트 목록에 추가
	clisock_list[num_chat] = s;
	num_chat++; // 인원수 증가

	setTime(); // 현재시간 저장
	printf("%snew client[%d] :%s\n", time_m, num_chat, buf);
	printf("현재 인원 수 : %d\n", num_chat);
}

// 클라이언트 제거
void removeClient(int s)
{
	close(s);
	setTime(); // 현재시간 저장
	num_chat--; // 참가자 감소
	printf("%s참가자 1명 탈퇴. 현재 인원 수 : %d\n", time_m, num_chat);
	for(int i = s; i < num_chat; i++)
		clisock_list[i] = clisock_list[i + 1];
}

// 현재 시간 저장하는 함수
void setTime()
{
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	sprintf(time_m, "<%d-%d-%d %d:%d:%d> ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

