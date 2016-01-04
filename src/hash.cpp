#include<iostream>
#include<unordered_map>
#include<vector>
#include<stdio.h>
#include<string>
#include<string.h>
#include<algorithm>
#include<vector>
using namespace std;
unordered_map<string,int>ump;
char otherinfo[1000];
char temp1[1000],temp2[1000];
char FileIn[100];
char FileOut[100];
int MinOverLap;
int MaxOverLap;
int len;
int K;
vector<string>vec;
int main(int argc,char* argv[])
{
	if(argc != 3){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\thash <infile> <outfile>"<<endl;
		return -1;
	}
	
	string tt1,tt2;
	int i,j;
	string str;
	int times;
	string tempstr;
	strcpy(FileIn,argv[1]);
	strcpy(FileOut,argv[2]);
	if(!freopen(FileIn,"r",stdin)){
		perror(FileIn);
	};
	if(!freopen(FileOut,"w",stdout)){
		perror(FileOut);
	};
	len=0;
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
			MinOverLap=((len/2)+(len*2/3))/2;
			K=MinOverLap;
		}
		vec.push_back(tt1);
		vec.push_back(tt2);
		for(i=0;i<=tt1.size()-K;i++) ump[tt1.substr(i,K)]++;
		for(i=0;i<=tt2.size()-K;i++) ump[tt2.substr(i,K)]++;
	}
	for(i=0;i<vec.size();i++)
	{
		tempstr=vec[i];
		printf("%s",tempstr.c_str());
		for(j=0;j<=len-K;j++) printf(" %d",ump[tempstr.substr(j,K)]); printf("\n");
	}
	return 0;
}
