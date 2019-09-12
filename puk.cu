#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

__global__
void puk(char *dst, char *src, int wid, int hei)
{
    int stride = gridDim.x * blockDim.x,
        idx = blockDim.x * blockIdx.x + threadIdx.x;

    for (int di = idx; di < wid * hei; di += stride)
    {
        int dxi, dyi, sxi, syi;
        float dxf, dyf, sxf, syf;

        dxi = di % wid;
        dyi = di / wid;
        dxf = (float )(dxi - wid/2) / (wid/2);
        dyf = (float )(dyi - hei/2) / (hei/2);
        sxf = dxf<0 ? -dxf*dxf : dxf*dxf;
        syf = dyf<0 ? -dyf*dyf : dyf*dyf;
        sxi = sxf * wid/2 + wid/2;
        syi = syf * hei/2 + hei/2;

        int si = wid * syi + sxi;
        dst[di*3+0] = src[si*3+0];
        dst[di*3+1] = src[si*3+1];
        dst[di*3+2] = src[si*3+2];
    }
}

__global__
void unpuk(char *dst, char *src, int wid, int hei)
{
    int stride = gridDim.x * blockDim.x,
        idx = blockDim.x * blockIdx.x + threadIdx.x;

    for (int di = idx; di < wid * hei; di += stride)
    {
        int dxi, dyi, sxi, syi;
        float dxf, dyf, sxf, syf;

        dxi = di % wid;
        dyi = di / wid;
        dxf = (float )(dxi - wid/2) / (wid/2);
        dyf = (float )(dyi - hei/2) / (hei/2);
        sxf = dxf<0 ? -sqrt(-dxf) : sqrt(dxf);
        syf = dyf<0 ? -sqrt(-dyf) : sqrt(dyf);
        sxi = sxf * wid/2 + wid/2;
        syi = syf * hei/2 + hei/2;

        int si = wid * syi + sxi;
        dst[di*3+0] = src[si*3+0];
        dst[di*3+1] = src[si*3+1];
        dst[di*3+2] = src[si*3+2];
    }
}

int main(int argc, char **argv)
{
    char *buf, *_sbuf, *_dbuf;
    size_t sz;
    uint32_t off, wid, hei;

    fseek(stdin, 0, SEEK_END);
    sz = ftell(stdin);
    rewind(stdin);
    buf = (char *)malloc(sz);
    fread(buf, 1, sz, stdin);

    off = *(uint32_t *)&buf[0xa];
    wid = *(uint32_t *)&buf[0x12];
    hei = *(uint32_t *)&buf[0x16];

    cudaMalloc(&_sbuf, sz);
    cudaMemcpy(_sbuf, buf, sz, cudaMemcpyHostToDevice);

    cudaMalloc(&_dbuf, sz);
    cudaMemcpy(_dbuf, buf, sz, cudaMemcpyHostToDevice);

    if (argc == 1)
        puk<<<0x100, 0x100>>>(_dbuf + off, _sbuf + off, wid, hei);
    else
        unpuk<<<0x100, 0x100>>>(_dbuf + off, _sbuf + off, wid, hei);
    cudaDeviceSynchronize();

    cudaMemcpy(buf, _dbuf, sz, cudaMemcpyDeviceToHost);

    fwrite(buf, 1, sz, stdout);
}
