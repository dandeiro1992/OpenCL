
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#define PROMIEN 1



int main(int argc, char ** argv)
{
	// 1. Get a platform.
	

	cl_uint platformCount;
	cl_uint deviceCount;
	int bestDevice = -1;
	int bestDeviceWg=0;
	int bestDeviceCu=0;
	int NWITEMS;
	cl_uint computeUnits;
	cl_uint workGroupSize;
	cl_device_type deviceType;

	char deviceName[1024];

	clGetPlatformIDs(0, NULL, &platformCount);
	if (platformCount == 0){
		printf("No platform founded. /n");
		return 1;
	}
	cl_platform_id* platform = (cl_platform_id*)malloc(sizeof (cl_platform_id)*platformCount);
	clGetPlatformIDs(platformCount, platform, NULL);


	cl_device_id* device=NULL;
	

	// 2. Find a gpu device.
	
	for (cl_uint i = 0; i < platformCount; i++){

		clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
		device = (cl_device_id*)malloc(sizeof (cl_device_id)*deviceCount);
		clGetDeviceIDs(platform[i], CL_DEVICE_TYPE_ALL, deviceCount, device, &deviceCount);
		

		clGetPlatformInfo(platform[i], CL_PLATFORM_NAME, sizeof(deviceName), deviceName, NULL);
		//printf("platform name %s \n", deviceName);

		for (cl_uint j = 0; j < deviceCount; j++){

			clGetDeviceInfo(device[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &computeUnits, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &workGroupSize, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, NULL);
			//printf("         device name %s,   cu: %d    wg:   %d \n", deviceName, computeUnits, workGroupSize);

			if (deviceType == CL_DEVICE_TYPE_GPU){
				bestDevice = j;
				bestDeviceCu = computeUnits;
				bestDeviceWg = workGroupSize;

			}
		}
		if (bestDevice==-1)
		for (cl_uint j = 0; j < deviceCount; j++){

			clGetDeviceInfo(device[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &computeUnits, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(cl_uint), &workGroupSize, NULL);
			clGetDeviceInfo(device[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &deviceType, NULL);

			if (computeUnits > bestDeviceCu){
				bestDevice = j;
				bestDeviceCu = computeUnits;
				bestDeviceWg = workGroupSize;

			}
		}

	}
	

	NWITEMS = bestDeviceWg*bestDeviceCu;

	// 3. Create a context and command queue on that device.
	cl_context context = clCreateContext(NULL, deviceCount, device, NULL, NULL, NULL);
	cl_command_queue queue = clCreateCommandQueue(context, device[bestDevice], 0, NULL);




	FILE *fp;
	const char fileName[] = "./kernel.cl";
	char* source;
	size_t programSize;
	fp = fopen(fileName, "rb");
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET); 
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		system("PAUSE");
		return 1;
	}
	source = (char *)malloc(size);
	programSize = fread(source, 1, size, fp);
	fclose(fp);




	// 4. Perform runtime source compilation, and obtain kernel entry point.
	cl_int errcode;
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, (const size_t *)&programSize, &errcode);
	errcode=clBuildProgram(program, 1, device, NULL, NULL, NULL);
	if (errcode != CL_SUCCESS){
		printf(" Blad kompilacji \n");
		system("PAUSE");
		return 0;
	}
	cl_kernel kernel = clCreateKernel(program, "dist", NULL);




	// 5. Create a data buffer.
	cl_mem buffer = clCreateBuffer(context,	CL_MEM_READ_WRITE, NWITEMS * sizeof(cl_float), NULL, NULL);

	

	cl_float *tmp=(cl_float*)malloc(sizeof (cl_float)*NWITEMS);
	cl_float *tmp2 = (cl_float*)malloc(sizeof (cl_float)*NWITEMS);

	
	for (int i = 0; i < NWITEMS; i++){
		tmp[i] = { ((cl_float)rand() / (cl_float)(RAND_MAX)) * (cl_float)PROMIEN }; 
	
	}

	for (int i = 0; i < NWITEMS; i++){
		tmp2[i] = { ((cl_float)rand() / (cl_float)(RAND_MAX)) * (cl_float)PROMIEN };

	}
	

	cl_mem coordX = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, NWITEMS * sizeof(cl_float), tmp, NULL); // wej
	cl_mem coordY = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, NWITEMS * sizeof(cl_float), tmp2, NULL); 
	
	cl_float suma = 0;
	cl_uint prom = PROMIEN;

	// 6. Launch the kernel. Let OpenCL pick the local work size.
	size_t global_work_size = NWITEMS;
	size_t local_work_size = bestDeviceWg;
	clSetKernelArg(kernel, 0, sizeof(buffer), (void*)&buffer);
	clSetKernelArg(kernel, 1, sizeof(coordX), (void*)&coordX);
	clSetKernelArg(kernel, 2, sizeof(coordY), (void*)&coordY);
	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, NULL);

	clFinish(queue);




	
	// 7. Look at the results via synchronous buffer map.
	cl_float *ptr;
	ptr = (cl_float *)clEnqueueMapBuffer(queue, buffer, CL_TRUE, CL_MAP_READ, 0, NWITEMS * sizeof(cl_float), 0, NULL, NULL, NULL);
	
	for (int i = 0; i < NWITEMS; i++){
		if (ptr[i]<=PROMIEN*PROMIEN)suma++;
	}

	printf("PI= %f\n", 4 * (suma/ (float)NWITEMS));

	clReleaseMemObject(coordX);
	clReleaseMemObject(coordY);
	clReleaseMemObject(buffer); 
	clReleaseProgram(program);
	clReleaseKernel(kernel); 
	clReleaseCommandQueue(queue); 
	clReleaseContext(context);



	system("PAUSE");
	return 0;
}