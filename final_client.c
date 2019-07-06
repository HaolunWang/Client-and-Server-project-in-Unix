/*
@author: Suhird & Haolun Wang

This is a sample project to show a dice game over sockets in C
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>


int main(int argc, char *argv[])
{
  srand(time(NULL));
  signal(SIGINT, SIG_IGN);
  char message[100];
  int server, portNumber;
  socklen_t len;
  struct sockaddr_in servAdd;     // server socket address
  
 if(argc != 3){
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
     fprintf(stderr, "Cannot create socket\n");
     exit(1);
  }

  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
  fprintf(stderr, " inet_pton() has failed\n");
  exit(2);
  }

 if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
  fprintf(stderr, "connect() has failed, exiting\n");
  exit(3);
 }


  int client_score[2]={0,0}, server_score[2]={0,0};
  char ch;

  /*reading string message from server*/

  if (read(server, message, 100)<0){
    fprintf(stderr, "read() error\n");
    exit(3); 
  }
  printf("-------------------------------------------------\n");
  printf("----- %s -----\n", message);
  printf("-------------------------------------------------\n");

  while(1)
  {
    
    /* Client dice roll and sending its score to server */
    printf("-------------------------------------------------\n");
    printf("Press the Enter key to roll Dice\n");
    printf("To Quit press 'q'\n");
    printf("-------------------------------------------------\n");
    while(1)
    {
      ch=fgetc(stdin);
      
      if(ch=='\n')
      {
        client_score[0] = (rand() % 6)+1;
        client_score[1]+=client_score[0];
        break;
      }
      else if(ch=='q')
      {
        client_score[0]=0;
        client_score[1]=0;
        write(server, &client_score, sizeof(client_score));        
        printf("\n-------- Quitting Game from Client -----------\n");
        printf("-------------------------------------------------\n");
        exit(7);        
      }
      else
      {
        client_score[0]=0;
        printf("To roll dice press Enter");
      }
    }    

    printf("\n--- Client score ---\n");
    printf("-----> Client dice value : %d\n", client_score[0]);
    printf("-----> Total client score : %d\n", client_score[1]);
    printf("\n\n");
    write(server, &client_score, sizeof(client_score));

    /*checking client score and reading from server*/
  
    if(client_score[1] >= 100)
    {
      if (read(server, &message, 50)<0){
        fprintf(stderr, "read() error\n");
        exit(3); 
      }
      printf("-------------------------------------------------\n");
      printf("%s\n",message);
      printf("Exiting...");
      close(server);
      exit(5);
    }
    
   
    if (read(server, &server_score, sizeof(server_score))<0){
      fprintf(stderr, "read() error\n");
      exit(3); 
    }
    printf("----- Server score -----\n");    
    printf(":::::> Server dice value :%d \n",server_score[0]);
    printf(":::::> Total server score : %d\n",server_score[1]);



    if(server_score[1] >= 100)
    {
      if (read(server, &message, 50)<0){
        fprintf(stderr, "read() error\n");
        exit(3); 
      }
      printf("-------------------------------------------------\n");
      printf("%s\n",message);
      printf("Exiting...");
      close(server);
      exit(5);
    }


    if (read(server, &message, 50)<0){
      fprintf(stderr, "read() error\n");
      exit(3); 
    }
    printf("-------------------------------------------------\n");     
    printf("%s\n",message);
    printf("-------------------------------------------------\n");
  }

  exit(0);
}
