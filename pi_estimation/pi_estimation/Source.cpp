#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip> 
#include <CL/cl.h>


void randomizeArray(cl_double* data, size_t vectorSize)
{

	for (size_t i = 0; i < vectorSize; ++i)
	{
		data[i] = double(i);
	}
}

int main()
{
	cl_int error = CL_SUCCESS;

	// Get platform number.
	cl_uint platformNumber;
	cl_uint deviceNumber;
	cl_uint punits, temp_pu;
	size_t wgroup, temp_wg;
	cl_bool integrated, temp_integra;
	cl_device_type devtype, temp_type;
	cl_platform_id* platformIds;
	cl_device_id* deviceIds;
	cl_device_id deviceIdFinal;
	char name[1024] = { '\0' };

	error = clGetPlatformIDs(0, NULL, &platformNumber);

	if (0 == platformNumber)
	{
		std::cout << "No OpenCL platforms found." << std::endl;

		return 0;
	}

	// Get platform identifiers.
	platformIds = new cl_platform_id[platformNumber];
	error = clGetPlatformIDs(platformNumber, platformIds, NULL);

	// Get platform info.
	for (cl_uint i = 0; i < platformNumber; ++i)
	{
		error = clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, 1024, &name, NULL);
		std::cout << "Platform:\t" << name << std::endl;
		std::cout << std::endl;
		error = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceNumber);

		if (0 == deviceNumber)
		{
			std::cout << "No OpenCL devices found on platform " << 1 << "." << std::endl;
		}

		// Get device identifiers.
		deviceIds = new cl_device_id[deviceNumber];

		error = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, deviceNumber, deviceIds, &deviceNumber);

		// Get device info.
		for (cl_uint j = 0; j < deviceNumber; j++)
		{
			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_NAME, 1024, &name, NULL);
			std::cout << "Device:\t\t" << name << std::endl;
			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_VERSION, 1024, &name, NULL);
			std::cout << "Version:\t" << name << std::endl;

			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &punits, NULL);
			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &wgroup, NULL);
			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &integrated, NULL);
			error = clGetDeviceInfo(deviceIds[j], CL_DEVICE_TYPE, sizeof(cl_device_type), &devtype, NULL);
			if (i == 0 && j == 0)
			{
				deviceIdFinal = deviceIds[j];
				temp_pu = punits;
				temp_wg = wgroup;
				temp_integra = integrated;
				temp_type = devtype;
			}
			std::cout << std::endl;
			if (temp_type == 2 && devtype == 4)
			{
				deviceIdFinal = deviceIds[j];
			}
			else
			{
				if (temp_integra == 1 && integrated == 0)
				{
					deviceIdFinal = deviceIds[j];
				}
				else if (devtype == 4 && integrated == 0)
				{
					if (temp_pu*temp_wg < punits*wgroup)
					{
						deviceIdFinal = deviceIds[j];
					}
				}
			}
			error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &temp_pu, NULL);
			error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &temp_wg, NULL);
			error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_HOST_UNIFIED_MEMORY, sizeof(cl_bool), &temp_integra, NULL);
			error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_TYPE, sizeof(cl_device_type), &temp_type, NULL);
		}
		delete[] deviceIds;		
	}
	std::cout << "Device was chosen" << std::endl;
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_NAME, 1024, &name, NULL);
	std::cout << "Name:\t\t" << name << std::endl;
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_VENDOR, 1024, &name, NULL);
	std::cout << "Vendor:\t\t" << name << std::endl;
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_VERSION, 1024, &name, NULL);
	std::cout << "Version:\t" << name << std::endl;
	std::cout << std::endl;
	// Get device count.
	

	

	// Allocate and initialize host arrays
	//size_t vectorSize = 67108864;
	size_t vectorSize = 67108864;
	size_t localWorkSize = 16;

	cl_double* a = new cl_double[vectorSize];
	cl_double* b = new cl_double[vectorSize/2];
	srand(time(NULL));
	randomizeArray(a, vectorSize);


	// Create the OpenCL context.
	cl_context context = clCreateContext(0, 1, &deviceIdFinal, NULL, NULL, NULL);

	if (NULL == context)
	{
		std::cout << "Failed to create OpenCL context." << std::endl;
	}

	// Create a command-queue
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceIdFinal, 0, &error);

	

	FILE *fp;
	const char fileName[] = "./Add.cl";
	char* source;
	size_t programSize;
	fp = fopen(fileName, "rb");
	fseek(fp, 0, SEEK_END); // seek to end of file
	size_t size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source = (char *)malloc(size);
	programSize = fread(source, 1, size, fp);
	fclose(fp);


	// Create the program.
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source, (const size_t *)&programSize, &error);
	error = clBuildProgram(program, 1, &deviceIdFinal, NULL, NULL, NULL);

	// Create the kernel.
	cl_kernel kernel = clCreateKernel(program, "Add", &error);
	cl_kernel initialization = clCreateKernel(program, "init", &error);

	// Allocate the OpenCL buffer memory objects for source and result on the device.
	cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_double) * vectorSize, NULL, &error);
	cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(cl_double) * vectorSize/2, NULL, &error);

	// Asynchronous write of data to GPU device.
	error = clEnqueueWriteBuffer(commandQueue, bufferA, CL_TRUE, 0, sizeof(cl_double) * vectorSize/2, a, 0, NULL, NULL);

	// Set the Argument values.
	error = clSetKernelArg(initialization, 0, sizeof(cl_mem), (void*)&bufferA);
	error = clSetKernelArg(initialization, 1, sizeof(cl_mem), (void*)&bufferB);

	// Launch kernel.
	error = clEnqueueNDRangeKernel(commandQueue, initialization, 1, NULL, &vectorSize, &localWorkSize, 0, NULL, NULL);

	// Read back results and check accumulated errors.
	error = clEnqueueReadBuffer(commandQueue, bufferA, CL_TRUE, 0, sizeof(cl_double) * vectorSize, a, 0, NULL, NULL);


	/*for (size_t i = 0; i < vectorSize; i++)
	{
		std::cout << i << " " << a[i] << std::endl;
	}*/

	// Print results.
	for (size_t i = vectorSize; i > 1; i = i / 2)
	{
		
		error = clEnqueueWriteBuffer(commandQueue, bufferA, CL_TRUE, 0, sizeof(cl_double) * i, a, 0, NULL, NULL);
		// Set the Argument values.
		error = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&bufferA);
		error = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
		// Launch kernel.
		error = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &i, &localWorkSize, 0, NULL, NULL);
		// Read back results and check accumulated errors.
		error = clEnqueueReadBuffer(commandQueue, bufferB, CL_TRUE, 0, sizeof(cl_double) * i/2, b, 0, NULL, NULL);
		clFinish(commandQueue);
		delete[] a;
		a = new cl_double[i/2];
		for (int j = 0; j < i/2; j++)
		{
			a[j] = b[j];
		}
		if (i > 2)
		{
			delete[] b;
			b = new cl_double[i / 4];
		}		
	}
	std::cout << " Oszacowanie liczby PI dla " << std::setprecision(16) << std::fixed << vectorSize << " powtorzen wynosi: " << sqrt(a[0]) << std::endl;
	


	// Cleanup and free memory.
	clFlush(commandQueue);
	clFinish(commandQueue);
	clReleaseKernel(kernel);
	clReleaseKernel(initialization);
	clReleaseProgram(program);
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	delete[] a;
	delete[] b;

	delete[] platformIds;
	

	// Press Enter, to quit application.
	std::cin.get();

	return 0;
}