#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

int initconn(char *);

void sendfile_(int, FILE *, int);
void recvfile_(FILE *, int, int);

int main(int argc, char **argv)
{
    int sock;
    FILE *file;
    unsigned __int32 sz, szbe;

    sock = initconn(argv[1]);

    file = fopen(argv[2], "rb");
    fseek(file, 0, SEEK_END);
    sz = ftell(file);
    rewind(file);
    szbe = htonl(sz);
    send(sock, &szbe, sizeof(unsigned __int32), 0);
    sendfile_(sock, file, sz);
    fclose(file);

    file = fopen(argv[3], "rb");
    fseek(file, 0, SEEK_END);
    sz = ftell(file);
    rewind(file);
    szbe = htonl(sz);
    send(sock, &szbe, sizeof(unsigned __int32), 0);
    sendfile_(sock, file, sz);
    fclose(file);

    file = fopen(argv[4], "wb");
    recv(sock, &szbe, sizeof(unsigned __int32), 0);
    sz = ntohl(szbe);
    recvfile_(file, sock, sz);
    fclose(file);

    closesocket(sock);

    return 0;
}

int initconn(char *ip)
{
    WSADATA wsa;

    int sock;
    struct sockaddr_in addr;

    WSAStartup(MAKEWORD(2,2), &wsa);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(11111);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    return sock;
}

void sendfile_(int dst, FILE *src, int nbytes)
{
    char buf[0x1000];

    while (nbytes > sizeof(buf))
        nbytes -= send(dst, buf, fread(buf, 1, sizeof(buf), src), 0);
    while (nbytes > 0)
        nbytes -= send(dst, buf, fread(buf, 1, nbytes, src), 0);
}

void recvfile_(FILE *dst, int src, int nbytes)
{
    char buf[0x1000];

    while (nbytes > sizeof(buf))
        nbytes -= fwrite(buf, 1, recv(src, buf, sizeof(buf), 0), dst);
    while (nbytes > 0)
        nbytes -= fwrite(buf, 1, recv(src, buf, nbytes, 0), dst);
}
