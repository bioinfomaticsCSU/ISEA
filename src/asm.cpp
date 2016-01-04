#include<cstdlib>
#include<cstring>
#include<stdio.h>
#include<iostream>
#include<unistd.h>
using namespace std;
//************ debug information **************
#include<fstream>
#include<dirent.h>
ofstream ofile;
time_t ntm;
time_t prtm;
struct tm* current_time;
//*********************************************

struct Lib
{
	char FilePe[18];
	char FileSe[18];
	int MinSpan;//min_insertsize
	int MaxSpan;//max_insertsize
	int aveSpan;//ave_insertsize
	int MinOverLap;//K值
	int ReadLength;//读数长度
	int std;//standard deviation
};
Lib lib[200];

int main(int argc,char *argv[]){
	//************** debug information ***************
	DIR *dir=NULL;
	dir = opendir("log");
	if(!dir){
		system("mkdir log");
	}

	ofile.open("log/corr.log");
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	
	if(argc != 1){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\t[install path]/asm"<<endl;
		return -1;
	}
	
	char resolved_path[80];
	realpath(argv[0],resolved_path);
	for(int i=strlen(resolved_path)-1;i--;i>=0){
			if(resolved_path[i]=='/'){
					resolved_path[i] = '\0';
					break;
			}
	}

	char strComm[1000];
	strComm[0]='\0';

	sprintf(strComm+strlen(strComm),"%s/extend && ",resolved_path);
	
	sprintf(strComm+strlen(strComm),"%s/combine && ",resolved_path);
	sprintf(strComm+strlen(strComm),"wait\n");
	
	sprintf(strComm+strlen(strComm),"%s/scaffold ",resolved_path);
	sprintf(strComm+strlen(strComm),"wait\n");
	
	system(strComm);
	//cout<<strComm;
	return 0;
}

