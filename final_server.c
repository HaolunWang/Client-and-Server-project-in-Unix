/*
@author: Suhird & Haolun Wang

This is a sample project to show a dice game over sockets in C
*/

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>

static int client_count = 0;
void serviceClient(int, int);

int main(int argc, char *argv[]){  

  srand(time(NULL));
  pid_t p;
  int n;
  int sd, client, portNumber;
  socklen_t len;
  char *currentTime;
  time_t currentUnixTime;
  struct sockaddr_in servAdd;     // server socket address
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);
  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);
  
  bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
  listen(sd, 5);

  while(1){
    client = accept(sd, NULL, NULL);
    client_count++;    
    printf("-------------------------------------------------\n");
    printf("== Client #%d Connected,\n",client_count);
    printf("-------------------------------------------------\n");  
    if(!fork())
    {
      serviceClient(client, client_count);
    }    
    // wait(&n);
  }
    close(client);  
}

void serviceClient(int client, int c_no)
{
  int client_score[2]={0,0}, server_score[2]={0,0};
  char message[255],greeting[50]="Lets Play A Dice Game";
  write(client, greeting,sizeof(greeting));
  while(1)
  {
    
    /*reading client score */
    if(read(client, &client_score, sizeof(client_score))<0)
    {
      fprintf(stderr, "read() error\n");
      exit(3);
    }
    if(client_score[0] == 0)
    {
      printf("------------------------- Client %d has left the game. -------------------------\n",c_no);
      close(client);
      exit(5);      
    }
    printf("\n---- Client %d has roll the dice ----\n",c_no);
    printf("----------Client score----------\n");
    printf("-----> Client dice value : %d\n",client_score[0]);
    printf("-----> Total client score : %d\n\n\n",client_score[1]);


    
    /*checking client score and writing*/
    if(client_score[1] >= 100)
    {
      write(client,"Game over: You won the game.",50);
      printf("Client has won with score:%d\n",client_score[1]);
      close(client);
      exit(5);
    }

    /*server dice roll and sending server score to client*/
    /*write operation*/  
    server_score[0] = (rand() % 6)+1;
    server_score[1]+=server_score[0];
    printf("----------Server score----------\n");
    printf(":::::> Server dice value: %d\n", server_score[0]);
    printf(":::::> Total server score : %d\n", server_score[1]);
    write(client, &server_score, sizeof(server_score));

    /*checking server score and writing to client*/
    if(server_score[1] >= 100)
    {
      write(client,"Game over: You lost the game.",50);      
      printf("Server has won with score:%d\n",server_score[1]);
      close(client);
      exit(5);
    }
    write(client,"Game on: you can now play your dice",50);
    printf("-------------------------------------------------\n");
  }
    exit(1); 
}
