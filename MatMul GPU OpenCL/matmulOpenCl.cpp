#include "matMul.h"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CL/opencl.h>


void randomMemInit(float* data, int size){
   int i;

    for (i = 0; i < size; ++i){
       data[i] = rand()/(float)RAND_MAX;
    }
}

int main(int argc, char *argv[]) {
    int WA = 1000;
    int HA = 1000;
    int WB = 1000;
    int HB = WA;
    int WC = WB;
    int HC = HA;
    char* source_data = \
                        "__kernel void matMul(__global float* C,__global float* A,__global float* B,int wA,int wB){           \n" \
                        "  int tx = get_global_id(0);           \n" \
                        "  int ty = get_global_id(1);           \n" \
                        "  float value = 0;                     \n" \
                        "  for (int k = 0; k < wA; ++k){        \n" \
                        "       float elementA = A[ty * wA + k];\n" \
                        "       float elementB = B[k * wB + tx];\n" \
                        "        value += elementA * elementB;  \n" \
                        "  }                                    \n" \
                        "     C[ty * wA + tx] = value;          \n" \
                        "}                                      \n" \
                        "";
    size_t source_size = strlen(source_data);

    srand(2014);
     
    //Allocate host memory for matrices A and B
    unsigned int size_A = WA * HA;
    unsigned int mem_size_A = sizeof(float) * size_A;
    float* h_A = (float*) malloc(mem_size_A);
     
    unsigned int size_B = WB * HB;
    unsigned int mem_size_B = sizeof(float) * size_B;
    float* h_B = (float*) malloc(mem_size_B);
    
    //Initialize host memory
    randomMemInit(h_A, size_A);
    for (int i = 0; i<size_A;i++){
        printf("%f ", h_A[i]);
           if(((i + 1) % WC) == 0)
           printf("\n");
    }
    printf("\n");
    randomMemInit(h_B, size_B);
    for (int i = 0; i<size_B;i++){
        printf("%f ", h_B[i]);
           if(((i + 1) % WC) == 0)
           printf("\n");
    }
    printf("\n");


    unsigned int size_C = WC * HC;
    unsigned int mem_size_C = sizeof(float) * size_C;
    float* h_C = (float*) malloc(mem_size_C);
    
    
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_int ret;
    cl_uint num_devices, num_platforms;


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
    if (ret != CL_SUCCESS) {
        //printf("cl error : Unable to build program \n");
        
        size_t log_size;
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

        
        char *log = (char *) malloc(log_size);
        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        
        
        printf("details : %s\n", log);
    }

    cl_kernel kernel = clCreateKernel(program,"matMul",&ret);

    // Create the input and output arrays in device memory for our calculation
    cl_mem d_C = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_A, h_C, &ret);
    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_A, h_A, &ret);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_B, h_B, &ret);

    size_t localWorkSize[2], globalWorkSize[2];
    localWorkSize[0] = 16;
    localWorkSize[1] = 16;
    globalWorkSize[0] = 1024;
    globalWorkSize[1] = 1024;
    
    int wA = WA;
    int wC = WC;
    printf("%d \n",wA);
    printf("%d \n",wC);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem),&d_C);
    ret |= clSetKernelArg(kernel, 1, sizeof(cl_mem),&d_A);
    ret |= clSetKernelArg(kernel, 2, sizeof(cl_mem),&d_B);
    ret |= clSetKernelArg(kernel, 3, sizeof(int),(void *)&wA);
    ret |= clSetKernelArg(kernel, 4, sizeof(int),(void *)&wC);

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    ret = clEnqueueReadBuffer(command_queue,d_C,CL_TRUE,0,mem_size_C,h_C,0,NULL,NULL);
    
    printf("Matrix multiplication completed...\n");
    
    //Shutdown and cleanup
    free(h_A);
    free(h_B);
    free(h_C);
    
    
    
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    ret = clReleaseMemObject(d_A);
    ret = clReleaseMemObject(d_C);
    ret = clReleaseMemObject(d_B);

    printf("\n\nMatrix C (Results)\n");
    int i;
    for(i = 0; i < size_C; i++)
    {
       printf("%f ", h_C[i]);
       if(((i + 1) % WC) == 0)
       printf("\n");
    }
    printf("\n");
    
    return 0;
}