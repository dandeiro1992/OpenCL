#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <bitset>
#define SZYFROWANIE true
#define DESZYFROWANIE false

using namespace std;
std::map <string,int> mapa;
int NWD(int a, int b)
{
    while(a!=b)
       if(a>b)
           a-=b;
       else
           b-=a;
    return a;
}
int odwrotnosc_skladnika(int skladnik)
{
    return (26-skladnik)%26;
}
int odwrotnosc (int liczba)
{
    if(NWD(liczba,26)!=1)
    {
        cout<<"nie da sie odszyfrowac-ten klucz jest nieodpowiedni\n";
    }
    else
    {
        int wynik=liczba;
        for(int i=1;i<11;i++)
        {
            wynik=(wynik*liczba)%26;
        }
        return wynik;
    }

}
int ile_bajtow(string myString)
{
  for (std::size_t i = 0; i < myString.size(); ++i)
  {
      cout << bitset<8>(myString.c_str()[i]) << endl;
  }
}
int ile_znakow(string k)
{
    return k.size();
}
int ile_zajmuje(string k1)
{
    char k=k1.c_str()[0];
    cout<<(int)k<<"\n";
    if((int)(k&0x80)==0)
        return 1;
    else if((int)(k&0x70)==0)
        return 1;
    else if((int)(k&0x70)==64)
        return 2;
    else if((int)(k&0x70)==96)
        return 3;
    else if((int)(k&0x70)==112)
        return 4;
    else
        return 0;

}
void key_find(string tablica[][2],int &a,int &b)
{
    for(a=0;a<26;a++)
    {
        for (b=0;b<26;b++)
        {
            if((mapa.find(tablica[0][0])->second)==(((a*(mapa.find(tablica[0][1])->second))+b)%32))
            {
                break;
            }
        }
        if((mapa.find(tablica[1][0])->second)==(((a*(mapa.find(tablica[1][1])->second))+b)%26))
        {
            break;
        }
    }
    //cout<<"a:"<<a;
    //cout<<"b:"<<b;
}
string affine(string tekst,int czynnik,int skladnik,bool szyfrowanie)
{
    if(szyfrowanie)
    {
        string k=tekst;

        for(int i=0;i<tekst.length();i++)
        {
            int tmp=((czynnik*(mapa.find(tekst.substr(i,1))->second)+skladnik)%26);
            for(std::map<string,int>::iterator it=mapa.begin();it!=mapa.end();++it)
            {
                if(it->second==tmp)
                {
                    k.replace(i,1,it->first);
                    break;
                }
            }
        }
        return k;
    }
    else
    {
        string jawny=tekst;
        int czynnik2=odwrotnosc(czynnik);
        int skladnik2=odwrotnosc_skladnika(skladnik);
        for(int i=0;i<tekst.length();i++)
        {
            int tmp=((((mapa.find(tekst.substr(i,1))->second))+skladnik2)*czynnik2)%26;
            for(std::map<string,int>::iterator it=mapa.begin();it!=mapa.end();++it)
            {
                if(it->second==tmp)
                {
                    jawny.replace(i,1,it->first);
                    break;
                }
            }
        }
        return jawny;
    }

}
int main()
{
    int a,b;
    int i=0;
    ifstream alfabet;
    alfabet.open("alfabet.txt",ios_base::in);
    string s;
    getline(alfabet,s);
    cout<<s;
    while(alfabet.good())
    {
        mapa.insert(std::pair <string,int>(s,i));
        getline(alfabet,s);
        cout<<s;
        i++;
    }
    alfabet.close();

    for (std::map<string,int>::iterator it=mapa.begin(); it!=mapa.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

    cout<<odwrotnosc(5)<<endl;;
    //cout<<affine(string_szyfr,a,b,DESZYFROWANIE);
    cout<<"\n";
    string tresc="alamakota";
    cout<<affine(tresc,1,1,SZYFROWANIE);
    cout<<"\n";
    cout<<affine("bmbnblpub",1,1,DESZYFROWANIE);
    /*ofstream plik;
    plik.open("jawny.txt",ios_base::out);
    plik<<affine(string_szyfr,a,b,DESZYFROWANIE);
    plik.close();*/
    return 0;
}
