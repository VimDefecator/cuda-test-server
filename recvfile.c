#include <unistd.h>

void recvfile(int dst, int src, int nbytes)
{
    char buf[0x1000];

    while (nbytes > sizeof(buf))
        nbytes -= write(dst, buf, read(src, buf, sizeof(buf)));
    while (nbytes > 0)
        nbytes -= write(dst, buf, read(src, buf, nbytes));
}
