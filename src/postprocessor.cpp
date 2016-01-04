#include<iostream>
#include<vector>
#include<unordered_set>
#include<unordered_map>
#include<string>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<algorithm>
using namespace std;
char FileIn[100];
char FileOut1[100];
char FileOut2[100];
char otherinfo[1000];
char temp1[1000],temp2[1000];
struct Pair_End
{
	string LeftStr;
	string RightStr;
};
vector<Pair_End>vec;
vector<Pair_End>PR;//最终pair-end
unordered_set<string>temps;
unordered_set<string>SR;//最终single-end
unordered_map<string,int>times;
unordered_map<string,bool>mp;
string Reverse_Compliment(string ori)
{
	//对原字符串进行反向互补
	string ans=ori;
	int len=ori.size();
	for(int i=len-1;i>=0;i--)
	{
		if(ori[i]=='A') ans[len-i-1]='T';
		else if(ori[i]=='C') ans[len-i-1]='G';
		else if(ori[i]=='G') ans[len-i-1]='C';
		else if(ori[i]=='T') ans[len-i-1]='A';
	}
	return ans;
}
int main(int argc,char* argv[])
{
	strcpy(FileIn,argv[1]);
	sprintf(FileOut1,"%s",argv[2]);
	sprintf(FileOut2,"%s",argv[3]);
	
	if(!freopen(FileIn,"r",stdin)){
		perror(FileIn);
		exit(-1);
	};
	char temp[100];
	Pair_End pe;
	int len=0;
	int MinOverLap;
	int i,j;
	string tempstr;
	string tempstr1;
	string tempstr2;
	string revstr1;
	string revstr2;
	bool flag1,flag2;
	while(gets(otherinfo))
	{
		if(otherinfo[0]!='>') continue;
		scanf("%s%s",temp1,temp2);
		pe.LeftStr=(string)temp1;
		pe.RightStr=(string)temp2;
		vec.push_back(pe);
		temps.insert(pe.LeftStr);
		temps.insert(pe.RightStr);
		if(!len)
		{
			len=strlen(temp1);
			MinOverLap=((len/2)+(len*2/3))/2+1;
		}
	}
	unordered_set<string>::iterator iter=temps.begin();
	while(iter!=temps.end())
	{
		string SRead=*iter;
		for(int i=0;i<=len-MinOverLap;i++)
		{
			tempstr=SRead.substr(i,MinOverLap);
			times[tempstr]++;
		}
		iter++;
	}
	for(i=0;i<vec.size();i++)
	{
		pe=vec[i];
		flag1=0; flag2=0;
		for(j=0;j<=len-MinOverLap;j++)
		{
			tempstr1=pe.LeftStr.substr(j,MinOverLap);
			if(times[tempstr1]==1)
			{
				flag1=1; break;
			}
		}
		if(flag1==0)
		{
			SR.insert(pe.LeftStr);
			revstr1=Reverse_Compliment(pe.LeftStr);
			SR.insert(revstr1);
		}
		for(j=0;j<=len-MinOverLap;j++)
		{
			tempstr2=pe.RightStr.substr(j,MinOverLap);
			if(times[tempstr2]==1)
			{
				flag2=1; break;
			}
		}
		if(flag2==0)
		{
			SR.insert(pe.RightStr);
			revstr2=Reverse_Compliment(pe.RightStr);
			SR.insert(revstr2);
		}
		if(flag1==0&&flag2==0)
		{
			if(mp[pe.LeftStr+pe.RightStr]==false) PR.push_back(pe);
			mp[pe.LeftStr+pe.RightStr]=true;

			revstr1=Reverse_Compliment(pe.RightStr);
			revstr2=Reverse_Compliment(pe.LeftStr);
			pe.LeftStr=revstr1;
			pe.RightStr=revstr2;
			if(mp[pe.LeftStr+pe.RightStr]==false) PR.push_back(pe);
			mp[pe.LeftStr+pe.RightStr]=true;
		}
	}
	freopen(FileOut1,"w",stdout);
	unordered_set<string>::iterator it=SR.begin();
	while(it!=SR.end())
	{
		tempstr=*it;
		printf("%s\n",tempstr.c_str());
		it++;
	}
	freopen(FileOut2,"w",stdout);
	for(i=0;i<PR.size();i++)
	{
		printf(">%d\n",i+1);
		pe=PR[i];
		printf("%s\t%s\n",pe.LeftStr.c_str(),pe.RightStr.c_str());
	}
	return 0;
}
