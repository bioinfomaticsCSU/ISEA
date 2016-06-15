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
		while(scanf("%s",SRead)!=EOF)
		{
			tempstr=(string)SRead;
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
	if(!freopen("lib.info","r",stdin)){
		perror("lib.info");
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
		sprintf(lib[i].FilePe,"pe_%d.txt",i);
		sprintf(lib[i].FileSe,"se_%d.txt",i);
	}
	
	for(i=1;i<=FileNum;i++)
	{
		freopen(lib[i].FilePe,"r",stdin);//读入所有文库，并保存配对关系
		while(gets(line))
		{
			if(line[0]=='>') continue;
			ptr=strtok(line, "\t"); strcpy(ls, ptr);
			ptr=strtok(NULL, "\t"); strcpy(rs, ptr);
			MateFinder& mf=LtoR[i][(string)ls];
			mf.v.push_back((string)rs);

			MateFinder& mf2=RtoL[i][(string)rs];
			mf2.v.push_back((string)ls);
		}
	}
	MinExtensionLength=(int)(lib[1].ReadLength*1.5);
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

void setExtLen(){
	int dgree=0;
	int totalLen=0;
	for(int i=0;i<cnt;i++){
		if(isused[i])continue;
		for(int j=0;j<vout[i].size();j++){
			if(!isused[vout[i][j]]){
				dgree++;
			}
		}
		totalLen += (InttoStr[i].size()-lib[1].MinOverLap);
	}
	totalLen/=2;
	double ruler = (double)dgree/totalLen;
	ofile<<"totalLen = "<<totalLen<<", dgree = "<<dgree<<", dgree/totalLen = "<<ruler<<endl;
	if(ruler<0.025)MinExtensionLength=(int)(lib[1].ReadLength*1.5);
	else MinExtensionLength=(int)(lib[1].ReadLength*1.24);
	ofile<<"DegreeRate = "<<ruler<<endl;
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

bool IsExistInAssembledString(int Csize,string CutReadOfOtherMate,string AssembledString,int index,int LoopI,int & insertsize,int & matchreads,int dir)
{
	int pos=0;
	string SubAssembledString;
	if(dir==1)
	{
		if(AssembledString.size()>lib[index].MaxSpan)
			SubAssembledString=AssembledString.substr(AssembledString.size()-lib[index].MaxSpan);
		else SubAssembledString=AssembledString;
	}
	else
	{
		if(AssembledString.size()>lib[index].MaxSpan)
			SubAssembledString=AssembledString.substr(0,lib[index].MaxSpan);
		else SubAssembledString=AssembledString;
	}
	pos=SubAssembledString.find(CutReadOfOtherMate,pos);
	if(pos==-1) 
	{
		return false;
	}
	if(dir==1) insertsize=SubAssembledString.size()-pos+(lib[index].ReadLength-lib[index].MinOverLap+LoopI);
	else insertsize=Csize-LoopI+pos+lib[index].ReadLength-lib[index].MinOverLap;
//	cout<<insertsize<<endl;
	if(insertsize>=lib[index].MinSpan&&insertsize<=lib[index].MaxSpan) 
	{
		matchreads++;
		return true;
	}
	return false;
}

bool isok(string & SupportStr,int Csize,string AssembledString,string CutRead,int index,int LoopI,int & insertsize,int & flag,int & totalreads,int & matchreads,int dir)
{
	int i;
	MateFinder mf;
	if(dir==1) mf=RtoL[index][CutRead];
	else mf=LtoR[index][CutRead];
	if(mf.v.size()==0) 
	{
		flag=0;//表示没有mate
		return false;
	}
	totalreads++;
	for(i=0;i<mf.v.size();i++)
	{
	//	cout<<CutRead<<" "<<mf.v[i]<<endl;
		if(true==IsExistInAssembledString(Csize,mf.v[i],AssembledString,index,LoopI,insertsize,matchreads,dir))
		{
			SupportStr+=CutRead;
			flag=1;
			return true;
		}
	}
	flag=2;//表示有mate却没有在已经拼好的序列中存在
	return false;
}

double GetScore(string & SupportStr,string AssembledString,string CandidateString,int index,int & totalreads,int & matchreads,int dir)
{
	int i;
	int insertsize=0;
	double score=0;
	int sign;//设置一个临时标记，为赋值给 flag 做准备
	SupportStr="";
	for(i=0;i<=(int)CandidateString.size()-lib[index].ReadLength;i++)
	{
		if(true==isok(SupportStr,CandidateString.size(),AssembledString,CandidateString.substr(i,lib[index].ReadLength),index,i,insertsize,sign,totalreads,matchreads,dir))
		{
			double sd = lib[index].std;
			if(lib[index].MaxSpan<500)score+=exp((abs(insertsize-lib[index].aveSpan))/(sd*3.14)*(-1));
			else score+=(2*exp((abs(insertsize-lib[index].aveSpan))/(sd*3.14)*(-1)));
		}
		else
		{
			if(sign==2)
			{
				//if(lib[index].MaxSpan<500) score-=0.01;
			}
		}
	}
	return score;
}

int GetFirstDifPos(int CurSize,vector<Node>vec,int MinCandidateLength,int dir)
{
	int temppos;//记录差值在阈值范围内的最后一个
	int i,j;
	int cntzero=0;
	int cntpositive=0;
	temppos=vec.size();
	for(i=0;i<tempvec.size();i++)
	{
		if(tempvec[i].score==0) cntzero++;
		else cntpositive++;
	}
	for(i=0;i<tempvec.size();i++)
	{
		if(CurSize>(lib[FileNum].MinSpan+lib[FileNum].MaxSpan)/2)
		{
			if(cntpositive>=cntzero)
			{
				if(vec[0].score>3*vec[i].score)
				{
					temppos=i;
					break;
				}
			}
			else
			{
				if(vec[i].score<0)
				{
					temppos=i;
					break;
				}
			}
		}
		else
		{
			if(cntpositive>=cntzero)
			{
				if(vec[0].score>10*vec[i].score)
				{
					temppos=i;
					break;
				}
			}
			else
			{
				if(vec[i].score<0)
				{
					temppos=i;
					break;
				}
			}
		}
	}
	if(dir==1)
	{
		for(i=0;i<MinCandidateLength;i++)
		{
			for(j=1;j<temppos;j++)
			{
				if(vec[0].CurStr[i]!=vec[j].CurStr[i])
				{
					return i;
				}
			}
		}
	}
	else
	{
		for(i=0;i<MinCandidateLength;i++)
		{
			for(j=1;j<temppos;j++)
			{
				if(vec[0].CurStr[vec[0].CurStr.size()-1-i]!=vec[j].CurStr[vec[j].CurStr.size()-1-i])
				{
					return i;
				}
			}
		}
	}
	return -1;
}

bool IsAllNegativeAndNoFit(vector<Node>vec,int & num)
{
	int i;
	for(i=0;i<vec.size();i++)
	{
		if(vec[i].score>0) return false;
		if(vec[i].score!=NegativeMin) num++;
	}
	if(num!=1) return true;
	else return false;
}

int GetMin(int a,int b)
{
	return a<b?a:b;
}

bool JudgeDifScore(int CurSize,double ds1,double ds2)
{
	if(CurSize>(lib[FileNum].MinSpan+lib[FileNum].MaxSpan)/2)
	{
		if(ds1<10) return (ds1<2*ds2);
		else if(ds2<20) return (ds1<1.9*ds2);
		else if(ds2<30) return (ds1<1.8*ds2);
		else if(ds2<40) return (ds1<1.7*ds2);
		else if(ds2<50) return (ds1<1.6*ds2);
		else if(ds2<60) return (ds1<1.5*ds2);
	}
	else
	{
		return (ds1<10*ds2);
	}
	return false;
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

bool IsInCondition(string FixStr,string Substr,int FileID)
{
	if(FixStr==Substr) return false;
	int pos=FixStr.find(Substr);
	if(pos==-1) return false;
	if(lib[FileID].MaxSpan-lib[FileID].MinSpan<300)
	{
		if(FixStr.size()-Substr.size()<=2*lib[FileID].ReadLength) return false;
	}
	else
	{
		if(FixStr.size()-Substr.size()<=4*lib[FileID].ReadLength) return false;
	}
	return true;
}

bool IsAllInCondition(vector<Node>vec,int FileID)
{
	int cntpositive=0;
	int cntzero=0;
	if(vec[0].score==0) cntzero++;
	else if(vec[0].score>0) cntpositive++;
	for(int i=1;i<vec.size();i++)
	{
		if(IsInCondition(vec[0].SupportStr,vec[i].SupportStr,FileID)==false) return false;
		if(vec[i].score==0) cntzero++;
		else if(vec[i].score>0) cntpositive++;
	}
	if(cntzero>cntpositive) return false;
	return true;
}

void Left_Extension(string &SeedStr)
{
	int i,j;
	int index;
	int difpos;//得分相差很小的两条contig之间第一个不同的点
	int MinCandidateLength;//candidate序列中的最短长度
	int TotalReads;//总共的reads
	int MatchReads;//match上的reads
	int NumOfNotInf;//在所有candidate有匹配的数目
	int dir;//表示方向，1表示向右，-1表示向左
	string MaxScoreString;//获得最大得分的字符串
	string SubSeedStr;
	string TempSubSeedStr;
	bool IsExtension;
	bool IsQuit;
	bool IsFitCondition;
	double perc;
	set<int> used;
	bool circled(false);
	int circleLen(0);
	while(1)
	{
		Node nd;
		Node tempnd;//保存从栈顶出来的元素
		while(!st.empty()) st.pop();//清空栈中元素
		SubSeedStr=SeedStr.substr(0,lib[1].MinOverLap);//取出长度为k-mer的字串去查找index
		index=StrtoInt[SubSeedStr];
		nd.CurStr=SubSeedStr;
		nd.index=index;
		st.push(nd);
		IsExtension=false;
		IsQuit=false;
		tempvec.clear();
		while(!st.empty()||tempvec.size()>0)//栈不为空或者临时容器的size大于1继续
		{
			if(st.empty()&&tempvec.size()>0)//栈中为空并且不止一个可能节点
			{
				MinCandidateLength=tempvec[0].CurStr.size();
				for(i=1;i<tempvec.size();i++)//找出candidate序列中的最短长度
				{
					if(tempvec[i].CurStr.size()<MinCandidateLength) MinCandidateLength=tempvec[i].CurStr.size();
				}
				IsFitCondition=false;//判断是否满足只去一个的条件，还是要多个candidate取公共的
				for(i=1;i<=FileNum;i++)
				{
					for(j=0;j<tempvec.size();j++)
					{
						if(SeedStr.size()<(lib[i].MinSpan+lib[i].MaxSpan)/2) continue;
						if(tempvec[j].score==NegativeMin) continue;
						TotalReads=0;
						MatchReads=0;
						double gs=GetScore(tempvec[j].SupportStr,SeedStr,tempvec[j].CurStr.substr(tempvec[j].CurStr.size()-GetMin(MinCandidateLength,lib[1].MinSpan)),i,TotalReads,MatchReads,-1);
						if(TotalReads==0)
						{
							tempvec[j].score=NegativeMin;
						}
						else
						{
							if(MatchReads)
							{
								perc=MatchReads*1.0/TotalReads;
								tempvec[j].score+=gs*perc;
							}
							else
							{
								if(TotalReads==1) tempvec[j].score=0;
								else tempvec[j].score=NegativeMin;
							}
						}
					}
					if(tempvec.size()>0)
					{
						sort(tempvec.begin(),tempvec.end(),cmp2);
						NumOfNotInf=0;
						if(IsAllNegativeAndNoFit(tempvec,NumOfNotInf))
						{
							IsQuit=true;
							break;
						}
						int veclen=tempvec.size();
						if(NumOfNotInf==1)
						{
							for(j=veclen-1;j>=0;j--)
							{
								if(tempvec[j].score==NegativeMin) tempvec.pop_back();
								else break;
							}
						}
						else
						{
							for(j=veclen-1;j>=0;j--)
							{
								if(tempvec[j].score<0) tempvec.pop_back();
								else break;
							}
						}
						if(IsAllInCondition(tempvec,i)==true)
						{
							IsFitCondition=true;
							break;
						}
					}
				}
				if(IsQuit==true) break;
				if(IsFitCondition==false)
				{
					difpos=GetFirstDifPos(SeedStr.size(),tempvec,MinCandidateLength,-1);
					if(difpos<lib[1].MinOverLap*1.2) IsQuit=true;
					else
					{
						SubSeedStr=tempvec[0].CurStr.substr(tempvec[0].CurStr.size()-difpos);
					}
					break;
				}
				else
				{
					SubSeedStr=tempvec[0].CurStr;
					break;
				}  
			}
			tempnd=st.top();
			st.pop();
			if(tempnd.CurStr.size()>MinExtensionLength)
			{
				tempnd.score=0;
				tempvec.push_back(tempnd);//把打到指定长度的序列存入临时容器
				SubSeedStr=tempnd.CurStr;
				IsExtension=true;
				continue;
			}
			for(i=0;i<vin[tempnd.index].size();i++)
			{
				nd = tempnd;
				nd.index=vin[tempnd.index][i];
				if(isused[nd.index]==true) continue;
				TempSubSeedStr=InttoStr[nd.index];
				if(IsNodeOk(TempSubSeedStr)==false)
				{
					continue;
				}
				nd.CurStr=TempSubSeedStr+tempnd.CurStr.substr(lib[1].MinOverLap-1);
				nd.collection.insert(nd.index);
				st.push(nd);
			}
		}
		if(false==IsExtension||true==IsQuit) break;
		else
		{
			SeedStr=SubSeedStr+SeedStr.substr(lib[1].MinOverLap);
			for(set<int>::iterator it=tempvec[0].collection.begin();it!=tempvec[0].collection.end();it++)
			{
				if(used.find(*it)!=used.end()){
					if(circled){
						circleLen += (InttoStr[*it].length() - lib[1].MinOverLap + 1);
					}else{
						circled = true;
						circleLen += (InttoStr[*it].length() - lib[1].MinOverLap + 1);
					}
					if(circleLen>lib[FileNum].MaxSpan){
						ofile<<"left"<<endl;
						if(circleLen>SeedStr.length())ofile<<circleLen<<' '<<SeedStr.length()<<endl;
						else SeedStr = SeedStr.substr(circleLen);
						return;
					}
				}else{
					used.insert(*it);
					circled = false;
					circleLen = 0;
				}
			}
		}
	}
}

void Right_Extension(string &SeedStr)
{
	int i,j;
	int index;
	int difpos;//得分相差很小的两条contig之间第一个不同的点
	int MinCandidateLength;//candidate序列中的最短长度
	int TotalReads;//总共的reads
	int MatchReads;//match上的reads
	int NumOfNotInf;//在所有candidate有匹配的数目
	int dir;//表示方向，1表示向右，-1表示向左
	string MaxScoreString;//获得最大得分的字符串
	string SubSeedStr;
	string TempSubSeedStr;
	bool IsExtension;
	bool IsQuit;
	bool IsFitCondition;
	double perc;
	set<int> used;
	int circleLen(0);
	bool circled(false);
	
	while(1)
	{
		Node nd;
		Node tempnd;//保存从栈顶出来的元素
		while(!st.empty()) st.pop();//清空栈中元素
		SubSeedStr=SeedStr.substr(SeedStr.size()-lib[1].MinOverLap);//取出长度为k-mer的字串去查找index
		index=StrtoInt[SubSeedStr];
		nd.CurStr=SubSeedStr;
		nd.index=index;
		st.push(nd);
		IsExtension=false;
		IsQuit=false;
		tempvec.clear();
		while(!st.empty()||tempvec.size()>0)//栈不为空或者临时容器的size大于1继续
		{
			if(st.empty()&&tempvec.size()>0)//栈中为空并且不止一个可能节点
			{
				MinCandidateLength=tempvec[0].CurStr.size();
				for(i=1;i<tempvec.size();i++)//找出candidate序列中的最短长度
				{
					if(tempvec[i].CurStr.size()<MinCandidateLength) MinCandidateLength=tempvec[i].CurStr.size();
				}
				IsFitCondition=false;
				for(i=1;i<=FileNum;i++)
				{
					for(j=0;j<tempvec.size();j++)
					{
						if(SeedStr.size()<(lib[i].MinSpan+lib[i].MaxSpan)/2) continue;
						if(tempvec[j].score==NegativeMin) continue;
						TotalReads=0;
						MatchReads=0;
						double gs=GetScore(tempvec[j].SupportStr,SeedStr,tempvec[j].CurStr.substr(0,GetMin(MinCandidateLength,lib[1].MinSpan)),i,TotalReads,MatchReads,1);
				
						if(TotalReads==0)
						{
							tempvec[j].score=NegativeMin;
						}
						else
						{
							if(MatchReads)
							{
								perc=MatchReads*1.0/TotalReads;
								tempvec[j].score+=gs*perc;
							}
							else
							{
								if(TotalReads==1) tempvec[j].score=0;
								else tempvec[j].score=NegativeMin;
							}
						}
					}
					if(tempvec.size()>0)
					{
						sort(tempvec.begin(),tempvec.end(),cmp2);
						NumOfNotInf=0;
						if(IsAllNegativeAndNoFit(tempvec,NumOfNotInf))
						{
							IsQuit=true;
							break;
						}
						int veclen=tempvec.size();
						if(NumOfNotInf==1)
						{
							for(j=veclen-1;j>=0;j--)
							{
								if(tempvec[j].score==NegativeMin) tempvec.pop_back();
								else break;
							}
						}
						else
						{
							for(j=veclen-1;j>=0;j--)
							{
								if(tempvec[j].score<0) tempvec.pop_back();
								else break;
							}
						}
						if(IsAllInCondition(tempvec,i)==true)
						{
							IsFitCondition=true;
							break;
						}
					}
				}
				if(IsQuit==true) break;
				if(IsFitCondition==false)
				{
					difpos=GetFirstDifPos(SeedStr.size(),tempvec,MinCandidateLength,1);
					if(difpos<lib[1].MinOverLap*1.2) IsQuit=true;
					else
					{
						SubSeedStr=tempvec[0].CurStr.substr(0,difpos);
					}
					break;
				}
				else
				{
					SubSeedStr=tempvec[0].CurStr;
					break;
				}  
			}
			tempnd=st.top();
			st.pop();
			if(tempnd.CurStr.size()>MinExtensionLength)
			{
				tempnd.score=0;
				tempvec.push_back(tempnd);//把打到指定长度的序列存入临时容器
				SubSeedStr=tempnd.CurStr;
				IsExtension=true;
				continue;
			}
			for(i=0;i<vout[tempnd.index].size();i++)
			{
				nd = tempnd;
				nd.index=vout[tempnd.index][i];
				nd.collection.insert(nd.index);
				if(isused[nd.index]==true) continue;
				TempSubSeedStr=InttoStr[nd.index];
				if(IsNodeOk(TempSubSeedStr)==false)
				{
					continue;
				}
				nd.CurStr=tempnd.CurStr.substr(0,tempnd.CurStr.size()-lib[1].MinOverLap+1)+TempSubSeedStr;
				st.push(nd);
			}
		}
		if(false==IsExtension||true==IsQuit) break;
		else
		{
			SeedStr=SeedStr.substr(0,SeedStr.size()-lib[1].MinOverLap)+SubSeedStr;
			for(set<int>::iterator it=tempvec[0].collection.begin();it!=tempvec[0].collection.end();it++)
			{
				if(used.find(*it)!=used.end()){
					if(circled){
						circleLen += (InttoStr[*it].length() - lib[1].MinOverLap + 1);
					}else{
						circled = true;
						circleLen += (InttoStr[*it].length() - lib[1].MinOverLap + 1);
					}
					if(circleLen>lib[FileNum].MaxSpan){
						ofile<<"right"<<endl;
						if(circleLen>SeedStr.length())ofile<<circleLen<<' '<<SeedStr.length()<<endl;
						else SeedStr = SeedStr.substr(0,SeedStr.length() - circleLen);
						return;;
					}
				}else{
					used.insert(*it);
					circled = false;
					circleLen = 0;
				}
			}
		}
	}
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

bool IsExistInContigSet(string SeedStr,int TotalNum)
{
	int i;
	for(i=0;i<TotalNum;i++)
	{
		int pos=contigs[i].find(SeedStr);
		if(pos!=-1) return true;
		SeedStr=Reverse_Compliment(SeedStr);
		pos=contigs[i].find(SeedStr);
		if(pos!=-1) return true;
	}
	return false;
}

void Seed_Extension()
{
	int i;
	int cnt=0;
	string SeedStr;
	ofile<<"contigs.size() = "<<contigs.size()<<endl;
	for(i=0;i<contigs.size();i++)
	{
		SeedStr=contigs[i];
		if(IsExistInContigSet(SeedStr,cnt)==true) continue;
		Right_Extension(SeedStr);
		Left_Extension(SeedStr);
		cout<<">"<<cnt<<endl;
		cout<<SeedStr<<endl;
		contigs[cnt]=SeedStr;
		cnt++;
		//************** debug information ***************
		prtm = ntm; 
		ntm = time(NULL);
		current_time = localtime(&ntm);
		ofile<<"cnt = "<<cnt<<", "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
		ofile<<"------------------------------------------"<<endl;
		prtm = ntm; 
		if(1033 == cnt)
			cnt = 1033;
		//************************************************
	}
}

int main()
{
	DIR *dir=NULL;
	dir = opendir("log");
	if(!dir){
		system("mkdir log");
	}
	ofile.open("log/extend.log");
	prtm = time(NULL); 
	ntm = prtm;
	current_time = localtime(&ntm);
	ofile<<"program start...\t"<< ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	
	ReadInputFile();
	
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
	
	
	setExtLen();
	
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"setExtLen over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
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
	
	freopen("contigs.init.fasta","w",stdout);
	Seed_Extension();
	//************** debug information ***************
	prtm = ntm; 
	ntm = time(NULL);
	current_time = localtime(&ntm);
	ofile<<"Seed_Extensions over, "<<ntm-prtm <<"(s)\t"<<current_time->tm_year+1900<<"/"<<current_time->tm_mon<<"/"<<current_time->tm_mday<<" "<<current_time->tm_hour<<":"<<current_time->tm_min<<":"<<current_time->tm_sec<<endl;
	ofile<<"------------------------------------------"<<endl;
	prtm = ntm; 
	//************************************************
	freopen("seedstring.txt","w",stdout);
	for(int i=0;i<cnt;i++)
	{
		if(isused[i]==true) continue;
		string seedstring=InttoStr[i];
		printf("%s\n",seedstring.c_str());
	}
	return 0;
}
