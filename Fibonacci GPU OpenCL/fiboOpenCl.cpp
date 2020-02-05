#include "fib.h"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CL/opencl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Error please give a number to compute \n");
        return 1;
      }
    
    int n = atoi(argv[1]);
    char* source_data = \
                        "int fibonacci(int n) {                 \n" \
                        "  int t = 0;                           \n" \
                        "  int u = 1;                           \n" \
                        "  int v = 1;                           \n" \
                        "                                       \n" \
                        "  if(n <= 0){                          \n" \
                        "       return 0;                       \n" \
                        "  }                                    \n" \
                        "  if(n > 0 && n < 3){                  \n" \
                        "       return 1;                       \n" \
                        "  }                                    \n" \
                        "  for (int i = 2; i < n; i++) {        \n" \
                        "    t = u + v;                         \n" \
                        "    u = v;                             \n" \
                        "    v = t;                             \n" \
                        "  }                                    \n" \
                        "  return v;                            \n" \
                        "}                                      \n" \
                        "                                       \n" \
                        "kernel void fib(                       \n" \
                        "    global const float *a,             \n" \
                        "    global float *r) {                 \n" \
                        "  unsigned int id = get_global_id(0);  \n" \
                        "  r[id] = fibonacci(a[id]);            \n" \
                        "}                                      \n" \
                        "";
    size_t source_size = strlen(source_data);

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_int ret;
    cl_uint num_devices, num_platforms;

    //Get the Platform ID
    ret = clGetPlatformIDs(1,&platform_id,&num_platforms);
    if (ret != CL_SUCCESS) {
        fprintf(stderr, "cannot get platform IDs: %d\n", ret);
        return 1;
    }

    ret = clGetDeviceIDs(platform_id,CL_DEVICE_TYPE_DEFAULT,1,&device_id,&num_devices);
      if (ret != CL_SUCCESS) {
        fprintf(stderr, "cannot get device ID: %d\n", ret);
        return 1;
      }
    
    cl_context context = clCreateContext(NULL,1,&device_id,NULL,NULL,&ret);
    
    cl_command_queue command_queue = clCreateCommandQueue(context,device_id,0,&ret);

    cl_program program = clCreateProgramWithSource(context,1,(const char **) &source_data,(const size_t *) &source_size,&ret);
    
    ret = clBuildProgram(program,1,&device_id,NULL,NULL,NULL);

    cl_kernel kernel = clCreateKernel(program,"fib",&ret);

    float a[1] = { (float)n };
    float r[1] = { 0 };

    cl_mem ma = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(a),NULL,&ret);
    cl_mem mr = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(r),NULL,&ret);

    ret = clEnqueueWriteBuffer(command_queue,ma,CL_TRUE,0,sizeof(a),a,0,NULL,NULL);
    ret = clEnqueueWriteBuffer(command_queue,mr,CL_TRUE,0,sizeof(r),r,0,NULL,NULL);

    ret = clSetKernelArg(kernel,0,sizeof(ma),(void *)&ma);
    ret = clSetKernelArg(kernel,1,sizeof(mr),(void *)&mr);

    size_t work[2] = { 1, 0 };
    ret = clEnqueueNDRangeKernel(command_queue,kernel,1,NULL,work,work,0,NULL,NULL);
    ret = clEnqueueReadBuffer(command_queue,mr,CL_TRUE,0,sizeof(r),r,0,NULL,NULL);
    
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    ret = clReleaseMemObject(ma);
    ret = clReleaseMemObject(mr);

    printf("%ld\n", (long)r[0]);
    return 0;
}