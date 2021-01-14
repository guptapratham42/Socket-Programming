#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8000
#define buff_sz 4000
void send_ack(int sock_fd)
{
    char ack[60];
    ack[0] = 'o';
    ack[1]='k';
    write(sock_fd, ack, 3);
}
void check_ack(int sock_fd)
{
    int temp=3;
    char ack[60];
    recv(sock_fd, ack, temp, 0);
}
int Par_int(char *a)
{
    int len;
    len = strlen(a);
    int n = 0, i=0;
    for (i = 0; i < len; i++)
    {
        if (a[i] != ' ')
        {
            n *= 10 ;
            n+= a[i] - '0';
        }
    }
    return n;
}
int main(int argc, char const *argv[])
{
    int port = 8000;
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        printf("socket failed");
        exit(0);
    }
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        printf("setsockopt");
        exit(0);
    }
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = port;    
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char *size;
    char *buff = (char *)malloc(buff_sz);
    recv(new_socket, buff, buff_sz, 0);
    int arg_c, fd;
    send_ack(new_socket);
    arg_c = Par_int(buff);
    size = (char *)malloc(buff_sz + 5);
    int fsize = 0, fblocks = 0, x=0;
    for (x = 0; x < arg_c; x++)
    {
        struct stat statbuf;
        int tot;
        memset(buff, '\0', buff_sz);
        recv(new_socket, buff, buff_sz, 0);
        tot=0;
        send_ack(new_socket);
        if ((fd = open(buff, O_RDWR, 0666)) < 0)
        {
            perror("");
            exit(1);
        }
        stat(buff, &statbuf);
        fsize = statbuf.st_size;
        int i;
        sprintf(size, "%d", fsize);
        send(new_socket, size, strlen(size), 0);
        check_ack(new_socket);
        fblocks =fsize / buff_sz;
        fblocks++;
        if(fsize % buff_sz == 0)
            fblocks--;
        for (i = 0; i < fblocks; i++)
        {
        	int sz;
            memset(buff, '\0', buff_sz);
            if (!(i +1 -fblocks))
            {
                sz = fsize;
                sz -= buff_sz * (fblocks - 1);
                read(fd, buff, sz);
                send(new_socket, buff, sz, 0);
                tot += sz;
                check_ack(new_socket);
            }
            else
            {
                sz = buff_sz;
                read(fd, buff, buff_sz);
                send(new_socket, buff, sz, 0);
                tot += sz;
                check_ack(new_socket);
            }
        }
        close(fd);
        send_ack(new_socket);
    }
    return 0;
}
