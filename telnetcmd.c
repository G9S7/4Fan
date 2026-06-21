#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <inttypes.h>

#ifdef dbginfo_console_ON
    #define DBGprint_console(...) fprintf(stdout, __VA_ARGS__), fflush(stdout)
#else
    #define DBGprint_console(...)
#endif 

int sock_input_output(int sock, const uint8_t * in, size_t in_size, size_t wait_time, uint8_t * out,size_t* out_size){
    write(sock, in, in_size);
    bzero(out,*out_size);
    sleep(wait_time);
    ssize_t _s = read(sock, out, *out_size-1);
    out[_s] = 0;
    * out_size = _s;
}


int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    int len;

    if (argc < 6)
    {
        DBGprint_console("Usage: %s address port username password cmd\n", argv[0]);
        return 1;
    }
    const uint8_t *ip = argv[1];
    uint16_t port = 23;
    sscanf(argv[2], "%" SCNd16, &port);
    uint8_t username[128] = {};
    strcat(username,argv[3]);
    strcat(username,"\n");

    uint8_t password[128] = {};
    strcat(password,argv[4]);
    strcat(password,"\n");

    uint8_t Cmd[256] = {};
    int i;
    for(i=5;i<argc;i++){
        strcat(Cmd,argv[i]);
        strcat(Cmd," ");
    }
    strcat(Cmd,"\n");

    //Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        DBGprint_console("Could not create socket. Error");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    //Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        DBGprint_console("connect failed. Error");
        return 1;
    }

    sleep(2);
    uint8_t buf[1024] = {};
    bzero(buf,1024);
    ssize_t buf_size = read(sock, buf, 1024);
    buf[buf_size] = 0;
    DBGprint_console("%s\n",buf);

    //username
    buf_size = 1024;
    sock_input_output(sock,username,strlen(username),1,buf,&buf_size);
    DBGprint_console(buf);
    //password
    buf_size = 1024;
    sock_input_output(sock,password,strlen(password),1,buf,&buf_size);
    DBGprint_console(buf);
    //Cmd
    buf_size = 1024;
    sock_input_output(sock,Cmd,strlen(Cmd),1,buf,&buf_size);
    printf(buf);

    //exit
    buf_size = 1024;
    sock_input_output(sock,"exit\n",strlen("exit\n"),1,buf,&buf_size);
    DBGprint_console(buf);

    close(sock);

    return 0;
}
