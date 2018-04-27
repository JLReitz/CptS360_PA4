#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

//#define DEBUG_TERMINAL

// Global Variables ********************************************************************************

int _fd, _pid = 0, _status = 0, _fp;
int _sock, _newsock, _length; // socket descriptors
int _serverPort; // server port number

char * _t1 = "xwrxwrxwr-------";
char * _t2 = "----------------";
char * _cwd = ""; //current working directory
char * _hd = ""; //homedirectory

struct sockaddr_in _server_addr, _client_addr, _name_addr;
struct hostent * _hp;

// Functions ***************************************************************************************

int server_init(char * name)
{
	
	int r; // help variables

    printf("************************* server init *************************n\n");
    // get DOT name and IP address of this host

    printf("1: get and show server host info\n");
    _hp = gethostbyname(name);
    if (_hp == 0) {
            printf("unknown host\n");
            exit(1);
    }
    printf("    hostname = %s  IP = %s\n",
           _hp->h_name,  inet_ntoa(*(long *)_hp->h_addr));

    //  create a TCP socket by socket() syscall
    printf("2: create a socket\n");
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
            printf("socket call failed\n");
            exit(2);
    }

    printf("3: fill server_addr with host IP and PORT# info\n");
    // initialize the server_addr structure
    _server_addr.sin_family = AF_INET;             // for TCP/IP
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // THIS HOST IP address
    _server_addr.sin_port = htons(0); // let kernel assign port

    printf("4: bind socket to host info\n");
    // bind syscall: bind the socket to server_addr info
    r = bind(_sock,(struct sockaddr *)&_server_addr, sizeof(_server_addr));
    if (r < 0) {
            printf("bind failed\n");
            exit(3);
    }

    printf("5: find out Kernel assigned PORT# and show it\n");
    // find out socket port number (assigned by kernel)
    _length = sizeof(_name_addr);
    r = getsockname(_sock, (struct sockaddr *)&_name_addr, &_length);
    if (r < 0) {
            printf("get socketname error\n");
            exit(4);
    }

    // show port number
    _serverPort = ntohs(_name_addr.sin_port); // convert to host ushort
    printf("    Port = %d\n", _serverPort);

    // listen at port with a max. queue of 5 (waiting clients)
    printf("6: server is listening ....\n\n");
    listen(_sock, 5);
    printf("************************* init done *************************\n\n");
}

int put(char * filename)
{
	int n;
	char line[256];
	struct stat * filestat;
					
	if(stat(filename, filestat) != -1)
	{
		//Check to see if the file is the correct type
		if(S_ISREG(filestat->st_mode))
		{
			//Determine the size of the file
			int filesize = (int)filestat->st_size;
			
			//Write the file size to the client
			n = write(_newsock, &filesize, sizeof(int));
			n = read(_newsock, line, 256);
			
			if(!strcmp(line, "OK"))
			{
				//Open the file and read its contents to the socket
				int i = 0, count = 0;
				char content[256];
				FILE * fp = fopen("filename", "r");
				
				while(count < filesize)
				{
					content[i] = fgetc(fp);
					
					if(i == 255) //Do this 256 bytes at a time
					{
						n = write(_newsock, content, 256);
						i = 0;
					}
					else
						i++;
						
					count++;
				}
				
				fclose(fp);
				return 1;
			}
		}	
	}
	
	n = write(_newsock, (char)0, 256);
	return 0;
}

int get(char * filename)
{
	int filesize = 0;
	int n = write(_newsock, "OK", 256);
	char line[256];
	
	n = read(_newsock, &filesize, sizeof(int));
	
	if(n > 0)
	{
		int count = 0;
		
		n = write(_newsock, "OK", 256);
		
		FILE *fp = fopen(filename, "w");
		
		while(count < filesize)
		{
			n = read(_newsock, line, 256);
			count += n;
			fprintf(fp, "%s", line);
		}
		
		fclose(fp);
		return 1;
	}
	
	n = write(_newsock, 0, 256);
	return 0;
}

FILE * execute(char * cmd)
{
	return popen(cmd, "w");
}

// Main ********************************************************************************************

int main(int argc, char *argv[])
{
	int i, n;
	char * hostname = "";
    char line[256] = "", out[256] = "";
    
    //Set '_cwd' to the home directory
    //_hd = getenv("HOME");
    _cwd = _hd = "/";
    
    chdir(_cwd);
    
    if(argc == 2)
		hostname = argv[1];
    else
    	hostname = "localhost";
    
    server_init(hostname);
    
    while(1)
    {
    	char c;
    	char * temp = "", * cmd = "";
    	FILE * outstream = 0;
    	
#ifndef DEBUG_TERMINAL 	
        printf("Server Message: accepting new connection ....\n");

        // Try to accept a client connection as descriptor newsock
        _length = sizeof(_client_addr);
        _newsock = accept(_sock, (struct sockaddr *)&_client_addr, &_length);
        
        if (_newsock < 0) 
        {
            printf("Server Message: accept error\n");
            exit(1);
        }
        
        printf("Server Message: accepted a client connection from\n");
        printf("-----------------------------------------------\n");
        printf("        IP = %s  port = %d\n", inet_ntoa(_client_addr.sin_addr.s_addr),
               ntohs(_client_addr.sin_port));
        printf("-----------------------------------------------\n\n");
#endif

        // Processing loop
        while(1)
        {        	
#ifdef DEBUG_TERMINAL
			gets(line);
			n = strlen(line);
#else
            n = read(_newsock, line, 256);
            
            if (n==0) 
            {
		        printf("Server Message: client died, server will loop\n");
		        close(_newsock);
		        break;
            }
#endif
            
            // show the line string
            printf("Client Message Received: n = %d bytes\n", n);
            printf("Command Received: \"%s\"\n", line);
            temp = line;
			
			//Execute the given command and return the output file
			cmd = strtok(line, " ");
			
			if(!strcmp(cmd, "get"))
			{
				char * filename_literal;
				char filename[256];
				char pathname[256];
				
				strcpy(filename, strtok(0, " "));
				strcpy(pathname, _hd);
				
				if(filename[0] != '/')
				{
					strcat(pathname, filename);	
					strcpy(filename_literal, pathname);			
				}
				
				if(put(filename))
					printf("File transfer successfully completed\n\n");
				else
					printf("File transfer was unsuccessful\n\n"); 			
			}
			else if(!strcmp(cmd, "put"))
			{
				char * filename_literal;
				char filename[256];
				char pathname[256];
				
				strcpy(filename, strtok(0, " "));
				strcpy(pathname, _hd);
				
				if(filename[0] != '/')
				{
					strcat(pathname, filename);	
					strcpy(filename_literal, pathname);			
				}					
				
				if(get(filename))
					printf("File transfer successfully completed\n\n");
				else
					printf("File transfer was unsuccessful\n\n"); 
			}
			else if(!strcmp(cmd, "cd"))
			{
				char * dir = strtok(0, " ");
				
				//Check for absolute pathname
				if(dir[0] == '/')
					_cwd = dir;
				else if(!dir)
					_cwd = _hd;
				else
				{
					char cwd_temp[256];
					strcpy(cwd_temp, _cwd);
					strcat(cwd_temp, dir);
					_cwd = cwd_temp;
				}
					
				chdir(_cwd);
			}
			else
            	outstream = execute(temp);
            
            if(outstream) //Our output file will be defined if the last step worked correctly
            {
            	i = 0;
            	
		        //Scan the output stream for results and place them in the output buffer
		        while((c=fgetc(outstream)) != EOF)
		        {
		        	out[i] = c;
		        	i++;
		        	
		        	//If the output is longer than 256 bytes
		        	if(i == 255)
		        	{
#ifdef DEBUG_TERMINAL
						printf("%s", out);
#else
						printf("%s", out);
		        		n = write(_newsock, out, 256); //Print to the socket and begin loading again
#endif
		        	}
		        }
		       	
#ifdef DEBUG_TERMINAL
				printf("%s\n\n", out);
#else
				printf("%s\n\n", out);
        		n = write(_newsock, out, 256); //Write any remaining characters left in the buffer
        		//n = write(_newsock, "\n\n", 2);
#endif
				
				pclose(outstream);
            }
            else
            {
#ifdef DEBUG_TERMINAL
				printf("Your request could not be completed at this time for an unknown reason. Please try again\n\n");
#else
        		n = write(_newsock, out, 256); //Write any remaining characters left in the buffer
        		n = write(_newsock, "Your request could not be completed at this time for an unknown reason. Please try again\n\n", 256);
#endif
            }
        }
	}
	
	exit(1);
}
