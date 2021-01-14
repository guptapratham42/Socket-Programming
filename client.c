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
    int port=8000;
  struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    //char *hello = "Hello from client";
    //char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = port;
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char *buff = (char *)malloc(buff_sz + 5);
    sprintf(buff, "%d", argc - 1);
    int fsize = 0;
    char *size, *fname;
    size = (char *)malloc(buff_sz);
    int fblocks = 0;
    send(sock, buff, strlen(buff), 0);
    check_ack(sock);
    int fd, x=1;
    fname = (char *)malloc(buff_sz);
    for(x = 1; x < argc; x++)
    {
        int tot;
        memset(fname, '\0', buff_sz);
        strcpy(fname, argv[x]);
        tot=0;
        send(sock, fname, strlen(fname), 0);
        int i;
        check_ack(sock);
        recv(sock, size, buff_sz, 0);
        i=0;
        send_ack(sock);
        fsize = Par_int(size);
        // printf("size is %d\n", fsize);
        fblocks =fsize / buff_sz;
        fblocks++;
        if(fsize % buff_sz == 0)
            fblocks--;
        //fblocks = () ? fsize / buff_sz : fsize / buff_sz + 1;
        if ((fd = open(fname, O_CREAT | O_RDWR|O_TRUNC, 0666)) < 0)
        {
            perror("");
            exit(1);
        }
        for (i = 0; i < fblocks;)
        {
            memset(buff, '\0', buff_sz);
            int sz;
            memset(size,'\0',buff_sz);
            if (!(i +1 -fblocks))
            {
                sz = fsize;
                sz -= buff_sz * (fblocks - 1);
                recv(sock, buff, sz, 0);
                write(fd, buff, sz);
                tot += sz;
                send_ack(sock);
            }
            else
            {
                sz = buff_sz;
                recv(sock, buff, sz, 0);
                write(fd, buff, sz);
                tot += sz;
                send_ack(sock);
            }
            memset(buff, '\0', buff_sz);
            sprintf(buff, "\rPercentage of file copied: %lf", (100.0 * tot) / fsize);
            i++;
            write(1, buff, strlen(buff));
        }
        printf("\n");
        close(fd);
        check_ack(sock);
    }
    return 0;
}
