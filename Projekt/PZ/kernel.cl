#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

void int_to_char(int* source, unsigned char* cel, int rozmiar)
{
    int i;
    for(i=0; i<rozmiar;i++)
    {

        cel[i*4]=(source[i] & 0xFF000000)>>24;
        cel[i*4+1]=(source[i] & 0x00FF0000)>>16;
        cel[i*4+2]=(source[i] & 0x0000FF00)>>8;
        cel[i*4+3]=source[i] & 0x000000FF;
    }

}


void sha256(unsigned char* tekst, int length, unsigned char* result)
{
    int i;
    int wynik[8];

    unsigned int K[]={0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    unsigned char a[64];
    unsigned int W[64];
    int round=0;
    long int counter=length*8;
    int work=1;
    //                    0            1           2           3           4           5           6           7
    //                    a            b           c           d           e           f           g           h
    unsigned int H[]={0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
    unsigned int Val[8];

    while(work)
    {
        if(length<56)
        {
            for(i=0; i<length; i++) a[i]=tekst[i+round];
            a[length]=128;
            for(i=length+1;i<63;i++) a[i]=0;

            a[63]=(counter & 0xFFFF);  counter=counter>>8;
            a[62]=(counter & 0xFFFF);  counter=counter>>8;
            a[61]=(counter & 0xFFFF);  counter=counter>>8;;
            a[60]=counter>>8;;
            work=0;
        }


        else
        {
            for(i=0; i<length && i<64 ; i++) a[i]=tekst[i+round];
            if(length>=56 && length< 64) a[length]=128;
            for(i=length+1;i<64;i++) a[i]=0;
            length=length-64;
            if(length<0) length=-1;
        }




        for(i=0 ; i<64; i=i+4)
        {
            W[i/4]=(a[i]<<24)|(a[i+1]<<16)|(a[i+2]<<8)|a[i+3];
        }


        for(i=16 ; i<64; i++)
        {//                                                                            W(t-7)                                                                                  W(t-16)
            W[i]=(((W[i-2]>>17)|(W[i-2]<<(32-17))) ^ ((W[i-2]>>19)|(W[i-2]<<(32-19))) ^  W[i-2]>>10) + W[i-7] + (((W[i-15]>>7)|(W[i-15]<<(32-7))) ^ ((W[i-15]>>18)|(W[i-15]<<(32-18))) ^  (W[i-15]>>3)) + W[i-16];
        }

        for(i=0;i<8; i++) Val[i]=H[i];
        unsigned int T1,T2;



        for(i=0; i<64; i++)
        {
            T1=Val[7]  +(  ((Val[4]>>6)|(Val[4]<<(32-6)))^((Val[4]>>11)|(Val[4]<<(32-11)))^((Val[4]>>25)|(Val[4]<<(32-25)))  )+(  (Val[4]&Val[5])^((~Val[4])&Val[6])  )+  K[i]   +  W[i];
            T2=(((Val[0]>>2)|(Val[0]<<(32-2)))^((Val[0]>>13)|(Val[0]<<19))^((Val[0]>>22)|(Val[0]<<(32-22))) ) +  ( (Val[0]&Val[1])^(Val[0]&Val[2])^(Val[1]&Val[2]));
            Val[7]=Val[6];
            Val[6]=Val[5];
            Val[5]=Val[4];
            Val[4]=Val[3]+T1;
            Val[3]=Val[2];
            Val[2]=Val[1];
            Val[1]=Val[0];
            Val[0]=T1+T2;
        }

            H[7]=Val[7]+H[7];
            H[6]=Val[6]+H[6];
            H[5]=Val[5]+H[5];
            H[4]=Val[4]+H[4];
            H[3]=Val[3]+H[3];
            H[2]=Val[2]+H[2];
            H[1]=Val[1]+H[1];
            H[0]=Val[0]+H[0];

            round=round+64;;
    }



    for(i=0; i<8; i++)
    {
        wynik[i]=H[i];
        //printf("%.8x \n",wynik[i]);
    }
    int_to_char(wynik, result,8);

}


void hmac_sha256(unsigned char *key, unsigned char *text, unsigned char *output, unsigned int key_length, unsigned int text_length)
{
    int i;
	//printf("%u\n", text_length);
	//for(i=0;i<key_length;i++) printf("%x ", key[i]);
	//printf("\n");
	//for(i=0;i<text_length;i++) printf("%x ", text[i]);
    unsigned char keyK[64];
    unsigned char ipad[65]="66666666666666666666666666666666666666666666666666666666666666666";
    unsigned char opad[65]="\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\";
	
    unsigned char blockB[64];
	unsigned char blockA[64];

    unsigned char conc[192];
    i=0;

    // padding klucza hmac
    if(key_length>64)
    {
        sha256(key,key_length,keyK);
        i=32;
        key_length=32;
    }
	
    for(i=0; i<key_length; i++) keyK[i]=key[i];
    for(i=key_length;i<64;i++) keyK[i]=0;
    for(i=0; i<64; i++)
    {
        conc[i]=ipad[i]^keyK[i]; // xor ipadu
    }

    for(i=64; i<64+text_length; i++)
    {  //konkatenacja wiadomosci z ipadem
        conc[i]=text[i-64];
    }
	
    sha256(conc,64+text_length,blockB); // sha, blockB rozmiar 32
    for(i=0; i<64; i++)
    {
		blockA[i]=opad[i]^keyK[i]; // xor opadu
    }


    for(i=0; i<64; i++)
    {
    conc[i]=blockA[i];
    }

    for(i=64; i<96;i++)
    {
    conc[i]=blockB[i-64];
    }
    sha256(conc,96,blockA);
	for(i=0;i<32;i++) output[i]=blockA[i];
		//printf("%.2x", blockA[i]);
}

void pbkdf2(unsigned char *password, unsigned char *salt, unsigned int c, unsigned int dkLen, unsigned int pass_length, unsigned int salt_length, unsigned char *T)
{
    unsigned int l;
    if(dkLen%32==0) l=(dkLen/32);
    else l=(dkLen/32)+1;
    unsigned int r=dkLen-((l-1)*32);
    unsigned char F[32];
    unsigned char U[32];
    unsigned char tmp[32];
    unsigned char saltconti[256];
    unsigned char conti[4];
    unsigned int i, j, k;
    for(k=0;k<salt_length;k++)
    {
        saltconti[k]=salt[k];
    }
    for(i=0;i<l;i++)
    {
        int bb=i+1;
        int_to_char(&bb, conti, 1);
        for(k=salt_length;k<salt_length+4;k++)
        {
            saltconti[k]=conti[k-salt_length];
        }
        hmac_sha256(password,saltconti,U, pass_length, salt_length+4);
        for(k=0;k<32;k++)
        {
            F[k]=U[k];
            //printf("%.2x",F[k]);
        }
        for(j=1;j<c;j++)
        {
            hmac_sha256(password,U,tmp, pass_length, 32);
            for(k=0;k<32;k++)
            {
                U[k]=tmp[k];
                //printf("%.2x",U[k]);
            }
            for(k=0;k<32;k++)
            {
                F[k]=F[k]^U[k];
            }
        }
		if(i!=(l-1))
		{
			for(j=i*32;j<i*32+32;j++)
			{
				T[j]=F[j-i*32];
				//printf("%.2x",F[j-i*32]);
			}
		}
        else
		{
			for(j=i*32;j<i*32+r;j++)
			{
				T[j]=F[j-i*32];
				//printf("%.2x",F[j-i*32]);
			}
		}
    }

}

__kernel void func(__global unsigned char *haslo, __global unsigned char *sol, __global unsigned int *c, __constant unsigned int *dkLen, __global unsigned int *pass_length, __global unsigned int *salt_length, __global unsigned char *T)
{

	int i;
	int id=get_global_id(0);
	unsigned char password[256];
	for(i=pass_length[0]*id;i<pass_length[0]*id+pass_length[0];i++) password[i-pass_length[0]*id]=haslo[i];
	//for(i=0;i<pass_length[0];i++) printf("%c", password[i]);
	unsigned char salt[256];
	for(i=0;i<salt_length[0];i++) salt[i]=sol[i];
	unsigned char result[1024];
    pbkdf2(password, salt, c[0], dkLen[0], pass_length[0], salt_length[0], result);
	for(i=dkLen[0]*id;i<dkLen[0]*id+dkLen[0];i++) T[i]=result[i-dkLen[0]*id];
	//for(i=0;i<dkLen[0];i++) printf("Id %u %x \n",id, result[i]);



  
}
