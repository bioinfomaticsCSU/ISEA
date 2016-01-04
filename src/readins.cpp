#include<iostream>
#include<stdio.h>
#include<string>
#include<string.h>
#include<unordered_map>
#include<set>
#include<algorithm>
#include<vector>
#include<stack>
#include<cmath>
using namespace std;

#include<fstream>
#include<dirent.h>
ofstream ofile;
time_t ntm;
time_t prtm;
struct tm* current_time;

const int MaxFileNum=10;
const int MaxReadLength=220;
const double NegativeMin=-1000000;

int MinExtensionLength;//k-mer扩展被接受的最小长度

vector<vector<int> > vout;
vector<vector<int> > vin;

int FileNum;//文库的数目
int cnt=0;//k-mer的数目
double PlusScore=0.8;

char cha[]={'A','C','G','T'};

struct MateFinder 
{
	vector<string>v;
};
unordered_map<string,MateFinder>LtoR[MaxFileNum];//通过左边的mate查找右边的mate集合
unordered_map<string,MateFinder>RtoL[MaxFileNum];//通过右边的mate查找左边的mate集合
unordered_map<string,int>StrtoInt;//从字符串获得index
unordered_map<string,int>times;
vector<string> InttoStr;//从index获得字符串

int NumOfContig=0;//合并后种子序列的数目
vector<string> contigs;//保存长度从大到小排序后的种子

struct Lib
{
	char FilePe[18];
	char FileSe[18];
	int MinSpan;//min_insertsize
	int MaxSpan;//max_insertsize
	int aveSpan;//ave_insertsize
	int MinOverLap;//K值
	int ReadLength;//读数长度
	int errolp;
	int std;//standard deviation
};

Lib lib[MaxFileNum];//存储文库信息
char line[MaxReadLength];//把输入文件按行读取然后分割
vector<int> isused;//记录该节点是否使用过

struct Node 
{
	string CurStr;
	int index;
	double score;
	string SupportStr;
	set<int> collection;
};

stack<Node>st;//保存深度优先搜索的节点
vector<Node>tempvec;//保存临时的可能节点

bool Delete_Element(int i)
{
	return (isused[i]==1);
}

int IsExistInVector(int val,vector<int>vec)
{
	int i;
	for(i=0;i<vec.size();i++)
	{
		if(val==vec[i]) return i;
	}
	return -1;
}

void Graph_Buid()
{
	string tempstr;//存储读入的单端字符串
	string subtempstr;//存储读入的单端字符串的子串
	string nextsubstr;//下一个连接的k-mer
	char SRead[MaxReadLength];//读入的单端的读数
	unordered_map<string,int>::iterator iter1;//判断当前节点是否出现过
	unordered_map<string,int>::iterator iter2;//判断当前节点的下一个节点是否出现过
	int index1;
	int index2;
	int i,j;
	int FileId;
	for(FileId=1;FileId<=FileNum;FileId++)
	{
		if(!freopen(lib[FileId].FileSe,"r",stdin)){
			perror(lib[FileId].FileSe);
		};//读入第一个文库的单读数文件
		ofile<<"construct de bruign graph from "<<lib[FileId].FileSe<<endl;
		while(scanf("%s",SRead)!=EOF)
		{
			tempstr=(string)SRead;
			lib[FileId].ReadLength = tempstr.size();
			for(i=0;i<lib[FileId].ReadLength-lib[1].MinOverLap;i++)
			{
				subtempstr=tempstr.substr(i,lib[1].MinOverLap);
				nextsubstr=tempstr.substr(i+1,lib[1].MinOverLap);
				iter1=StrtoInt.find(subtempstr);
				if(iter1==StrtoInt.end())
				{
					StrtoInt[subtempstr]=cnt;
					InttoStr.push_back(subtempstr);
					index1=cnt;
					vin.push_back(vector<int>());
					vout.push_back(vector<int>());
					cnt++;
				}
				else index1=StrtoInt[subtempstr];

				iter2=StrtoInt.find(nextsubstr);
				if(iter2==StrtoInt.end())
				{
					StrtoInt[nextsubstr]=cnt;
					InttoStr.push_back(nextsubstr);
					index2=cnt;
					vin.push_back(vector<int>());
					vout.push_back(vector<int>());
					cnt++;
				}
				else index2=StrtoInt[nextsubstr];
				if(IsExistInVector(index1,vin[index2])==-1) vin[index2].push_back(index1);
				if(IsExistInVector(index2,vout[index1])==-1) vout[index1].push_back(index2);
			}
		}
	}
	isused.resize(InttoStr.size());
}

void ReadInputFile()
{
	if(!freopen("lib.pre.info","r",stdin)){
		perror("lib.pre.info");
	};//读入配置文件
	scanf("%d",&FileNum);
	scanf("%d",&lib[0].MinOverLap);
	int i;
	char *ptr;
	char ls[MaxReadLength],rs[MaxReadLength];//用C读入的字符串
	
	for(i=1;i<=FileNum;i++)
	{
		scanf("%s%d%d%d%d%d",lib[i].FilePe,&lib[i].MinSpan,&lib[i].MaxSpan,&lib[i].std,&lib[i].ReadLength, &lib[i].errolp);
		lib[i].MinOverLap = lib[0].MinOverLap;
		lib[i].aveSpan=(lib[i].MinSpan+lib[i].MaxSpan)/2;
		sprintf(lib[i].FilePe,"pe_readins_%d.txt",i);
		sprintf(lib[i].FileSe,"se_readins_%d.txt",i);
	}
}

void Simplify_Graph()//对图进行简化包括一度节点的合并和tips的移除
{
	int i,j;
	int tindex;
	string tempstr;//存储合并后的字符串
	string onestr;//入度和出度都为1的字符串
	for(i=0;i<cnt;i++)
	{
		if(isused[i]==true) continue;
		tempstr=InttoStr[i];
		if(vout[i].size()==1)
		{
			tindex=vout[i][0];
			while(vin[tindex].size()==1&&vout[tindex].size()==1)//一直往前合并，直到出度不为1的节点
			{
				if(isused[tindex]==true) break;//防止出现环
				isused[tindex]=true;
				onestr=InttoStr[tindex];
				tempstr+=onestr.substr(lib[1].MinOverLap-1);
				tindex=vout[tindex][0];
			}
			if(vin[tindex].size()==1)//入度为1
			{
				if(isused[tindex]==false)
				{
					isused[tindex]=true;
					onestr=InttoStr[tindex];
					tempstr+=onestr.substr(lib[1].MinOverLap-1);
					vout[i].clear();
					for(j=0;j<vout[tindex].size();j++)
					{
						int TempOutNode=vout[tindex][j];
						vout[i].push_back(TempOutNode);//把最后一个节点的出度关系赋给第一个点
						vin[TempOutNode].push_back(i);//把最后一个节点的入度关系赋给第一个点
					} 
				}
			}
			else//入度不为1
			{
				vout[i].clear();
				vout[i].push_back(tindex);
				if(IsExistInVector(i,vin[tindex])==-1) vin[tindex].push_back(i);
			}
			StrtoInt[tempstr]=i;
			InttoStr[i]=tempstr;
		}
	//	cout<<"i="<<i<<endl;
	//	cout<<vin[i].size()<<" "<<vout[i].size()<<endl;
	//	cout<<tempstr.size()<<" "<<2*lib[1].MinOverLap<<endl;
		if(vin[i].size()==0&&tempstr.size()<2*lib[1].MinOverLap) isused[i]=true;
		if(vout[i].size()==0&&tempstr.size()<2*lib[1].MinOverLap) isused[i]=true;
		if((vout[i].size() - vin[i].size())==1){
			vector<int>::iterator it1,it2;
			bool bin(0),bout(0);
			for(it1 = vout[i].begin();it1!=vout[i].end();it1++)
			{
				if(*it1 == i){
					bout = 1;
					break;
				}
			}
			for(it2 = vin[i].begin();it2!=vin[i].end();it2++)
			{
				if(*it2 == i){
					bin = 1;
					break;
				}
			}
			if(bin&&bout){
				vout[i].erase(it1);
				vin[i].erase(it2);
			}
		}
	}
}

void DeleteUsedNode()
{
	int i;
	vector<int>::iterator iter;
	for(i=0;i<cnt;i++)
	{
		if(isused[i]==true) continue;
		iter=remove_if(vin[i].begin(),vin[i].end(),Delete_Element);
		vin[i].erase(iter,vin[i].end());
		iter=remove_if(vout[i].begin(),vout[i].end(),Delete_Element);
		vout[i].erase(iter,vout[i].end());
	}
}

bool cmp1(string s1,string s2)//对种子序列的长度从大到小排序
{
	return s1.size()>s2.size();
}

bool cmp2(Node n1,Node n2)
{
	return n1.score>n2.score;
}

void Seed_Selection(int cutoff)
{
	int i;
	string tempstr;
	for(i=0;i<cnt;i++)
	{
		if(isused[i]==true) continue;
		tempstr=InttoStr[i];
		if(tempstr.size()<cutoff) continue;
		contigs.push_back(tempstr);
		NumOfContig++;
	}
	sort(contigs.begin(),contigs.end(),cmp1);
}

int GetMin(int a,int b)
{
	return a<b?a:b;
}

bool IsNodeOk(string str)
{
	int i;
	int len=str.size();
	int scnt[4];
	memset(scnt,0,sizeof(scnt));
	for(i=0;i<len;i++)
	{
		if(str[i]=='A') scnt[0]++;
		else if(str[i]=='C') scnt[1]++;
		else if(str[i]=='G') scnt[2]++;
		else if(str[i]=='T') scnt[3]++;
	}
	for(i=0;i<4;i++) if(scnt[i]>len*0.9) return false;
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
	}
	return ans;
}


int main(int argc,char *argv[])
{
	DIR *dir=NULL;
	dir = opendir("mapping");
	if(!dir){
		system("mkdir mapping");
	}
	
	ofile.open("mapping/readins.log");
	
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	
	char resolved_path[80];
	realpath(argv[0],resolved_path);
	for(int i=strlen(resolved_path)-1;i--;i>=0){
			if(resolved_path[i]=='/'){
					resolved_path[i] = '\0';
					break;
			}
	}
	
	ReadInputFile();
	
	char commandPost[(4096+512)*FileNum];
	commandPost[0] = '\0';
	for(int i=1;i<=FileNum;i++)
	{
		sprintf(commandPost+strlen(commandPost),"%s/postprocessor %d_filter_pe.txt %s %s &\n",resolved_path,i,lib[i].FileSe, lib[i].FilePe);
	}
	sprintf(commandPost+strlen(commandPost),"wait\n");
	ofile<<commandPost<<endl;
	system(commandPost);
	
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"read over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	
	Graph_Buid();
	
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"Graph_Buid over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	
	int lasttt=0;
	while(1)
	{
		Simplify_Graph();
		DeleteUsedNode();
		int tt=0;
		for(int j=0;j<cnt;j++) if(isused[j]==true) tt++;
		if(tt!=lasttt) 
		{
			lasttt=tt;
		}
		else 
		{
			break;
		}
	}
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"Simplify_Graph over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	
	Seed_Selection(lib[1].MinSpan);

	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"Seed_Selection over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	
	freopen("mapping/rawpath.txt","w",stdout);
	for(int i=0;i<cnt;i++)
	{
		if(isused[i]==true) continue;
		string seedstring=InttoStr[i];
		printf("%s\n",seedstring.c_str());
	}
	
	fflush(stdout);
	
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"generate insread over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	
	char leftfasta[4096], righfasta[4096], type[10];
	if(!freopen("mapping/inputfiles.info","r",stdin)){
		perror("open mapping/inputfiles.info failed.\n");
		exit(0);
	};
	int thread, kmer, readlen, errOverlap;
	cin>>thread>>kmer>>FileNum;
	char command[(4096+512)*FileNum];
	sprintf(command,"perl %s/exactlongpath.pl mapping/rawpath.txt> mapping/longpath.fa&\nwait\npython %s/bowtie2-2.2.1/bowtie2-build mapping/longpath.fa index > mapping/bowtie2-build.log 2>&1 &\nwait\n",resolved_path,resolved_path);
	
	for(int i=0; i<FileNum; i++){
		cin>>type>>leftfasta>>righfasta>>readlen>>errOverlap;
		bool mp;
		mp = type[0]=='m'?1:0;
		sprintf(command+strlen(command),"perl %s/bowtie2-2.2.1/bowtie2 -f --no-head --threads %d -I 0 -X %d --%s -x index ",resolved_path, thread, mp?40000:10000, mp?"rf":"fr");
		sprintf(command+strlen(command),"-1 %s -2 %s -S mapping/mappingresults%d.sam > mapping/summary%d 2>&1 & \nwait\n", leftfasta, righfasta,i+1,i+1);
		sprintf(command+strlen(command),"perl %s/countins.pl mapping/mappingresults%d.sam 0 40000 %s > mapping/ins%d.info 2>mapping/err%d.info &\nwait\n",resolved_path,i+1,mp?"-mp":"",i+1,i+1);
		sprintf(command+strlen(command),"perl %s/calinsertsize.pl mapping/ins%d.info > mapping/hist%d 2>&1 &\nwait\n",resolved_path,i+1,i+1);
	}
	
	sprintf(command+strlen(command),"perl %s/getlib.pl ", resolved_path);
	
	for(int i=0; i<FileNum; i++){
		sprintf(command+strlen(command),"mapping/hist%d ",i+1);
	}
	sprintf(command+strlen(command),"> lib.info&\nwait\n");
	
	ofile<<"==========================\ncommand:\n"<<command<<endl;
	system(command);
	
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program end\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	//************************************************
	return 0;
}
