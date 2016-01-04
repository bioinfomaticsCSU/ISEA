//
//** move reads that containing too many errors, which are considered can't be corrected.
//
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<set>
using namespace std;
char FileIn[100];
int MinOverLap;
int MaxOverLap;
int len;
char FileOut1[100];
//char FileOut2[100];
struct ErrC
{
	int arr[200];
	string reads;
};
set<string>ss;
int main(int argc,char* argv[])
{
	if(argc != 3){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\tfilter <infile> <outfile>"<<endl;
		return -1;
	}
	strcpy(FileIn,argv[1]);
	strcpy(FileOut1,argv[2]);
//	strcpy(FileOut2,argv[3]);
	if(!freopen(FileIn,"r",stdin)){
		perror(FileIn);
	};
	freopen(FileOut1,"w",stdout);
	char temp[200];
	len=0;
	int i,j;
	int score;
	int cntless;
	bool flag1,flag2;
	ErrC ec1;
	ErrC ec2;
	string outstr;
	int cnt=1;
	while(cin>>ec1.reads)
	{
		flag1=false;
		flag2=false;
		if(!len)
		{
			len=ec1.reads.size();
			MaxOverLap=len-1;
			MinOverLap=((len/2)+(len*2/3))/2;
		}
		cntless=0;
		for(i=0;i<len-MinOverLap+1;i++)
		{
			scanf("%d",&ec1.arr[i]);
			if(ec1.arr[i]<=3) cntless++;
		}
		if(cntless>(len-MinOverLap+1)*7/8) flag1=true;
	//	if(flag1==false) ss.insert(ec1.reads);


		cin>>ec2.reads;
		cntless=0;
		for(i=0;i<len-MinOverLap+1;i++)
		{
			scanf("%d",&ec2.arr[i]);
			if(ec2.arr[i]<=3) cntless++;
		}
		if(cntless>(len-MinOverLap+1)*7/8) flag2=true;
	//	if(flag2==false) ss.insert(ec2.reads);
		if(flag1==false||flag2==false)
		{
			printf(">%d\n",cnt++);
			printf("%s\t%s\n",ec1.reads.c_str(),ec2.reads.c_str());
		//	for(i=0;i<len-MinOverLap+1;i++) printf(" %d",ec1.arr[i]); printf("\n");
		//	for(i=0;i<len-MinOverLap+1;i++) printf(" %d",ec2.arr[i]); printf("\n");
		}
	}
	return 0;
}
