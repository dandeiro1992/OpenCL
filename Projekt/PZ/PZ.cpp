#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include <CL/cl.h>
#endif
#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#endif
#define _SCL_SECURE_NO_WARNINGS  
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include<iostream>
#include <time.h>
#include <math.h>
//#define MAX_SOURCE_SIZE (0x100000)
//#define MEM_SIZE (128)
using namespace std;
int number_of_iterations = 0;

bool inicjalizacja = true;

/*void Permutenr(const string& input, string output, int r)
{
	if (number_of_iterations == global_item_size)
	{
		//Sleep(200);

		cout << "mam " << global_item_size << endl;

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
}*/

/*char *losuj(char* start, char*alfabet, int ile)
{
	char *temp = new char[ile];
	int *iterators
}*/

char* Permutacje(const string& input, string output, int r, cl_context context, cl_command_queue command_queue, cl_mem &TC, int &liczba_charow, cl_mem &signs, cl_mem &wyjscie, char *tablica_charow, int dlugosc_klucza, cl_program program, size_t source_size, char *source_str, cl_device_id device_id, cl_kernel &kernel, unsigned char *rezultat, char* salt, unsigned int &dkLen, unsigned int &c, unsigned int &salt_length, cl_mem &bufferB, cl_mem &bufferC, cl_mem &bufferD, cl_mem &bufferF, cl_mem &bufferG, size_t global_item_size, size_t local_item_size, string *permutacje, unsigned char* search)
{
	bool inicjacja = true;
	int liczba_iteracji = 0;
	bool carry = true;
	int dlugosc_uzywany_alfabet = input.length();
	string koncowy_string = "";
	for (int i = 0; i < dlugosc_klucza; i++)
		koncowy_string += input[input.length() - 1];
	string text = "";
	for (int i = 0; i < dlugosc_klucza; i++)
		text += input[0];
	/*for (int i = 0; i < global_item_size*dkLen; i++)
		tablica_charow[i] = 0;*/
	for (int i = 0; i < dlugosc_klucza; i++)
			tablica_charow[i] = text[i];
	liczba_iteracji = 1;
	while (text != koncowy_string)
	{
		carry = true;
		for (int i = 0; i < dlugosc_klucza; i++)
		{
			if (carry)
			{
				text[i] = input[(input.find(text[i]) + 1) % input.length()];
				if (text[i] != input[0])
				{
					carry = false;
				}
			}
		}
		//liczba_iteracji++;
		if (liczba_iteracji < global_item_size)
		{
			for (int i = 0; i < dlugosc_klucza; i++)
					{
						tablica_charow[liczba_iteracji*dlugosc_klucza + i] = text[i];
					}
			liczba_iteracji++;
			/*for (int k = 0; k < dkLen; k++)
			{
				cout << text[k];
			}*/
			//cout << endl;
			//Sleep(1000);
		}
		else
		{
			//std::cout << "bede zaczynal wykonywane kerneli" << endl;
			if (inicjacja)
			{
				//cout << salt << " sol  " << endl;
				//cout << "dkLen    " << dkLen << endl;
				//cout << "salt length   " << salt_length << endl;
				//cout << "c   " << c << endl;
				//cout << "dlugosc klucza wejs  " << dlugosc_klucza << endl;
				//cout << "search  " << search << endl;
				/*for (int i = 0; i < 1000; i++)
					cout << tablica_charow[i];*/
				//Sleep(10000);
				inicjacja = false;
				int ret;
				TC = clCreateBuffer(context, CL_MEM_READ_WRITE, liczba_charow*sizeof(char), NULL, &ret);
				signs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
				wyjscie = clCreateBuffer(context, CL_MEM_READ_WRITE, global_item_size*dkLen*sizeof(char), NULL, &ret);
				//cout << "jestem tutaj 2" << endl;
				bufferB = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char)* strlen(salt), NULL, &ret);
				bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
				bufferD = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
				bufferF = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
				bufferG = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char)* dkLen, NULL, &ret);
				/* Copy input data to the memory buffer */

				ret = clEnqueueWriteBuffer(command_queue, TC, CL_TRUE, 0, liczba_charow*sizeof(char), tablica_charow, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera" << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, signs, CL_TRUE, 0, sizeof(int), &dlugosc_klucza, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal" << endl;
				}
				//cout << "jestem tutaj 3" << endl;

				ret = clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, sizeof(char)* strlen(salt), salt, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal" << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(int), &c, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal" << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, bufferD, CL_TRUE, 0, sizeof(int), &dkLen, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalalfuffer D" << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, bufferF, CL_TRUE, 0, sizeof(int), &salt_length, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal fuffer F" << endl;
				}

				//Sleep(200);


				/* Set OpenCL kernel arguments */
				ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&TC);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd TC" << endl;
				}

				ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd sol" << endl;
				}
				ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferC);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd count" << endl;
				}
				ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferD);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd dklen dlugosc ciagu wyjsciowego" << endl;
				}
				ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&signs);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd signs" << endl;
				}
				ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferF);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd dlugosci soli" << endl;
				}
				ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&wyjscie);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd wyjscie" << endl;
				}


				/* Execute OpenCL kernel as data parallel */
				ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

				/* Transfer result to host */
				ret = clEnqueueReadBuffer(command_queue, wyjscie, CL_TRUE, 0, global_item_size*dkLen * sizeof(char), rezultat, 0, NULL, NULL);

				//cout << "jestem tutajaaaaa" << endl;
				for (int i = 0; i < global_item_size*dkLen; i = i + dkLen)
				{
					//for (int j = i; j < i + dkLen; j++) printf("%.2x", rezultat[j]);
					//cout << endl;
					//Sleep(1000);
					int flag = 1;
					for (int j = i; j < i + dkLen; j++)
					{
						if (rezultat[j] != search[j - i])
						{
							flag = 0;
							//printf("%u", flag);
						}
						//printf("%.2x %.2x %u \n", rezultat[j], search[j - 1], flag);
					}
					//cout << endl;
					if (flag == 1)
					{
						char *temp = new char[dlugosc_klucza + 1];
						for (int j = (i / dkLen)*dlugosc_klucza; j < (i / dkLen)*dlugosc_klucza + dlugosc_klucza; j++) temp[j - (i / dkLen)*dlugosc_klucza] = tablica_charow[j];
						temp[dlugosc_klucza] = '\0';
						for (int j = 0; j < strlen(temp); j++) printf("%c", temp[j]);
						cout << endl;
					//	cout << "LOLOLO" << endl;
						return temp;
						

						//cout << endl;
					}
				}
				//cout << endl;
				liczba_iteracji = 0;
			}
			else
			{
				int ret;
				/* Copy input data to the memory buffer */

				ret = clEnqueueWriteBuffer(command_queue, TC, CL_TRUE, 0, liczba_charow*sizeof(char), tablica_charow, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera   22222" << endl;
					cout << ret << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, signs, CL_TRUE, 0, sizeof(int), &dlugosc_klucza, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal 2222" << endl;
					cout << ret << endl;
				}
				//cout << "jestem tutaj 3" << endl;

				ret = clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, sizeof(char)* strlen(salt), salt, 0, NULL, NULL);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd buffera laalal 3333" << endl;
					cout << ret << endl;
				}
				ret = clEnqueueWriteBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(int), &c, 0, NULL, NULL);
				ret = clEnqueueWriteBuffer(command_queue, bufferD, CL_TRUE, 0, sizeof(int), &dkLen, 0, NULL, NULL);
				ret = clEnqueueWriteBuffer(command_queue, bufferF, CL_TRUE, 0, sizeof(int), &salt_length, 0, NULL, NULL);

				//Sleep(200);

				/* Set OpenCL kernel arguments */
				ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&TC);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd TC" << endl;
				}

				ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd sol" << endl;
				}
				ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferC);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd count" << endl;
				}
				ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferD);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd dklen dlugosc ciagu wyjsciowego" << endl;
				}
				ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&signs);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd signs" << endl;
				}
				ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferF);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd dlugosci soli" << endl;
				}
				ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&wyjscie);
				if (ret != CL_SUCCESS)
				{
					cout << "nie moglem przepisac do dddd wyjscie" << endl;
				}



				/* Execute OpenCL kernel as data parallel */
				ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

				/* Transfer result to host */
				ret = clEnqueueReadBuffer(command_queue, wyjscie, CL_TRUE, 0, global_item_size*dkLen * sizeof(char), rezultat, 0, NULL, NULL);


				for (int i = 0; i < global_item_size*dkLen; i = i + dkLen)
				{
					//for (int j = i; j < i + dkLen; j++) printf("%.2x", rezultat[j]);
					//cout << endl;
					int flag = 1;
					for (int j = i; j < i + dkLen; j++)
					{
						if (rezultat[j] != search[j - i]) flag = 0;
						//printf("%u", flag);
					}
					//cout << endl;
					if (flag == 1)
					{
						char *temp = new char[dlugosc_klucza + 1];
						for (int j = (i / dkLen)*dlugosc_klucza; j < (i / dkLen)*dlugosc_klucza + dlugosc_klucza; j++) temp[j - (i / dkLen)*dlugosc_klucza] = tablica_charow[j];
						temp[dlugosc_klucza] = '\0';
						for (int j = 0; j < strlen(temp); j++) printf("%c", temp[j]);
						//cout << endl;
						//cout << "LOLOLO" << endl;
						return temp;
						//Sleep(10000);

						//cout << endl;
					}
				}
				liczba_iteracji = 0;
			}
		}
	}
	if (inicjacja)
	{
		inicjacja = false;
		int ret;
		TC = clCreateBuffer(context, CL_MEM_READ_WRITE, liczba_charow*sizeof(char), NULL, &ret);
		signs = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
		wyjscie = clCreateBuffer(context, CL_MEM_READ_WRITE, global_item_size*dkLen*sizeof(char), NULL, &ret);
		//cout << "jestem tutaj 2" << endl;
		bufferB = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char)* strlen(salt), NULL, &ret);
		bufferC = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
		bufferD = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
		bufferF = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &ret);
		bufferG = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(char)* dkLen, NULL, &ret);
		/* Copy input data to the memory buffer */

		ret = clEnqueueWriteBuffer(command_queue, TC, CL_TRUE, 0, liczba_charow*sizeof(char), tablica_charow, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera" << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, signs, CL_TRUE, 0, sizeof(int), &dlugosc_klucza, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal" << endl;
		}
		//cout << "jestem tutaj 3" << endl;

		ret = clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, sizeof(char)* strlen(salt), salt, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal" << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(int), &c, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal" << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, bufferD, CL_TRUE, 0, sizeof(int), &dkLen, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalalfuffer D" << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, bufferF, CL_TRUE, 0, sizeof(int), &salt_length, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal fuffer F" << endl;
		}

		//Sleep(200);


		/* Set OpenCL kernel arguments */
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&TC);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd TC" << endl;
		}

		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd sol" << endl;
		}
		ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferC);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd count" << endl;
		}
		ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferD);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd dklen dlugosc ciagu wyjsciowego" << endl;
		}
		ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&signs);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd signs" << endl;
		}
		ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferF);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd dlugosci soli" << endl;
		}
		ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&wyjscie);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd wyjscie" << endl;
		}


		/* Execute OpenCL kernel as data parallel */
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

		/* Transfer result to host */
		ret = clEnqueueReadBuffer(command_queue, wyjscie, CL_TRUE, 0, global_item_size*dkLen * sizeof(char), rezultat, 0, NULL, NULL);

		//cout << "jestem tutajaaaaa" << endl;
		for (int i = 0; i <global_item_size*dkLen; i = i + dkLen)//10*dkLen;i+=dkLen) //
		{
			//for (int j = i; j < i + dkLen; j++) printf("%.2x", rezultat[j]);
			//cout << endl;
			//Sleep(1000);
			int flag = 1;
			for (int j = i; j < i + dkLen; j++)
			{
				if (rezultat[j] != search[j - i])
				{
					flag = 0;
					//printf("%.2x %.2x ", rezultat[j], search[j - 1]);
				}
				//printf("%u", flag);
			}
			//cout << endl;
			if (flag == 1)
			{
				char *temp = new char[dlugosc_klucza + 1];
				for (int j = (i / dkLen)*dlugosc_klucza; j < (i / dkLen)*dlugosc_klucza + dlugosc_klucza; j++) temp[j - (i / dkLen)*dlugosc_klucza] = tablica_charow[j];
				temp[dlugosc_klucza] = '\0';
				for (int j = 0; j < strlen(temp); j++) printf("%c", temp[j]);
				//cout << endl;
				//cout << "LOLOLO" << endl;
				return temp;
				//Sleep(10000);

				//cout << endl;
			}
		}
	//	cout << endl;
		liczba_iteracji = 0;
	}
	else
	{
		int ret;
		/* Copy input data to the memory buffer */

		ret = clEnqueueWriteBuffer(command_queue, TC, CL_TRUE, 0, liczba_charow*sizeof(char), tablica_charow, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera   22222" << endl;
			cout << ret << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, signs, CL_TRUE, 0, sizeof(int), &dlugosc_klucza, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal 2222" << endl;
			cout << ret << endl;
		}
		//cout << "jestem tutaj 3" << endl;

		ret = clEnqueueWriteBuffer(command_queue, bufferB, CL_TRUE, 0, sizeof(char)* strlen(salt), salt, 0, NULL, NULL);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd buffera laalal 3333" << endl;
			cout << ret << endl;
		}
		ret = clEnqueueWriteBuffer(command_queue, bufferC, CL_TRUE, 0, sizeof(int), &c, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, bufferD, CL_TRUE, 0, sizeof(int), &dkLen, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, bufferF, CL_TRUE, 0, sizeof(int), &salt_length, 0, NULL, NULL);

		//Sleep(200);

		/* Set OpenCL kernel arguments */
		ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&TC);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd TC" << endl;
		}

		ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufferB);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd sol" << endl;
		}
		ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufferC);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd count" << endl;
		}
		ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufferD);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd dklen dlugosc ciagu wyjsciowego" << endl;
		}
		ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&signs);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd signs" << endl;
		}
		ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufferF);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd dlugosci soli" << endl;
		}
		ret = clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&wyjscie);
		if (ret != CL_SUCCESS)
		{
			cout << "nie moglem przepisac do dddd wyjscie" << endl;
		}



		/* Execute OpenCL kernel as data parallel */
		ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

		/* Transfer result to host */
		ret = clEnqueueReadBuffer(command_queue, wyjscie, CL_TRUE, 0, global_item_size*dkLen * sizeof(char), rezultat, 0, NULL, NULL);


		for (int i = 0; i < global_item_size*dkLen; i = i + dkLen)
		{
			//for (int j = i; j < i + dkLen; j++) printf("%.2x", rezultat[j]);
			//cout << endl;
			int flag = 1;
			for (int j = i; j < i + dkLen; j++)
			{
				if (rezultat[j] != search[j - i]) flag = 0;
				//printf("%u", flag);
			}
			//cout << endl;
			if (flag == 1)
			{
				char *temp = new char[dlugosc_klucza + 1];
				for (int j = (i / dkLen)*dlugosc_klucza; j < (i / dkLen)*dlugosc_klucza + dlugosc_klucza; j++) temp[j - (i / dkLen)*dlugosc_klucza] = tablica_charow[j];
				temp[dlugosc_klucza] = '\0';
				for (int j = 0; j < strlen(temp); j++) printf("%c", temp[j]);
				//cout << endl;
				//cout << "LOLOLO" << endl;
				return temp;
				//Sleep(10000);

			//	cout << endl;
			}
		}
		liczba_iteracji = 0;
		
	}

	return "";

}


char *hex_to_char(char *hex)
{
	int hex_len = strlen(hex);
	char *tab = new char[hex_len / 2+1];
	for (int i = 0; i < hex_len; i=i+2)
	{
		char temp[2];
		temp[0] = hex[i];
		temp[1] = hex[i + 1];
		tab[i / 2] = strtoul(temp, NULL, 16);
	}
	tab[hex_len / 2] = '\0';
	return tab;
}

int main(int argc, char ** argv)
{

	// OD bogusia
	char* password;
	char* salt;
	unsigned int c;
	unsigned int salt_length;
	unsigned int dkLen;
	/*char* inser = argv[1];
	unsigned char* search = (unsigned char*)hex_to_char(inser);
	cout << endl;
	char *endofall;
	unsigned int pass_length = strlen(password);
	unsigned int salt_length = strlen(salt);
	int dkLen = 32;
	;
	//unsigned char T[dkLen];
	for (int i = 0; i < strlen(inser)/2; i++) printf("%c", search[i]);
	cout << endl;


	//char e = 'e';
	//e = e + 10;
	//cout << e<< "e+10" << endl;
	int dlugosc_klucza = 4;
	//string alfabet = "!#$%&'()*+,./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_'abcdefghijklmnopqrstuvwxyz{|}~";
	string alfabet = "abcdefghijklmnop";
	int alfabet_dlugosc = alfabet.length();
	//Permutenr("abcdefghijklmnop", "", dlugosc_klucza);*/

	string male_litery = "abcdefghijklmnopqrstuvwxyz";
	string duze_litery = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	string cyfry = "0123456789";
	string znaki_specjalne = "!#$%&'()*+,./:;<=>?@[]^_{|}~";
	char *password_wejsciowe = argv[1];
	c = atoi(argv[2]);
	char* salt_tmp = argv[3];
	string typ_slownika = argv[4];
	int dlugosc_min_hasla = atoi(argv[5]);
	int dlugosc_max_hasla = atoi(argv[6]);
	string uzywany_alfabet = "";
	if (argc != 7)
	{
		cout << argv[0];
		cout << "Podano zla ilosc parametrow, uruchom ponoiwnie program z dobra iloscia" << endl;
		//Sleep(10000);
	}
	else
	{
		
		//cout << "rozmiar to   " << argv[1] << endl;;
		//poszukiwany[sizeof(argv[1]) + 1] = '\n';
		
		
		//cout << strlen(argv[3]) << "iwasiuk to przyalek" << endl;
		//cout << (strlen((unsigned char*)salt_tmp)) << "lollllllll" << endl;
		

		//cout << password_wejsciowe << "  poszukiwany  " << endl;
		//cout << c << "   liczba iteracji" << endl;
		//cout << salt_tmp << "   sol" << endl;
		//cout << typ_slownika << "   typ slownika" << endl;
		//cout << dlugosc_min_hasla << "   dlugosc min hasla" << endl;
		//cout << dlugosc_max_hasla << "   dlugosc mas hasla" << endl;

		if (dlugosc_min_hasla > dlugosc_max_hasla)
		{
			cout << " blednie podales zakres min max" << endl;
			return 0;
		}
		///// wybieranie alfabetu:
		
		if (typ_slownika.find("A") != -1)
		{
			uzywany_alfabet += duze_litery;
		}
		if (typ_slownika.find("a") != -1)
		{
			uzywany_alfabet += male_litery;
		}
		if (typ_slownika.find("1") != -1)
		{
			uzywany_alfabet += cyfry;
		}
		if (typ_slownika.find("*") != -1)
		{
			uzywany_alfabet += znaki_specjalne;
		}
		cout << "uzywany alfabet to  " << uzywany_alfabet << endl;

		//konwertowanie wejscia hex na wejscie normalne
		//cout << "wartosc normalna  " << hex_to_char(poszukiwany) << endl;
		password = hex_to_char(password_wejsciowe);
		dkLen = strlen(password);
		//for (int i = 0; i < 5; i++)
			//cout << password[i];
		//cout << endl;

		// konwertowanie soli na normalna
		salt = hex_to_char(salt_tmp);
		//cout << salt << "   salt" << endl;
		//salt = "pporczarnecki";
		salt_length = strlen(salt);
		//cout << strlen(salt) << "sol ma dlugosc" << endl;
		//Sleep(10000);
	}

	cl_int ret;
	//cl_platform_id platform_id = NULL;
	//cl_uint ret_num_devices;
	//cl_uint ret_num_platforms;
	//cl_device_id device_id = NULL;
	//cl_context context = NULL;
	//cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;

	

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
		error = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_CPU, 0, NULL, &deviceNumber);

		if (0 == deviceNumber)
		{
			std::cout << "No OpenCL devices found on platform " << 1 << "." << std::endl;
		}

		// Get device identifiers.
		deviceIds = new cl_device_id[deviceNumber];

		error = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_CPU, deviceNumber, deviceIds, &deviceNumber);

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
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_VERSION, 1024, &name, NULL);
	std::cout << "Mem:\t" << name << std::endl;
	std::cout << std::endl;
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &temp_pu, NULL);
	if (error != CL_SUCCESS)
	{
		//cout << "jestem tutaj kiiu" << endl;
		cout << error << endl;
		cout << "nie moglem przepisac do buffera" << endl;
	}
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &temp_wg, NULL);
	if (error != CL_SUCCESS)
	{
		//cout << "jestem tutaj kiiu" << endl;
		cout << error << endl;
		cout << "nie moglem przepisac do buffera" << endl;
	}
	size_t mitsize[3];
	error = clGetDeviceInfo(deviceIdFinal, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*3, mitsize, NULL);
	if (error != CL_SUCCESS)
	{
		//cout << "jestem tutaj kiiu" << endl;
		cout << error << endl;
		cout << "nie moglem przepisac do buffera" << endl;
	}
	size_t global_item_size = temp_pu*temp_wg;	
	size_t local_item_size = temp_wg;
	cout << mitsize[0] << endl;
	if (local_item_size > mitsize[0])
	{
		local_item_size = mitsize[0];
		global_item_size = temp_pu*local_item_size;
	}
		
	//cout << mitsize << endl;
	string *permutacje = new string[global_item_size];
	/*odczytywanie lancucha znakow 
	FILE *fp;
	const char fileName[] = "./kernel.cl";
	size_t source_size;
	char *source_str;

	// Load kernel source file 
	fp = fopen(fileName, "rb");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);*/


	FILE *fp;
	const char fileName[] = "./kernel.cl";
	char* source_str;
	size_t source_size;
	fp = fopen(fileName, "rb");
	fseek(fp, 0, SEEK_END); // seek to end of file
	size_t size = ftell(fp); // get current file pointer
	fseek(fp, 0, SEEK_SET); // seek back to beginning of file
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char *)malloc(size);
	source_size = fread(source_str, 1, size, fp);
	fclose(fp);

	/*Create OpenCL context */

	cl_context context = clCreateContext(0, 1, &deviceIdFinal, NULL, NULL, NULL);

	if (NULL == context)
	{
		std::cout << "Failed to create OpenCL context." << std::endl;
	}

	/* Create Command Queue */
	cl_command_queue commandQueue = clCreateCommandQueue(context, deviceIdFinal, 0, &error);

	/* Create kernel program from source file*/
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	ret = clBuildProgram(program, 1, &deviceIdFinal, NULL, NULL, NULL);
	if (ret != CL_SUCCESS)
	{
		//cout << "jestem tutaj kiiu" << endl;
		cout << ret << endl;
		cout << "nie moglem przepisac do buffera" << endl;
	}
	size_t len;
	clGetProgramBuildInfo(program, deviceIdFinal, CL_PROGRAM_BUILD_LOG, NULL, NULL, &len);
	char *log = new char[len];
	clGetProgramBuildInfo(program, deviceIdFinal, CL_PROGRAM_BUILD_LOG, len, log, NULL);
	//for (int i = 0; i < len; i++) std::cout << log[i];

	/* Create data parallel OpenCL kernel */
	kernel = clCreateKernel(program, "func", &ret);
	size_t param;
	size_t blad;
	clGetKernelWorkGroupInfo(kernel, deviceIdFinal, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &param, &blad);
	cout << param << endl;
	if (local_item_size > param)
	{
		local_item_size = param;
		global_item_size = temp_pu*local_item_size;
	}
	unsigned char *rezultat = new unsigned char[global_item_size*dkLen];

	

	
	cl_mem TC = NULL;
	cl_mem wyjscie = NULL;
	cl_mem signs = NULL;

	cl_mem bufferA = NULL;
	cl_mem bufferB = NULL;
	cl_mem bufferC = NULL;
	cl_mem bufferD = NULL;
	cl_mem bufferE = NULL;
	cl_mem bufferF = NULL;
	cl_mem bufferG = NULL;
	char *endofall;
	int dlugosc_klucza = dlugosc_max_hasla;
	int liczba_charow = global_item_size*dlugosc_klucza;
	char *tablica_charow = new char[global_item_size*dlugosc_klucza];
	for (dlugosc_klucza = dlugosc_min_hasla; dlugosc_klucza <= dlugosc_max_hasla; dlugosc_klucza++)
	{
		endofall = Permutacje(uzywany_alfabet, "", dlugosc_klucza, context, commandQueue, TC, liczba_charow, signs, wyjscie, tablica_charow, dlugosc_klucza, program, source_size, source_str, deviceIdFinal, kernel, rezultat, salt, dkLen, c, salt_length, bufferB, bufferC, bufferD, bufferF, bufferG, global_item_size, local_item_size, permutacje, (unsigned char*)password);
		if (endofall != ""){
			//cout << "Znalazlem:  ";
			//for (int i = 0; i < dlugosc_klucza; i++) printf("%c", endofall[i]);
			//printf("\n");
			
			break;
		}
		else
		{
			if (dlugosc_klucza == dlugosc_max_hasla) cout << "Nie znalazlem hasla" << endl;
		}
		printf("\n");
	}
	/*char *tablica_charow = new char[global_item_size*dlugosc_klucza];
	endofall = Permutacje(uzywany_alfabet, "", dlugosc_klucza, context, commandQueue, TC, liczba_charow, signs, wyjscie, tablica_charow, dlugosc_klucza, program, source_size, source_str, deviceIdFinal, kernel, rezultat, salt, dkLen, c, salt_length, bufferB, bufferC, bufferD, bufferF, bufferG, global_item_size, local_item_size, permutacje, (unsigned char*)password);
	for (int i = 0; i < dlugosc_klucza; i++) printf("%c", endofall[i]);
	printf("lol");*/

	/*for (int dlugosc_klucza = dlugosc_min_hasla; dlugosc_klucza <= dlugosc_max_hasla; dlugosc_klucza++)
	{
		char *tablica_charow = new char[global_item_size*dlugosc_klucza];
		int liczba_charow = global_item_size*dlugosc_klucza;
		endofall = Permutenr2(uzywany_alfabet, "", dlugosc_klucza, context, commandQueue, TC, liczba_charow, signs, wyjscie, tablica_charow, dlugosc_klucza, program, source_size, source_str, deviceIdFinal, kernel, rezultat, salt, dkLen, c, salt_length, bufferB, bufferC, bufferD, bufferF, bufferG, global_item_size, local_item_size, permutacje, password);
		for (int i = 0; i < dlugosc_klucza; i++) printf("%c", endofall[i]);
		printf("lol");
		if (endofall != "") break;
	}*/
	// Cleanup and free memory.
	clFlush(commandQueue);
	clFinish(commandQueue);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	clReleaseMemObject(bufferD);
	clReleaseMemObject(bufferE);
	clReleaseMemObject(bufferF);
	clReleaseMemObject(bufferG);
	clReleaseMemObject(TC);
	clReleaseMemObject(signs);
	clReleaseMemObject(wyjscie);
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	free(source_str);
	//system("pause");
	getchar();
	return 0;
}

