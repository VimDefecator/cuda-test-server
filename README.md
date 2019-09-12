Not properly tested yet, though 127.0.0.1 works well.

"server.c" is to be compiled by GCC and run on Linux machine with NVIDIA GPU and CUDA SDK installed.

"client.c" is to be compiled by CL and run on Windows machine in same network.

Usage: `client <ip> <cuda_source_file_name> <input_file_name> <output_file_name>`

Example: puk.cu should transform vaz.bmp into some kind of futuristic vehicle
