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
	int errolp;
};
Lib lib[200];

int main(int argc,char * argv[]){
	//************** debug information ***************
	DIR *dir=NULL;
	dir = opendir("log");
	if(!dir){
		system("mkdir log");
	}

	bool bfast = false;
	
	if(argc == 2)bfast = 1;
	
	ofile.open("log/corr.log");
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	
	char resolved_path[80];
	realpath(argv[0],resolved_path);
	for(int i=strlen(resolved_path)-1;i--;i>=0){
			if(resolved_path[i]=='/'){
					resolved_path[i] = '\0';
					break;
			}
	}
	if(!freopen("lib.info","r",stdin)){
		perror("lib.info");
		exit(0);
	};
	int FileNum;
	scanf("%d",&FileNum);
	scanf("%d",&lib[0].MinOverLap);
	int i;
	char *ptr;

	for(i=1;i<=FileNum;i++)
	{
		scanf("%s%d%d%d%d%d",lib[i].FilePe,&lib[i].MinSpan,&lib[i].MaxSpan,&lib[i].std,&lib[i].ReadLength, &lib[i].errolp);
		lib[i].MinOverLap = lib[0].MinOverLap;
		lib[i].aveSpan=(lib[i].MinSpan+lib[i].MaxSpan)/2;
	}
	
	char strComm[1000];
	strComm[0]='\0';
	
	for(i=1;i<=FileNum;i++)
	{
		if(lib[i].errolp == 0){
			sprintf(strComm+strlen(strComm),"perl %s/ecsim.pl %d_error_correction.txt &\n",resolved_path,i);
		}else if(lib[i].errolp > 0){
			sprintf(strComm+strlen(strComm),"%s/errCor %d_error_correction.txt %d &\n",resolved_path,i,lib[i].errolp);
			if(!bfast)sprintf(strComm+strlen(strComm),"wait\n");
		}else{
			sprintf(strComm+strlen(strComm),"%s/errCor %d_error_correction.txt  &\n",resolved_path,i);
			if(!bfast)sprintf(strComm+strlen(strComm),"wait\n");
		}
	}
	
	sprintf(strComm+strlen(strComm),"wait\n");
	for(i=1;i<=FileNum;i++)
	{
		sprintf(strComm+strlen(strComm),"%s/postprocessor %d_error_correction.txt se_%d.txt pe_%d.txt &\n",resolved_path,i,i,i);
	}
	sprintf(strComm+strlen(strComm),"wait\n");
	ofile<<strComm<<endl;
	system(strComm);
	
	return 0;
}

