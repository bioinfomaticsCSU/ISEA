#include<iostream>
#include<unordered_map>
#include<vector>
#include<stdio.h>
#include<string>
#include<string.h>
#include<algorithm>
using namespace std;
unordered_map<string,int>ump;
char otherinfo[1000];
char temp1[1000],temp2[1000];
char FileIn[100];
char FileOut[100];
int MinOverLap;
int MaxOverLap;
int len;
vector<string>vec;
int vecsize;
int main(int argc,char* argv[])
{
	if(argc != 2){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\tsta <infile>"<<endl;
		return -1;
	}
	string tt1,tt2;
	int i,j,k;
	int K;
	string str;

	strcpy(FileIn,argv[1]);
	if(!freopen(FileIn,"r",stdin)){
		perror(FileIn);
	};
	len=0;
	string tempstr;

	int idnum;
	for(i=0;i<strlen(FileIn);i++)
	{
		if(FileIn[i]>='0'&&FileIn[i]<='9')
		{
			idnum=FileIn[i]-'0';
			break;
		}
	}
	while(gets(otherinfo))
	{
		if(otherinfo[0]!='>') continue;
		scanf("%s%s",temp1,temp2);
		tt1=(string)temp1;
		tt2=(string)temp2;
		if(!len)
		{
			len=tt1.size();
			MaxOverLap=len-1;
			MinOverLap=((len/2)+(len*2/3))/2+1;
			K=MinOverLap;
		}
		vec.push_back(tt1);
		vec.push_back(tt2);
	}
	vecsize=vec.size();
	for(int i=0;i<vecsize;i++)
	{
		tempstr=vec[i];
		for(int j=0;j<=len-K;j++)
		{
			ump[tempstr.substr(j,K)]++;
		}
	}
	
	sprintf(FileOut,"sta_%d.txt",idnum);
	freopen(FileOut,"w",stdout);
	for(j=0;j<vecsize;j++)
	{
		tempstr=vec[j];
		printf("%s",tempstr.c_str());
		for(k=0;k<=len-K;k++) printf(" %d",ump[tempstr.substr(k,K)]); printf("\n");
	} 
	return 0;
}
