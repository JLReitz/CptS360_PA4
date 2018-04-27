// The echo client client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX 256

// Define variables
struct hostent *hp;              
struct sockaddr_in  server_addr; 

int server_sock, r;
int SERVER_IP, SERVER_PORT; 


// clinet initialization code

int client_init(char *argv[])
{
  printf("======= clinet init ==========\n");

  printf("1 : get server info\n");
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  printf("2 : create a TCP socket\n");
  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  printf("3 : fill server_addr with server's IP and PORT#\n");
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  printf("4 : connecting to server ....\n");
  r = connect(server_sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("5 : connected OK to \007\n"); 
  printf("---------------------------------------------------------\n");
  printf("hostname=%s  IP=%s  PORT=%d\n", 
          hp->h_name, inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  printf("========= init done ==========\n");
}

main(int argc, char *argv[ ])
{
  int n;
  char line[MAX], ans[MAX], temp[MAX], c;
  char *cmd;
  FILE *output = 0;

  if (argc < 3){
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  client_init(argv);
  // sock <---> server
  printf("********  processing loop  *********\n");
  while (1){

    printf("\n");
    printf("********************** menu *********************\n");
    printf("* get  put  ls   cd   pwd   mkdir   rmdir   rm  *\n");
    printf("* lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *\n");
    printf("*************************************************\n\n");
    //printf("Enter a command: ");
    printf("\n");
 //n = read(server_sock, ans, MAX);
   // printf("%s :", ans);
    printf("input a line : ");
    bzero(line, MAX);                // zero out line[ ]
    fflush(stdin);
    fgets(line, MAX, stdin);         // get a line (end with \n) from stdin
  //fscanf(stdin,"%s", line);
    line[strlen(line)-1] = 0;        // kill \n at end
    if (line[0]==0)                  // exit if NULL line
       exit(0);
  
    strcpy(temp, line);
    cmd = strtok(temp, " ");

    printf("cmd= %s\n", cmd);

    if(line[0] == line[1] == 'l')
    {
      for(int i = 0; i < MAX-1; i++){
          line[i] = line[i+1];
      }
    }
    else if(line[0] == 'l' && strcmp(cmd, "ls")){
        
          for(int i = 0; i < MAX-1; i++){
          line[i] = line[i+1];
      }
    }
    
    printf("%s\n", line);


    if(!strcmp(cmd, "exit"))
      exit(0);
    else if(!strcmp(cmd, "get")){
        printf("entered get\n");
    }
    else if(!strcmp(cmd, "put")){
        printf("entered put\n");
    }
    else if(!strcmp(cmd, "ls")){
        printf("entered ls\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);

         /* n = read(server_sock, ans, MAX);
                printf("dir server response: %s\n", ans);
                if (strcmp(ans,"dir")==0)
                {
                        while (strcmp(ans, "ls complete")!= 0)
                        {
                                bzero(ans,MAX);
                                read(server_sock, ans,MAX);
                                printf("%s\n",ans);
                        }
                }
                else //(strcmp(response, "file")==0){
                {
                        bzero(ans,MAX);
                        read(server_sock, ans,MAX);
                        printf("%s\n",ans );
                        return 2;
                }*/
    }
    else if(!strcmp(cmd, "cd")){
        printf("entered cd\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=\n%s\n",n, ans);
    }
    else if(!strcmp(cmd, "pwd")){
        printf("entered pwd\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=\n%s\n",n, ans);
    }
    else if(!strcmp(cmd, "mkdir")){
        printf("entered mkdir\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=\n%s\n",n, ans);
    }
    else if(!strcmp(cmd, "rmdir")){
        printf("entered rmdir\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=\n%s\n",n, ans);
    }
    else if(!strcmp(cmd, "rm")){
        printf("entered rm\n");
        n = write(server_sock, line, MAX);
        printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

        // Read a line from sock and show it
        n = read(server_sock, ans, MAX);
        printf("client: read  n=%d bytes; echo=\n%s\n",n, ans);
    }
    else if(!strcmp(cmd, "lcat")){
        printf("entered lcat\n");
        output = popen(line, "w");
    }
    else if(!strcmp(cmd, "lls")){
        printf("entered lls\n");
        output = popen(line, "w");
    }
    else if(!strcmp(cmd, "lcd")){
        printf("entered lcd\n");
       output = popen(line, "w");
    }
    else if(!strcmp(cmd, "lpwd")){
        printf("entered lpwd\n");
        output = popen(line, "w");
    }
    else if(!strcmp(cmd, "lmkdir")){
        printf("entered lmkdir\n");
       output =  popen(line, "w");
    }
    else if(!strcmp(cmd, "lrmdir")){
        printf("entered lrmdir\n");
        output = popen(line, "w");
    }
    else if(!strcmp(cmd, "lrm")){
        printf("entered lrm\n");

        output = popen(line, "w");
    }
    else{
        printf("invalid command\n");
    }

    if(output){
      printf("reading from popen()\n");
       while(c = fgetc(output) != EOF){
         putchar(c);
       }
        pclose(output);
    }
    
    /*// Send ENTIRE line to server
    n = write(server_sock, line, MAX);
    printf("client: wrote n=%d bytes; line=(%s)\n", n, line);

    // Read a line from sock and show it
    n = read(server_sock, ans, MAX);
    printf("client: read  n=%d bytes; echo=(%s)\n",n, ans);
    */
  }
}


