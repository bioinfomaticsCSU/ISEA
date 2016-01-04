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
	int k;//k-mer
	char type[10];
	char left[4096+512];
	char right[4096+512];
	int ReadLength;//读数长度
	int errolp; // MinOverLap for error correction
};
Lib lib[200];

int main(int argc,char *argv[]){
	//************** debug information ***************
	DIR *dir=NULL;
	dir = opendir("log");
	if(!dir){
		system("mkdir log");
	}

	ofile.open("log/preprocess.log");
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	
	if(argc != 1){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\t[install path]/preprocess"<<endl;
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
	if(!freopen("mapping/inputfiles.info","r",stdin)){
		perror("mapping/inputfiles.info");
	};
	int Threads,FileNum;
	scanf("%d",&Threads);
	scanf("%d",&lib[0].k);
	scanf("%d",&FileNum);
	int i;

	for(i=1;i<=FileNum;i++)
	{
		scanf("%s%s%s%d%d",lib[i].type,lib[i].left,lib[i].right,&lib[i].ReadLength, &lib[i].errolp);
		lib[i].k = lib[0].k;
	}
	
	char strComm[1000+( 4096 + 512)*FileNum];
	strComm[0]='\0';
	
	for(i=1;i<=FileNum;i++)
	{
		sprintf(strComm+strlen(strComm),"%s/prepare %s %s %s _isea.%d.fasta &\n",resolved_path,lib[i].left,lib[i].right,lib[i].type,i);
	}
	sprintf(strComm+strlen(strComm),"wait\n");
	
	for(i=1;i<=FileNum;i++)
	{
		sprintf(strComm+strlen(strComm),"%s/hash _isea.%d.fasta %d.out &\n",resolved_path,i,i);
	}
	sprintf(strComm+strlen(strComm),"wait\n");
	
	for(i=1;i<=FileNum;i++)
	{
		sprintf(strComm+strlen(strComm),"%s/filter %d.out %d_filter_pe.txt &\n",resolved_path,i,i);
	}
	sprintf(strComm+strlen(strComm),"wait\n");
	
	for(i=1;i<=FileNum;i++)
	{
		sprintf(strComm+strlen(strComm),"%s/sta %d_filter_pe.txt &\n",resolved_path,i);
	}
	sprintf(strComm+strlen(strComm),"wait\n");
	
	ofile<<strComm<<endl;
	system(strComm);
	
	return 0;
}

