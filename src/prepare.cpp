#include<iostream>
#include<fstream>
#include<stdio.h>
#include<vector>
#include<unordered_map>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<fstream>
using namespace std;
//************ debug information **************
#include<fstream>
#include<dirent.h>
ofstream ofile;
time_t ntm;
time_t prtm;
struct tm* current_time;
//*********************************************

char FileOut[100];
char otherinfo[1000];
char temp1[1000],temp2[1000];
char symbol[]={'A','C','G','T'};
struct Pair 
{
	string Leftstr;
	string Rightstr;
};
vector<Pair>v;
unordered_map<string,bool>ump;
bool IsFitCondition(string & str)
{
	int i;
	int NumOfTelorant;
	int cnt=0;
	NumOfTelorant=str.size()*0.1;
	for(i=0;i<str.size();i++)
	{
		if(str[i]=='N'||str[i]=='n') cnt++;
		if(cnt>=NumOfTelorant) return false;
	}
	if(cnt>0)
	{
		for(i=0;i<str.size();i++)
		{
			if(str[i]=='N'||str[i]=='n') str[i]=symbol[rand()%4];
		}
	}
	return true;
}

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
		else ans[len-i-1]='N';
	}
	return ans;
}

int getNewLength(int oldlen){
	int newlen = oldlen;
	if(newlen < 66)return newlen;
	int found = 0;
	double cutoff = 0.01001;
	while(1){
		for(int j = 33; j < 50; j++){
			if( (double)(newlen % j)/newlen < cutoff ){
				found = 1;
				return j;
				break;
			};
		}
		if(!found)cutoff++;
	}
}

int main(int argc,char* argv[])
{
	//************** debug information ***************
	DIR *dir=NULL;
	dir = opendir("log");
	if(!dir){
		system("mkdir log");
	}

	int idnum;
	for(int i=0;i<strlen(argv[4]);i++)
	{
		if(argv[4][i]>='0'&&argv[4][i]<='9')
		{
			idnum=argv[4][i]-'0';
			break;
		}
	}
	
	char logfile[100];
	sprintf(logfile,"log/prepare.%d.log",idnum);
	ofile.open(logfile);
	
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	
	if(argc != 5){
		cerr<<"bad parameters"<<endl;
		cerr<<"Usage:\n\tprepare <left file> <right file> <pr or mp> <outfile>"<<endl;
		return -1;
	}
	
	srand(unsigned(time(NULL)));
	int i,j;
	int cnt=1;
	Pair pr;
	ifstream if1(argv[1]);
	ifstream if2(argv[2]);

	bool paired_end = 1;
	if(argv[3][0] == 'm')paired_end = 0;
	
	int cntorig = 0;
	int cnttmp = 0;
	int readlen;
	int newlen;
	
	while(if1.getline(otherinfo,1000))
	{
		cntorig ++;
		if2.getline(otherinfo,1000);
		if1.getline(temp1,1000);
		if2.getline(temp2,1000);
		pr.Leftstr=(string)temp1;
		pr.Rightstr=(string)temp2;
		
		for(j=0;j<pr.Leftstr.size();j++) pr.Leftstr[j]=toupper(pr.Leftstr[j]);
		for(j=0;j<pr.Rightstr.size();j++) pr.Rightstr[j]=toupper(pr.Rightstr[j]);
		
		pr.Leftstr = Reverse_Compliment(pr.Leftstr);
		
		if(paired_end)
			swap(pr.Leftstr, pr.Rightstr);
		
		Pair pr2 = pr;
		readlen = pr2.Leftstr.size();
		
		newlen = getNewLength(readlen);
		
		for(int i = 0;i < readlen;){
			pr.Leftstr = pr2.Leftstr.substr(i,newlen);
			pr.Rightstr = pr2.Rightstr.substr(i,newlen);
			i += newlen ;
			cnttmp++;
			if(ump[pr.Leftstr+pr.Rightstr]==true) {
				continue;
			}
			ump[pr.Leftstr+pr.Rightstr]=true;
			if(IsFitCondition(pr.Leftstr)==true&&IsFitCondition(pr.Rightstr)==true)	v.push_back(pr);
		}
	}
	ofile<<"orig read length = "<<readlen<<endl;
	ofile<<"new read length = "<<newlen<<endl;
	ofile<<cntorig<<" pairs in orig fasta files "<<argv[1]<<", "<<argv[2]<<endl;
	ofile<<cnttmp<<" pairs after sliced"<<endl;
	ofstream of1(argv[4]);
	
	for(i=0;i<v.size();i++)
	{
		of1<<">"<<cnt++<<"\n";
		pr=v[i];
		
		of1<<pr.Leftstr<<'\t'<<pr.Rightstr<<endl;
	}
	if1.close();
	if2.close();
	of1.close();
	
	ofile<<v.size()<<" unique pairs after sliced"<<endl;
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program end\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	return 0;
}
