#include <stdio.h>
#include <stdbool.h>




void int_to_char(int* source, char* cel, int rozmiar){

int i;
for(i=0; i<rozmiar;i++){

    cel[i*4]=(source[i] & 0xFF000000)>>24;
    cel[i*4+1]=(source[i] & 0x00FF0000)>>16;
    cel[i*4+2]=(source[i] & 0x0000FF00)>>8;
    cel[i*4+3]=source[i] & 0x000000FF;
}

}






void sha256(char* tekst, int length, char* result){

    int i;
    int wynik[8];

    unsigned int K[]={0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};




  unsigned char a[64];
  unsigned int W[64];

  int round=0;
  long long int counter=length*8;
  bool work=true;

  //                    0            1           2           3           4           5           6           7
  //                    a            b           c           d           e           f           g           h
  unsigned int H[]={0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
  unsigned int Val[8];



  while(work){



  if(length<56){
      for(i=0; i<length; i++)
      a[i]=tekst[i+round];

      a[length]=128;

      for(i=length+1;i<63;i++)
      a[i]=0;



      a[63]=(counter & 0xFFFF);  counter=counter>>8;
      a[62]=(counter & 0xFFFF);  counter=counter>>8;
      a[61]=(counter & 0xFFFF);  counter=counter>>8;;
      a[60]=counter>>8;;
      work=false;

}


 else{
      for(i=0; i<length && i<64 ; i++)
      a[i]=tekst[i+round];

      if(length>=56 && length< 64) a[length]=128;

      for(i=length+1;i<64;i++)
      a[i]=0;

      length=length-64;

      if(length<0) length=-1;


      }




  for(i=0 ; i<64; i=i+4){
      W[i/4]=(a[i]<<24)|(a[i+1]<<16)|(a[i+2]<<8)|a[i+3];
    }


  for(i=16 ; i<64; i++){//                                                                            W(t-7)                                                                                  W(t-16)
           W[i]=(((W[i-2]>>17)|(W[i-2]<<(32-17))) ^ ((W[i-2]>>19)|(W[i-2]<<(32-19))) ^  W[i-2]>>10) + W[i-7] + (((W[i-15]>>7)|(W[i-15]<<(32-7))) ^ ((W[i-15]>>18)|(W[i-15]<<(32-18))) ^  (W[i-15]>>3)) + W[i-16];

           }




 for(i=0;i<8; i++)
  Val[i]=H[i];


  unsigned int T1,T2;



  for(i=0; i<64; i++){
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



for(i=0; i<8; i++){
        wynik[i]=H[i];
        //printf("%.8x \n",wynik[i]);
}

int_to_char(wynik, result,8);

}







int main(){


    int i;
    // HMAC

    unsigned char* password="aaaaaaaaaabbbbbbbbbbccccjjgdsswryoofcnnhbvzxasdfjklccccccddddddddddeeeeeeeeeeffffffffffgggggg";
    unsigned char* message="akjfdsgkjfgsjfgfjfj";
    unsigned int message_length=strlen(message);

    unsigned char keyK[64];
    unsigned int password_lenght=strlen(password);

    unsigned char ipad[64]="6666666666666666666666666666666666666666666666666666666666666666";
    unsigned char opad[64]="\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\";


    unsigned char blockA[64];
    unsigned char blockB[64];

    unsigned char conc[192];
    i=0;

    // padding klucza hmac
    if(password_lenght>64){
        sha256(password,password_lenght,keyK);
        i=32;
        password_lenght=32;

    }

        for(i; i<password_lenght; i++)
            keyK[i]=password[i];
        for(i=password_lenght;i<64;i++)
            keyK[i]=0;

/////////




for(i=0; i<64; i++){
    conc[i]=ipad[i]^keyK[i]; // xor ipadu
}



for(i=64; i<64+message_length; i++){  //konkatenacja wiadomosci z ipadem
    conc[i]=message[i-64];
}



sha256(conc,64+message_length,blockB); // sha, blockB rozmiar 32


for(i=0; i<64; i++){
    blockA[i]=opad[i]^keyK[i]; // xor opadu
}


for(i=0; i<64; i++){
    conc[i]=blockA[i];
}

for(i=64; i<96;i++){
    conc[i]=blockB[i-64];
}



sha256(conc,96,blockA);



for(i=0; i<32; i++){
    printf("%.2x",blockA[i]);
}

return 0;
}
