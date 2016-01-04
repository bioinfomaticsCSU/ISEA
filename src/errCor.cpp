#pragma warning(disable:4786)
#include<iostream>
#include<unordered_map>
#include<map>
#include<vector>
#include<stdio.h>
#include<string>
#include<string.h>
#include<algorithm>
#include<vector>
#include<pthread.h>
#include<unistd.h>
using namespace std;
const int maxtimes=4;
int NumOfThread;
const char ch[]={'A','C','G','T'};
unordered_map<string,bool>umpb;
char otherinfo[1000];
char temp1[1000],temp2[1000];
char FileIn[100];
char FileOut[100];
int MinOverLap;
int MaxOverLap;
int len;
int K;
int vecsize;
vector<string>vec;
int num;
int ins[5];
bool isok=false;
struct Comp
{
	vector<int>v;
};
vector<Comp>vcom;
struct sta
{
	vector<int>v;
};
unordered_map<string,sta>ump1;
unordered_map<string,sta>ump2;

unordered_map<string,sta>umpL;//存右边的mate
unordered_map<string,sta>umpR;//存左边的mate

unordered_map<string,bool>is_exist;

unordered_map<string,int>sta_times;
vector<string>sev;

int totcnt=0;

void *thread(void *arg);

class FastMutex{
	public:
	FastMutex(){
		if(pthread_mutex_init(&mt,NULL)!=0){
			printf("init mutex error\n");
			exit(-1);
		};
	};
	
	~FastMutex(){
		pthread_mutex_destroy(&mt);
	}
	void lock(){
		pthread_mutex_lock(&mt);
	};
	void unlock(){
		pthread_mutex_unlock(&mt);
	};
	
	private:
	pthread_mutex_t mt;
};

class Runnable{
	public:
		void start(){
			if(pthread_create(&tidp,NULL,thread,(void *) this)==-1)   //void *为“无类型指针”，void *  可以指向任何类型的数据
			{
				printf("create thread error!\n");
				exit(-1);
			}
		};

		void join(){
			pthread_join(tidp,&a);
		};

		virtual void run(){};
	public:
		pthread_t tidp;
		void* a;
};

void *thread(void *arg)   
{
	Runnable *temp;
    temp=(Runnable *)arg;  
	temp->run();
	return (void *)0;     
};

bool Read_Filter(string str)
{
	int i;
	int len=str.size();
	int scnt[5];
	memset(scnt,0,sizeof(scnt));
	for(i=0;i<len;i++)
	{
		if(str[i]=='A') scnt[0]++;
		else if(str[i]=='C') scnt[1]++;
		else if(str[i]=='G') scnt[2]++;
		else if(str[i]=='T') scnt[3]++;
		else scnt[4]++;
	}
	for(i=0;i<=4;i++) if(scnt[i]>len*3/4) return false;
	return true;
}

bool IsChange(int pos)
{
	int i,j;
	Comp cp;
	cp=vcom[pos];
	for(j=0;j<cp.v.size();j++)
	{
		if(cp.v[j]<=1) return true;
	}
	return false;
}

bool Is_equal(string str1,string str2,int errornum)
{
	int i;
	int len=str1.size();
	int cnt=0;
	for(i=0;i<len;i++)
	{
		if(str1[i]!=str2[i]) cnt++;
		if(cnt>errornum) return false;
	}
	return true;
}

bool Overlap_LMate(string str,string LMate)
{
	int i,j;
	string temp;
	string ssub;
	unordered_map<string,sta>::iterator it;
	it=umpL.find(str);
	if(it==umpL.end()) return false;
	for(i=0;i<umpL[str].v.size();i++)
	{
		temp=vec[umpL[str].v[i]];
		for(j=MaxOverLap/2;j<=MaxOverLap;j++)
		{
			ssub=temp.substr(0,j);
			if(Is_equal(ssub,LMate.substr(LMate.size()-j),1)==true) return true;
			ssub=temp.substr(temp.size()-j);
			if(Is_equal(ssub,LMate.substr(0,j),1)==true) return true;
		}
	}
	return false;
}

bool Overlap_RMate(string str,string RMate)
{
	int i,j;
	string temp;
	string ssub;
	unordered_map<string,sta>::iterator it;
	it=umpR.find(str);
	if(it==umpR.end()) return false;
	for(i=0;i<umpR[str].v.size();i++)
	{
		temp=vec[umpR[str].v[i]];
		for(j=MaxOverLap/2;j<=MaxOverLap;j++)
		{
			ssub=temp.substr(0,j);
			if(Is_equal(ssub,RMate.substr(RMate.size()-j),1)==true) return true;
			ssub=temp.substr(temp.size()-j);
			if(Is_equal(ssub,RMate.substr(0,j),1)==true) return true;
		}
	}
	return false;
}


class MyWorker : public Runnable
{
public:
	MyWorker() {}
	MyWorker(int k, FastMutex *fm) : n(k), fm(fm) {}
	
	virtual void run()
	{
		for(int i=n*vec.size()/NumOfThread;i<(n+1)*vec.size()/NumOfThread;i++)
		{
			times=0;
			matchstr=vec[i];
			if(umpb[matchstr]==false) continue;
			savestr=vec[i];
			while(1)
			{
				oristr=matchstr;
				memset(cnt,0,sizeof(cnt));
				for(int k=MinOverLap;k<=MaxOverLap;k++)
				{
					ve.clear();
					vector<string>().swap(ve);
					tk=matchstr.substr(matchstr.size()-k,k);
					it=ump1.find(tk);
					if(it!=ump1.end())
					{
						st=it->second;
						for(int p=0;p<st.v.size();p++) ve.push_back(sev[st.v[p]]);
					}
					for(int p=0;p<tk.size();p++)
					{
						tempstr=tk;
						tempch=tempstr[p];
						for(int u=0;u<4;u++)
						{
							if(tempch==ch[u]) continue;
							tempstr[p]=ch[u];
							it=ump1.find(tempstr);
							if(it!=ump1.end())
							{
								st=it->second;
								for(int r=0;r<st.v.size();r++)
								{
									if(isok==true)
									{
										if(i%2==1)
										{
											fm->lock();
											vs1=sev[st.v[r]];
											vs2=vec[i-1];
											fm->unlock();
											if(Overlap_LMate(vs1,vs2)==true)
											{
												ve.push_back(vs1);
											}
										}
										else
										{
											fm->lock();
											vs1=sev[st.v[r]];
											vs2=vec[i+1];
											fm->unlock();
											if(Overlap_RMate(vs1,vs2)==true)
											{
												ve.push_back(vs1);
											}
										}
									}
									else
									{
										ve.push_back(sev[st.v[r]]);
									}
								}
							}
						}
					}
					for(int l=0;l<ve.size();l++)
					{
						int cnttime=sta_times[ve[l]];
						stemp=ve[l].substr(0,k);
						slen=stemp.size();
						for(int p=0;p<slen;p++)
						{
							if(stemp[p]=='A') cnt[ve[l].size()-slen+p][0]+=cnttime;
							else if(stemp[p]=='C') cnt[ve[l].size()-slen+p][1]+=cnttime;
							else if(stemp[p]=='G') cnt[ve[l].size()-slen+p][2]+=cnttime;
							else if(stemp[p]=='T') cnt[ve[l].size()-slen+p][3]+=cnttime;
						}
					}
					
					ve.clear();
					vector<string>().swap(ve);
					tk=matchstr.substr(0,k);
					it=ump2.find(tk);
					if(it!=ump2.end())
					{
						st=it->second;
						for(int p=0;p<st.v.size();p++) ve.push_back(sev[st.v[p]]);
					}
					for(int p=0;p<tk.size();p++)
					{
						tempstr=tk;
						tempch=tempstr[p];
						for(int u=0;u<4;u++)
						{
							if(tempch==ch[u]) continue;
							tempstr[p]=ch[u];
							it=ump2.find(tempstr);
							if(it!=ump2.end())
							{
								st=it->second;
								for(int r=0;r<st.v.size();r++)
								{
									if(isok==true)
									{
										if(i%2==1)
										{
											fm->lock();
											vs1=sev[st.v[r]];
											vs2=vec[i-1];
											fm->unlock();
											if(Overlap_LMate(vs1,vs2)==true)
											{
												ve.push_back(vs1);
											}
										}
										else
										{
											fm->lock();
											vs1=sev[st.v[r]];
											vs2=vec[i+1];
											fm->unlock();
											if(Overlap_RMate(vs1,vs2)==true)
											{
												ve.push_back(vs1);
											}
										}
									}
									else 
									{
										ve.push_back(sev[st.v[r]]);
									}
								}
							}
						}
					}
					for(int l=0;l<ve.size();l++)
					{
						int cnttime=sta_times[ve[l]];
						stemp=ve[l].substr(ve[l].size()-k,k);
						slen=stemp.size();
						for(int p=0;p<slen;p++)
						{
							if(stemp[p]=='A') cnt[p][0]+=cnttime;
							else if(stemp[p]=='C') cnt[p][1]+=cnttime;
							else if(stemp[p]=='G') cnt[p][2]+=cnttime;
							else if(stemp[p]=='T') cnt[p][3]+=cnttime;
						}
					}
				} 
				for(int k=0;k<matchstr.size();k++)
				{
					chr=matchstr[k];
					if(chr=='A') timec=cnt[k][0];
					else if(chr=='C') timec=cnt[k][1];
					else if(chr=='G') timec=cnt[k][2];
					else if(chr=='T') timec=cnt[k][3];
					if(cnt[k][0]>timec)
					{
						chr='A';
						timec=cnt[k][0];
					}
					if(cnt[k][1]>timec)
					{
						chr='C';
						timec=cnt[k][1];
					}
					if(cnt[k][2]>timec)
					{
						chr='G';
						timec=cnt[k][2];
					}
					if(cnt[k][3]>timec)
					{
						chr='T';
						timec=cnt[k][3];
					}
					matchstr[k]=chr;
				}
				times++;
				if(matchstr==oristr) break;
				if(times>maxtimes) break;
			}
			if(times>maxtimes&&matchstr!=oristr) continue;
			mismatch=0;
			int k;
			for(k=0;k<matchstr.size();k++)
			{
				if(matchstr[k]!=savestr[k]) mismatch++;
			}
			if(mismatch<0.12*len)
			{
				fm->lock();
				vec[i]=matchstr;
				fm->unlock();
			}
		}
	}
private:
	FastMutex *fm;
	int n;
	int times;
	int mismatch;
	sta st;
	string savestr;
	string matchstr;
	string oristr;
	string tempstr;
	string tk;
	string stemp;
	string vs1;
	string vs2;
	unordered_map<string,sta>::iterator it;
	char tempch;
	int timec;
	int slen;
	char chr;
	int cnt[100][4];
	vector<string>ve;
};

class MyProcess : public Runnable
{
public:
	MyProcess() {}
	MyProcess(int k, FastMutex *fm) : n(k), fm(fm) {}
	
	virtual void run()
	{
		for(int j=n*sev.size()/NumOfThread;j<(n+1)*sev.size()/NumOfThread;j++)
		{
			tempstr=sev[j];
			for(int i=MinOverLap;i<=MaxOverLap;i++)
			{
				keystr=tempstr.substr(0,i);
				fm->lock();
				ump1[keystr].v.push_back(j);
				fm->unlock();
			}
			for(int i=MinOverLap;i<=MaxOverLap;i++)
			{
				keystr=tempstr.substr(tempstr.size()-i,i);
				fm->lock();
				ump2[keystr].v.push_back(j);
				fm->unlock();
			}
		}
	}
	
private:
	FastMutex *fm;
	int n;
	string tempstr;
	string keystr;
};

int main(int argc,char* argv[])
{
	char leftfasta[4096], righfasta[4096], type[10];
	if(!freopen("mapping/inputfiles.info","r",stdin)){
		perror("open mapping/inputfiles.info failed.\n");
		exit(-1);
	};
	cin>>NumOfThread;
	int i,j;
	char TempNum[10];
	char LName[20];
	len=0;
	FastMutex mut;
	string temps;
	string temps1;
	string temps2;

	int FileNum, kmer;
	int Mins,Maxs,Mino,Type,nouse1,nouse2;
	if(!freopen("lib.info","r",stdin)){
		perror("lib.info");
	};
	scanf("%d",&FileNum);
	scanf("%d",&kmer);
	for(i=1;i<=FileNum;i++)
	{
		scanf("%s%d%d%d%d%d",LName,&Mins,&Maxs,&Mino,&nouse1,&nouse2);
		ins[i]=(Maxs-Mins)/2;
	}
	strcpy(FileOut,argv[1]);
	freopen(FileOut,"w",stdout);
	int idnum;
	for(i=0;i<strlen(FileOut);i++)
	{
		if(FileOut[i]>='0'&&FileOut[i]<='9')
		{
			idnum=FileOut[i]-'0';
			break;
		}
	}

	if(ins[idnum]<100) isok=true;
	else isok=false;

	char StaName[100];

	sprintf(StaName,"sta_%d.txt",idnum);
	freopen(StaName,"r",stdin);
	while(scanf("%s",temp1)!=EOF)
	{
		vec.push_back((string)temp1);
		sta_times[(string)temp1]++;
		if(!len)
		{
			len=strlen(temp1);
			MaxOverLap=len-1;
			K=((len/2)+(len*2/3))/2+1;
			if(argc==3){
				MinOverLap = atoi(argv[2]);
			}else MinOverLap=len*2/3+1;
		}
		vcom.push_back(Comp());
		for(j=0;j<=len-K;j++)
		{
			scanf("%d",&num);
			vcom.back().v.push_back(num);
		}
	}
	unordered_map<string,int>::iterator ite=sta_times.begin();
	while(ite!=sta_times.end())
	{
		string tsv=ite->first;
		sev.push_back(tsv);
		ite++;
	}
	vecsize=vec.size();
	for(i=0;i<vecsize;i++)
	{
		temps=vec[i];
		if(is_exist[temps]==true) continue;
		if(Read_Filter(temps)==false)
		{
			umpb[temps]=false;
			continue;
		}
		umpb[temps]=IsChange(i);
		is_exist[temps]=true;
	}
	
	for(i=0;i<vecsize/2;i++)
	{
		temps1=vec[2*i];
		temps2=vec[2*i+1];
		umpL[temps2].v.push_back(2*i);
		umpR[temps1].v.push_back(2*i+1);
	}
	
	if(MinOverLap!=(len*2/3+1)){
		vector<Comp>().swap(vcom);
		unordered_map<string,bool>().swap(is_exist);
	}
	
	MyProcess p[NumOfThread+1];
	for(i=0;i<NumOfThread;i++) p[i]=MyProcess(i, &mut);
	for(i=0;i<NumOfThread;i++) p[i].start();
	for(i=0;i<NumOfThread;i++) p[i].join();  // wait for all threads to end

	MyWorker w[NumOfThread];
	for(i=0;i<NumOfThread;i++) w[i] = MyWorker(i, &mut);
	for(i=0;i<NumOfThread;i++) w[i].start();
	for(i=0;i<NumOfThread;i++) w[i].join();  // wait for all threads to end


	int cnt=0;
	for(i=0;i<vecsize/2;i++)
	{
		cout<<">"<<i<<"\n";
		cout<<vec[2*i].c_str()<<"\t"<<vec[2*i+1].c_str()<<endl;
	}
	_exit(0);
	return 0;
}
