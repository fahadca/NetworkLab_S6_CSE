#include<stdio.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
struct timeval timeout;

void func(int connfd){
  char buff[MAX];
  int flag[20]={0};
  int f,c,ack,next=0,w1=1,w2,ws,nacksend=0;
  printf("Enter size of window : ");
  scanf("%d",&ws);
  w2=ws-1;
  
  while(1){
    sleep(1);
    bzero(buff,MAX);
    recv(connfd,buff,MAX,0);
    if(strcmp("EXIT",buff)==0){
      printf("EXIT\n");
      break;
    }
    f=atoi(buff);
    c=rand()%3;
    
    switch(c){
      case 0: continue;
      case 1: sleep(2);
              break;
    }
    if(f>=w1 && f<=w2 && flag[f]==0){
      if(f==w1){
        nacksend=0;
        flag[f]=1;
        for(;w1<=w2 && flag[w1]==1;w1++);
        printf("Frame %d received\nAcknowledgment send:%d\n",f,w1-1);
        bzero(buff,MAX);
        snprintf(buff,sizeof(buff),"%d",w1-1);
        send(connfd,buff,sizeof(buff),0);
        w2=w1+ws;
        }
      else{
        if(nacksend==0){
          printf("Received frame %d instead of frame %d\nsending NAK %d\n",f,w1,-1*w1);
          bzero(buff,MAX);
          snprintf(buff,sizeof(buff),"%d",-1*w1);
          send(connfd,buff,sizeof(buff),0);
          flag[f]=1;
        }
        else{
          printf("Received frame %d instead of frame %d \n",f,w1);
          flag[f]=1;
          continue;
        }
      }
    }
    else{
      printf("Frame %d discarded\n",f);
    }
  
  }
        
}
void main(){
  int sockfd,connfd,len;
  struct sockaddr_in servaddr,cli;
  sockfd=socket(AF_INET,SOCK_STREAM,0);
  if(sockfd==-1){
    printf("socket creation failed\n");
    exit(0);
  }
  else{
    printf("socket successfully created\n");
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(PORT);
    if((bind(sockfd,(SA*)&servaddr,sizeof(servaddr))) != 0){
      printf("Socket bind failed\n");
      exit(0);    
    }
    else
      printf("socket successfully created");
      
    if((listen(sockfd,5)) != 0)  {
      printf("listen failed\n");
      exit(0);
    }
    else
      printf("server listening");
      
    len=sizeof(cli);
    
    connfd=accept(sockfd,(SA*)&cli,&len);
    
    if(connfd<0){
      printf("server accept failed\n");
      exit(0);
    }
    else
      printf("server accept the client\n");
      
    func(connfd);
    
    close(sockfd);	
  }
}














    
 

