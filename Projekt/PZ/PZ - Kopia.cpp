#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif
#define _SCL_SECURE_NO_WARNINGS  
#define rozmiar 1024
#define liczba_kerneli 1024
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include<iostream>
#include <time.h>
#include <math.h>
#include <dos.h>
#include <windows.h>
#define MAX_SOURCE_SIZE (0x100000)
#define MEM_SIZE (128)
using namespace std;
int number_of_iterations = 0;
string *permutacje = new string[rozmiar];

void Permutenr(const string& input, string output, int r,)
{
	if (number_of_iterations == rozmiar)
	{
		//Sleep(200);

		cout << "mam " << rozmiar << endl;

		number_of_iterations = 0;
	}
	if (output.length() == r) {
		cout << output << endl;
		permutacje[number_of_iterations] = output;
		number_of_iterations++;
	}
	else {
		for (int i = 0; i<input.length(); ++i){
			Permutenr(input, output + input[i], r);
		}
	}
}

int main(int argc, char ** argv)
{
	//char e = 'e';
	//e = e + 10;
	//cout << e<< "e+10" << endl;
	int dlugosc_klucza = 4;
	//string alfabet = "!#$%&'()*+,./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_'abcdefghijklmnopqrstuvwxyz{|}~";
	string alfabet = "abcd";
	int alfabet_dlugosc = alfabet.length();
	Permutenr("abcdefghijklmnop", "", dlugosc_klucza);
	int liczba_hasel =pow(alfabet_dlugosc,dlugosc_klucza);
	cout << "jestem tutaj " << endl;
	cout << liczba_hasel << "   liczba hasel" << endl;
	for (int i = 0; i<rozmiar; i++)
	{
		cout << i << " ty wyraz tablicy wynosi:  " << permutacje[i] << endl;
	}
	cl_int ret;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;

	char *rezultat=new char[rozmiar*dlugosc_klucza];

	//////////////////////////////// robie dla stringów dlugosci 5 !!! przyk³ad taki
	//string *tablica = new string[5];
	string tablica[5] = { "abcde", "fghij", "klmno", "pqrst", "uwxyz" };
	string text = "";
	for (int i = 0; i < rozmiar; i++)
	{
		text.append(permutacje[i]);
	}
	cout << text << endl;
	cout << text.length() << endl;
	cout << text.size() << endl;
	char *tablica_charow = new char[text.length()];
	copy(text.begin(), text.end(), tablica_charow);
	cout << tablica_charow[24];
	cout << tablica_charow[0] << endl;
	int liczba_charow = text.length();
	cl_mem TC = NULL;
	cl_mem wyjscie = NULL;
	cl_mem signs = NULL;
	int liczba_liter = 4;
	// liczba_liter[0] = 5;
	// 1. Get a platform and device

	/*  Get Platform and Device Info*/
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/*odczytywanie lancucha znakow */
	FILE *fp;
	const char fileName[] = "./macierz.cl";
	size_t source_size;
	char *source_str;

	/* Load kernel source file */
	fp = fopen(fileName, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	/*Create OpenCL context */
		context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Create Buffer Object */
	TC = clCreateBuffer(context, CL_MEM_READ_WRITE, liczba_charow*sizeof(char), NULL, &ret);
	signs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
	wyjscie = clCreateBuffer(context, CL_MEM_READ_WRITE, liczba_charow*sizeof(char), NULL, &ret);
	/* Copy input data to the memory buffer */

	ret = clEnqueueWriteBuffer(command_queue, TC, CL_TRUE, 0, liczba_charow*sizeof(char), tablica_charow, 0, NULL, NULL);
	if (ret != CL_SUCCESS)
	{
		cout << "nie moglem przepisac do dddd buffera" << endl;
	}
	ret = clEnqueueWriteBuffer(command_queue, signs, CL_TRUE, 0, sizeof(int), &liczba_liter, 0, NULL, NULL);
	if (ret != CL_SUCCESS)
	{
		cout << "nie moglem przepisac do dddd buffera laalal" << endl;
	}
	/* Create kernel program from source file*/
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if (ret != CL_SUCCESS)
	{
		cout << "jestem tutaj" << endl;
		cout << ret << endl;
		cout << "nie moglem przepisac do buffera" << endl;
	}

	/* Create data parallel OpenCL kernel */
	kernel = clCreateKernel(program, "mnozenie", &ret);

	/* Set OpenCL kernel arguments */
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&TC);
	if (ret != CL_SUCCESS)
	{
		cout << "nie moglem przepisac do dddd TC" << endl;
	}
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&signs);
	if (ret != CL_SUCCESS)
	{
		cout << "nie moglem przepisac do dddd signs" << endl;
	}
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&wyjscie);
	if (ret != CL_SUCCESS)
	{
		cout << "nie moglem przepisac do dddd wyjscie" << endl;
	}

	size_t global_item_size =liczba_kerneli;
	size_t local_item_size = 1;

	/* Execute OpenCL kernel as data parallel */
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	/* Transfer result to host */
	ret = clEnqueueReadBuffer(command_queue, wyjscie, CL_TRUE, 0, rozmiar*dlugosc_klucza * sizeof(char), rezultat, 0, NULL, NULL);


	for (int i = 0; i < rozmiar*dlugosc_klucza; i++)
	{
		cout << rezultat[i];
	}
	cout << endl;
	/* Finalization */
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);

	free(source_str);
	system("pause");
	//getchar();
	return 0;
}