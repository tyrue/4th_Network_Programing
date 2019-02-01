# 채팅 서버와 클라이언트(Chat server & client) - 중간 과제

## 1. 문제 조건

1. 클라이언트 간의 채팅 프로그램을 기반으로 작성한다.  

2. 서버는 접속하는 클라이언트가 송신한 메시지를 모두 관리하고 모니터링 한다.    

3. 클라이언트가 보내는 문자를 모두 기록하고 있으며, 총 문자 수, 보낸 시간 등의 내역을 보관한다. 

4. 이 외에 서버는 총 접속자 수(클라이언트의 총 접속자 수), 각 클라이언트의 접속 시간, 클라이언트의 IP를 관리한다. 

5. 클라이언트는 최소 5개 이상 접속이 되도록 작성하시오. 

## 2. 실행 과정

### (1) 서버 접속

![image](https://user-images.githubusercontent.com/20302410/52108522-9a97a200-263d-11e9-992d-859f905b2eda.png)

위와 같이 프로그램을 실행하여 서버를 포트번호를 5001로 시작한다.



### (2) 클라이언트 접속

![image](https://user-images.githubusercontent.com/20302410/52108538-ab481800-263d-11e9-87c7-048eb27642f7.png)

클라이언트는 각각 위와 같이 서버 아이피, 서버 포트번호, 사용자이름을 입력하여 서버에 접속한다.



![image](https://user-images.githubusercontent.com/20302410/52108554-bac76100-263d-11e9-9c31-c2193ef96d8c.png)

서버에는 접속자의 아이피와 몇 번째의 접속자인지 출력된다.



### (3) 서버 명령어 목록

![image](https://user-images.githubusercontent.com/20302410/52108650-fd893900-263d-11e9-9a0d-a760c0005104.png)

서버 터미널에 help를 치면, 서버에서 사용할 수 있는 명령어 목록을 출력한다.



### (4) 서버 명령어 - iplist

![image](https://user-images.githubusercontent.com/20302410/52108783-78eaea80-263e-11e9-8eb7-362a4ab549ce.png)

서버 터미널에 iplist를 입력하면, 접속시간이 빠른 순서대로 접속자의 아이피주소를 출력한다. 만약 접속자가 아직 메시지를 입력하지 않으면, 접속자의 이름을 알 수 없으므로 iplist에는 접속자의 이름이 출력이 되지 않는다.

![image](https://user-images.githubusercontent.com/20302410/52108793-80aa8f00-263e-11e9-9b92-017c259558fb.png)

위와 같이 두현이 메시지를 보내면 접속자 이름이 저장되고, iplist에도 두현의 이름이 뜨게 된다. 두현은 1번째 접속자이다.

서버에는 접속자의 메시지가 현재시간, 아이피 주소, 접속자 이름, 메시지, 문자수 순으로 출력된다.



### (5) 서버 명령어 - time

![image](https://user-images.githubusercontent.com/20302410/52108800-899b6080-263e-11e9-973b-512990b94c02.png)

서버 터미널에 time을 입력하면 접속 시간 순으로 접속자의 접속 시간을 출력한다. time도 역시 아직 메시지를 보내지 않은 접속자의 이름은 출력이 되지 않는다.

![image](https://user-images.githubusercontent.com/20302410/52108813-9750e600-263e-11e9-8db6-3eb6e079ff84.png)

민성이 메시지를 보내 이름이 저장된 후의 time목록을 나타낸 것이다. 민성은 5번째 접속자이다.



### (6) 서버 명령어 - count

![image](https://user-images.githubusercontent.com/20302410/52108838-b64f7800-263e-11e9-9d12-3bee896aac7d.png)

접속자들이 각각 메시지를 보낸 후, count를 입력한 결과이다. 메시지 4개가 추가되어 총 메시지 수는 6개이다.



### (7) 클라이언트 명령어 - exit

![image](https://user-images.githubusercontent.com/20302410/52108914-07f80280-263f-11e9-8f5a-90df4a4c8abe.png)

철수가 터미널에 exit를 입력하여 서버를 나가면 서버에는 철수가 탈퇴했다고 메시지를 보낸다.

철수가 탈퇴한 후 접속자 접속 시간 목록이다. 철수가 삭제된 것을 알 수 있다.

![image](https://user-images.githubusercontent.com/20302410/52108941-1e9e5980-263f-11e9-9171-19bccb1246d6.png)

만약 접속자가 아무런 메시지도 안하고 바로 탈퇴하면 서버에서는 익명의 참가자가 탈퇴 했다고 알린다.

### (8) 서버 명령어 - exit

![image](https://user-images.githubusercontent.com/20302410/52108971-3544b080-263f-11e9-8873-ead2204401b8.png)

각 사용자들이 모두 탈퇴하고, 더 이상 서버를 열 필요가 없으면 서버 터미널에 exit를 입력하여 서버를 닫을 수 있다. 

### (9) log.txt

![image](https://user-images.githubusercontent.com/20302410/52108988-42619f80-263f-11e9-8e13-aadb7e28aefd.png)

log.txt파일에는 지금까지의 채팅 기록이 저장되어 있다. log.txt파일은 서버 프로그램이 실행되는 폴더에 저장된다.



# 파일 전송 서버와 클라이언트(Chat server & client) - 기말 과제

## 1. 문제 조건

1. 파일 전송 서버는 서버에서 실행중이며 클라이언트가 전달해주는 내용을 파일로 저장하고 보내주는 역할을 한다.

2. 파일 전송 클라이언트는 서버에 전송할 파일을 선택하여 전송한다. 서버에서 선택한 파일을 전송받아 저장하는 기능을 수행한다. get : 다운로드 명령, put : 업로드 명령

-클라이언트는 접속한 디렉토리에 대해서 파일을 업로드, 다운로드 하도록 함.

-서버에 전송한 파일을 저장하는 디렉토리는 서버 프로세스가 실행되는 곳으로 함. 다운로드할 수 있는 파일은 전체 디렉토리를 대상으로 가능함(퍼미션을 고려하여 다운로드 가능한 파일)

-text 파일, binary 파일 모두 전송이 가능하도록 함.



## 2. 실행 과정

### **(1)** **서버 접속**

![image](https://user-images.githubusercontent.com/20302410/52109339-7d180780-2640-11e9-97a8-cdb991c15856.png)

서버는 서버의 포트 번호를 입력하여 서버를 실행 한다. 클라이언트가 접속하면, 접속한 시간과 클라이언트의 IP주소, 몇 번째 참가자인지 터미널에 보여준다. 그리고 클라이언트의 메시지를 받을 준비를 한다.

![image](https://user-images.githubusercontent.com/20302410/52109362-95882200-2640-11e9-8e23-bead89aad207.png)

클라이언트는 IP, 서버의 포트번호, 참여자 이름을 입력하여 서버에 접속한다. 참여자 이름은 클라이언트를 구분하기 위함이다.

서버에 접속하면 모드를 입력 받을 준비를 한다.



### **(2) GET** **모드**

![image](https://user-images.githubusercontent.com/20302410/52109416-b94b6800-2640-11e9-91f1-0cbc23df90a4.png)

클라이언트가 0번을 입력하면 GET모드로 진입한다. 클라이언트는 원하는 파일이 있는 디렉토리를 입력한다.

​    ![image](https://user-images.githubusercontent.com/20302410/52109453-e861d980-2640-11e9-8f49-29c8e4a1102e.png)

그러면 해당 디렉토리에 있는 모든 파일 목록이 터미널에 보여진다.



![image](https://user-images.githubusercontent.com/20302410/52109479-02032100-2641-11e9-8df8-16832c5dde89.png)

서버에서는 클라이언트가 GET모드로 들어갔다는 것을 알린다. 그리고 클라이언트가 입력한 디렉토리에 있는 모든 파일 목록을 보여준다.

​     

![image](https://user-images.githubusercontent.com/20302410/52109498-16dfb480-2641-11e9-9a29-03bd57df8a62.png)

다음으로 클라이언트가 다운로드 하고 싶은 파일 이름(alarm_intr.c)을 입력하면 시간이 지난 후, 파일을 받을 수 있다. 터미널에는 다운로드한 파일의 크기를 알려준다. 다운로드가 완료되면 클라이언트는 서버에 나가게된다.

![image](https://user-images.githubusercontent.com/20302410/52109515-23fca380-2641-11e9-8057-a1aafda4643f.png)

서버에서는 파일 송신이 완료됨을 알리고, 파일의 크기를 보여준다. 그리고 클라이언트가 탈퇴한 것을 알려준다.



![image](https://user-images.githubusercontent.com/20302410/52109532-324abf80-2641-11e9-9879-d92304114cc4.png)

해당 디렉토리(소스코드들)에 있는 원본 파일이 클라이언트가 접속한 디렉토리(test2)에 제대로 복사가 된 것을 볼 수 있다.

​     

![image](https://user-images.githubusercontent.com/20302410/52109579-53131500-2641-11e9-8c20-fe9706a34e19.png)

만약 클라이언트가 원하는 디렉토리를 입력하지 않았다면 현재 서버가 실행된 디렉토리에 있는 파일 목록을 보여준다.

 ![image](https://user-images.githubusercontent.com/20302410/52109583-573f3280-2641-11e9-9774-86f7a3175781.png)   

서버에서는 현재 디렉토리(/home/doo/test1)에 있는 파일 목록을 보여준다.

### (3) PUT 모드

![image](https://user-images.githubusercontent.com/20302410/52109680-aab18080-2641-11e9-977f-e2ba5ec7534c.png)

클라이언트가 모드 선택에서 1번을 입력하면 PUT모드로 진입한다. PUT모드에서는 클라이언트가 실행된 현재 디렉토리에 있는 파일 목록을 보여준다.

![image](https://user-images.githubusercontent.com/20302410/52109688-af763480-2641-11e9-8c1a-d1835417de23.png)

서버에서는 클라이언트가 PUT모드로 진입한 것을 보여준다.

​     

![image](https://user-images.githubusercontent.com/20302410/52109694-b8ff9c80-2641-11e9-8184-f975fed1fe69.png)

클라이언트가 보내고 싶은 파일 이름을 입력하면, 시간이 지난 후 파일 전송 완료를 알리고, 보낸 파일의 크기를 보여준다. 그리고 서버에서 나가게 된다.

![image](https://user-images.githubusercontent.com/20302410/52109708-c87ee580-2641-11e9-8327-1a5258e56a2b.png)

서버에서는 파일을 제대로 받은 것을 알리고, 받은 파일의 크기를 보여준다. 그리고 해당 클라이언트를 서버에서 나가게 한다.

![image](https://user-images.githubusercontent.com/20302410/52109734-d9c7f200-2641-11e9-80e2-65800a162804.png)

클라이언트 디렉토리(test2)에서 서버 디렉토리(test1)로 바이너리 파일 test가 복사가 된 것을 볼 수 있다.

### (4) 서버와 클라이언트 종료

![image](https://user-images.githubusercontent.com/20302410/52109756-e8160e00-2641-11e9-923e-c9c53160c498.png)

클라이언트가 모드 선택에서 2번을 입력하면 서버에서 종료 메시지를 받고 바로 나가게 된다.

​     

![image](https://user-images.githubusercontent.com/20302410/52109767-f106df80-2641-11e9-8574-7a7f02c5ed00.png)

서버는 클라이언트에게 종료 메시지를 알려주고 탈퇴시킨다.

​     

![image](https://user-images.githubusercontent.com/20302410/52109781-f9f7b100-2641-11e9-89ac-49e6c958e8b4.png)

서버에서 exit를 입력하면 서버를 종료 시킨다.

​     

