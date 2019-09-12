#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int initsock();
int acceptconn(int);
void job(int);

int main(void)
{
    int sock, _sock;

    sock = initsock();
    
    while (1)
    {
        _sock = acceptconn(sock);

        job(_sock);

        close(_sock);
    }

    close(sock);
}

int initsock()
{
    int sock;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(11111);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    listen(sock, 5);

    return sock;
}

int acceptconn(int sock)
{
    struct sockaddr_in _addr;
    socklen_t _szaddr = sizeof(struct sockaddr_in);

    return accept(sock, (struct sockaddr *)&_addr, &_szaddr);
}

void recvfile(int, int, int);

void job(int fd_cli)
{
    char fnam_src[0x100] = "XXXXXX.cu",
         fnam_bin[0x100] = "XXXXXX",
         fnam_in [0x100] = "XXXXXX",
         fnam_out[0x100] = "XXXXXX";

    int fd;
    uint32_t sz, szbe;

    read(fd_cli, &szbe, sizeof(uint32_t));
    sz = ntohl(szbe);
    fd = mkstemps(fnam_src, 3);
    recvfile(fd, fd_cli, sz);
    close(fd);

    read(fd_cli, &szbe, sizeof(uint32_t));
    sz = ntohl(szbe);
    fd = mkstemp(fnam_in);
    recvfile(fd, fd_cli, sz);
    close(fd);

    fd = mkstemp(fnam_bin);
    close(fd);

    fd = mkstemp(fnam_out);
    close(fd);

    if (0 == fork())
    {
        execlp("nvcc", "nvcc", fnam_src, "-o", fnam_bin, NULL);
    }
    wait(NULL);

    if (0 == fork())
    {
        freopen(fnam_in, "r", stdin);
        freopen(fnam_out, "w", stdout);
        execl(fnam_bin, fnam_bin, NULL);
    }
    wait(NULL);

    struct stat st;

    fd = open(fnam_out, O_RDONLY);
    fstat(fd, &st);
    szbe = htonl(st.st_size);
    write(fd_cli, &szbe, sizeof(uint32_t));
    sendfile(fd_cli, fd, NULL, sz);
    close(fd);

    unlink(fnam_src);
    unlink(fnam_bin);
    unlink(fnam_in );
    unlink(fnam_out);
}

void recvfile(int dst, int src, int nbytes)
{
    char buf[0x1000];

    while (nbytes > sizeof(buf))
        nbytes -= write(dst, buf, read(src, buf, sizeof(buf)));
    while (nbytes > 0)
        nbytes -= write(dst, buf, read(src, buf, nbytes));
}
