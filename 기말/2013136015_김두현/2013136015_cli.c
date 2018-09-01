#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<dirent.h>
#include<netinet/in.h>
#include <time.h>		// 시간 헤더파일

#define MAXLINE 1024		// 최대 크기
#define NAME 20				// 사용자 이름 크기

int tcp_connect(int af, char*servip, unsigned short port);	// 서버와 연결하는 함수
void setTime();	
void errquit(char *mesg)	// 에러 메시지
{
	perror(mesg);
	exit(1);
}

time_t timer;		// 시간측정 
struct tm *t;		// 시간 정보를 저장하는 구조체 
char time_m[30];	// 현재 시간

int main(int argc, char *argv[])
{
	char* bufmsg;		 // 터미널에서 입력받은 메시지
	char bufall[MAXLINE+NAME] = {0}; // 사용자 이름 + 메시지 

	int max, s, i, length;		// 최대 소켓 번호+1, 서버 연결 소켓, 반복 변수, 내용 길이
	int namelen;				// 사용자 이름 크기
	namelen = strlen(argv[3]);	// 이름길이 저장
	bufmsg = bufall + namelen;	// 메시지 부분 

	s = tcp_connect(AF_INET , argv[1], atoi(argv[2])); // 서버 소켓 설정
	max = s + 1;				// 최대 소켓 + 1
	fd_set read_fds;			
	FD_ZERO(&read_fds);			// 열린 소켓을 모두 닫는다.	
	FILE* fp;					// 파일 포인터
	char cmd[MAXLINE] = "ls";	// 파일 목록 보는 명령어

	printf("모드 선택 (GET :0  PUT : 1  CLOSE : 2)\n");
	while(1)
	{
		printf("start\n");
		char put_file[MAXLINE] = {0};		// 서버로 보내는 파일
		char get_file[MAXLINE] = {0};		// 서버로부터 받은 파일
		char file_address[MAXLINE] = {0};	// 현재 디렉토리 
		char select_file[MAXLINE] = {0};	// 선택한 파일 이름

		FD_SET(0 , &read_fds);	// 키보드 입력 소켓
		FD_SET(s , &read_fds);	// 서버 연결 소켓
		
		if(select(max, &read_fds, NULL, NULL, NULL) < 0) // 소켓을 연다
			errquit("select fail");

		if(FD_ISSET(0, &read_fds))	// 터미널로부터 입력 받음
		{
			// 서버한테 메시지 보내기..
			if(fgets(bufmsg , MAXLINE, stdin))
			{
				send(s, bufmsg, strlen(bufmsg), 0);	// 메시지를 저장해서 보낸다
			}			
		}

		if(bufmsg[0] == '0')	// 서버로부터 파일을 받는 모드
		{
			printf("GET Mode\n");
			printf("다운로드 할 파일이 있는 경로 입력(입력하지 않으면 서버의 디렉토리를 가져옵니다)\n");
			scanf("%s", file_address);
			send(s, file_address, strlen(file_address), 0); // 서버에 보낸다.
			memset(file_address, 0, MAXLINE);	// 버퍼 초기화

			recv(s, file_address, MAXLINE, 0);	// 서버로부터 파일 리스트를 받아옴
			printf("파일 목록 : %s\n", file_address); // 리스트 출력

			printf("파일을 선택해 주세요 : ");
			scanf("%s", select_file);			// 가져올 파일 입력
			send(s, select_file, strlen(select_file), 0); // 선택한 파일 이름 전송
							
			//파일 생성
			if((fp = fopen(select_file, "w")) == NULL)
			{
				printf("파일을 열수 없습니다.\n");
				exit(1);
			}

			//파일을 수신하고 다시 쓴다.		
			i = 0;
			while((length = read(s, get_file, MAXLINE - 1)) > 0)
			{
				printf("파일 수신중.. 파일 크기 : %dbyte\n", length);
				i += length;
				fputs(get_file, fp);
				memset(get_file, 0, MAXLINE); // 버퍼 초기화
			}
			setTime();
			printf("%s파일 수신 완료!  총 파일 크기 : %d byte \n", time_m, i);
			fclose(fp);
			exit(0);
		}

		// 파일 보내기..       
		else if( bufmsg[0] == '1')       
		{         
			getcwd(file_address, MAXLINE);	// 클라이언트의 현재 디렉토리를 저장
			printf("현재 디렉토리 : %s\n", file_address);    
			printf("파일 리스트\n");
			system(cmd); // 해당 디렉토리에 있는 파일 목록들을 보여준다

			printf("파일을 선택해 주세요 : \n");     
			scanf("%s", select_file);			// 보낼 파일 이름을 입력 받음 
			send(s, select_file, strlen(select_file), 0); // 보낼 파일 이름 전송

			printf("**Filename : %s\n", select_file);     
			if((fp = fopen(select_file, "r")) == NULL)	// 파일을 읽기모드로 엶
			{
				printf("파일을 열수 없습니다.\n");
				exit(1);
			}

			sleep(1);
			i = 0;	// 파일 총 크기를 저장하는 변수
			while(!feof(fp)) // 파일 끝까지 확인함
			{
				fgets(put_file, MAXLINE - 1, fp);
				write(s, put_file, strlen(put_file));
				printf("파일 보내는 중.. 파일 크기 : %ld byte \n", strlen(put_file));
				i += strlen(put_file);
				memset(put_file, 0, MAXLINE);	// 버퍼 초기화
			}

			setTime();	
			printf("%s보내기 완료! 총 파일 크기 : %d byte\n", time_m, i);
			fclose(fp);
			exit(0);
		}

		else if( bufmsg[0] == '2')  // 종료 모드
		{
			recv(s, bufmsg, 90, 0);
			printf("%s\n", bufmsg);
			exit(1);
		}

		else	// 메뉴에 없는 값을 넣으면 다시 입력 받음
		{
			memset(bufmsg, 0, MAXLINE);
			printf("다시 입력해주세요!\n 모드 선택 (GET :0  PUT : 1  CLOSE : 2)\n");
			continue;
		}
	}	
}

// tcp 서버에 연결하는 함수
int tcp_connect(int af, char*servip, unsigned short port)
{
	int sd;
	struct sockaddr_in servaddr;
	sd = socket(af, SOCK_STREAM, 0);
	
	if(sd == -1)
	{
		perror("socket fail");
		exit(1);
	}

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = af;
	inet_pton(AF_INET , servip, &servaddr.sin_addr);
	servaddr.sin_port = htons(port);
	
	connect(sd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	return sd;
}

// 현재 시간 저장하는 함수
void setTime()
{
	timer = time(NULL); // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	sprintf(time_m, "<%d-%d-%d %d:%d:%d> ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
