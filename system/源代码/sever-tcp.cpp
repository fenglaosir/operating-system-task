#include<bits/stdc++.h>
#include "winsock2.h"
 #include<cstdlib>
#pragma comment(lib,"ws2_32.lib")
using namespace std;



void scre(char [],SOCKET);

void scwait()
{
	WSADATA wsaData;
    int port = 8888;//端口号
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("初始化失败");
        exit(0);
    }
 
    //创建用于监听的套接字,即服务端的套接字
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
 
    SOCKADDR_IN addrSrv;
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port); //1024以上的端口号
    /**
     * INADDR_ANY就是指定地址为0.0.0.0的地址，这个地址事实上表示不确定地址，或“所有地址”、“任意地址”。
     * 一般来说，在各个系统中均定义成为0值。
     */
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
 
    int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
    if(retVal == SOCKET_ERROR){
        printf("连接失败:%d\n", WSAGetLastError());
        exit(0);
    }
 
    if(listen(sockSrv,10) ==SOCKET_ERROR){
        printf("监听失败:%d", WSAGetLastError());
        exit(0);
    }
 
    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);
	struct sockaddr_in caddr; //方便取数据
    int addrlen = sizeof(caddr);
	while(true)
	{
		// 接收连接请求 
		SOCKET cfd = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
		cout<<"新连接\n";
		if(cfd!=SOCKET_ERROR)
		{
			char usern[1024],pass[1024];
			const char *ok="ok",*no="no";
			recv(cfd,usern,sizeof(usern),0);
			recv(cfd,pass,sizeof(pass),0);
			string user=usern,password=pass;
			// 超级用户 
			if(user=="root")
			{
				if(password=="124")
				{
					cout<<"登录成功\n"; 
					send(cfd,ok,sizeof(ok),0);
				}
				else
				{
					cout<<"密码错误\n";
					send(cfd,no,sizeof(no),0);
					continue;
				}
			}
			else
			{
				// 普通用户 
				ifstream userfile;
				userfile.open("user/"+user,ios::in);
				if(userfile.good())
				{
					string pass;
					userfile>>pass;
					if(password==pass)
					{
						cout<<"登陆成功\n";
						send(cfd,ok,sizeof(ok),0);
					}
					else
					{
						cout<<"密码错误\n";
						send(cfd,no,sizeof(no),0);
						continue;
					}
				}
				else
				{
					cout<<"用户不存在\n";
					send(cfd,no,sizeof(no),0);
					continue;
				}
				userfile.close();
			}
			// 创建用户线程 
			thread sre(scre,usern,cfd);
			sre.detach();
		}
	}
}


// str类，储存代替string 
struct str
{
	char x[1024];
	str(){
		;
	}
	str(const string &y)
	{
		strcpy(x,y.c_str());
	}
	bool operator<(const str y) const
	{
		return strcmp(x,y.x)<0;
	}
	bool operator==(const str y) const
	{
		return strcmp(x,y.x)==0;
	}
	bool operator==(const string y) const
	{
		return y==x;
	}
	void operator=(const str &y)
	{
		strcpy(x,y.x);
	}
	void operator=(const string &y)
	{
		strcpy(x,y.c_str());
	}
	void operator=(const char *y)
	{
		strcpy(x,y);
	}
};
// 文件/目录 
struct file
{
	int p;
	bool dir,lock;
	str name,text;
	map<str,file> next;
	file *father;
	file()
	{
		dir=true;
		lock=false;
	}

	bool operator<(const file x) const
	{
		if(dir!=x.dir) return dir;
		return name<x.name;
	}
};


// 储存代替file 
struct filec
{
	bool dir,lock;
	str name,text;
	filec(){
	}
	filec(file &x)
	{
		dir=x.dir;
		lock=x.lock;
		name=x.name;
		text=x.text;
	}
};
// filec转化为file 
file tofile(filec x)
{
	file y;
	y.dir=x.dir;
	y.lock=x.lock;
	y.name=x.name;
	y.text=x.text;
	return y;
}
// 储存模块 
vector<filec> save_d;
vector<set<int>> save_m;
//用户数据 
struct users
{
	string username,path,std;
	SOCKET cfd;
	file* dir;
	int stdi;
	file *stf;
	file jtb;
};
// 根目录 
file root;

// 深度优先读取文件 
void dfs(file &ro,int k)
{
	ro.p=k;
	for(auto item:save_m[k])
	{
		file x=tofile(save_d[item]);
		x.father=&ro;
		ro.next[x.name]=x;
		dfs(ro.next[x.name],item);
	}
}

void getdata()
{
	ifstream data,mod;
	//读取data文件 
	data.open("home/data.dat",ios::binary|ios::in);
	mod.open("home/mod.dat",ios::binary|ios::in);
	if(data.good()&&mod.good())
	{
		cout<<"开始读取\n";
		filec item;
		while (data.read(reinterpret_cast<char*>(&item), sizeof(filec))) 
		{
        	save_d.push_back(item);
    	}
    	data.close();
    	size_t setSize;
    	while (mod.read(reinterpret_cast<char*>(&setSize), sizeof(size_t))) 
		{
	        set<int> dataSet;
	        for (size_t i = 0; i < setSize; ++i) 
			{
	            int element;
	            mod.read(reinterpret_cast<char*>(&element), sizeof(int));
	            dataSet.insert(element);
	        }
	        save_m.push_back(dataSet);
    	}
    	dfs(root,0);
    	root.dir=true;
    	root.name="root";
    	root.p=0;
		cout<<"读取完毕\n";
		
	}
	else
	{
		//如果文件不存在，更新root文件 
		root.dir=true;
		root.name="root";
		root.p=0;
		save_d.push_back(filec(root));
		set<int> x;
		save_m.push_back(x);
	}
	
	data.close();
}
void save()
{
	ofstream data,mod;
	data.open("home/data.dat",ios::binary|ios::out|ios::trunc);
	mod.open("home/mod.dat",ios::binary|ios::out|ios::trunc);
	for(const filec& item : save_d)
	{
		data.write(reinterpret_cast<const char*>(&item), sizeof(filec));
	}
	data.close();
	for (const auto& dataSet : save_m) {
        // 写入集合大小
        size_t setSize = dataSet.size();
        mod.write(reinterpret_cast<const char*>(&setSize), sizeof(size_t));

        // 逐个写入集合元素
        for (int element : dataSet) {
            mod.write(reinterpret_cast<const char*>(&element), sizeof(int));
        }
    }
    mod.close();
}

// 指令 

//创建用户 
struct adduser
{
	string username,password;
	adduser(string &res,users &use,vector<string> v)
	{
		username=v[1];
		if(v.size()<3)
		{
			res+="请设置密码";
			return;
		}
		password=v[2];
		ofstream userfile;
		userfile.open("user/"+username,ios::out);
		userfile<<password;
		userfile.close();
		res+=username+"创建成功\n"; 
	}
};

//创建文件 
struct create
{
	string name;
	create(string &res,users &use,vector<string> v)
	{
		file x;
		x.p=save_d.size();
		name=v[1];
		str nam=name;
		if(use.dir->next.find(nam)!=use.dir->next.end())
		{
			res+="存在同名文件\n";
			return; 
		}
		x.name=name;
		x.dir=false;
		x.father=use.dir;
		x.text="";
		use.dir->next[nam]=x;
		save_d.push_back(filec(x));
		set<int> s;
		save_m.push_back(s);
		save_m[use.dir->p].insert(x.p);
	}
};

//进入目录 
struct cd
{
	string name;
	cd(string &res,users &use,vector<string> v)
	{
		name=v[1];
		// 返回上一级 
		if(name=="..")
		{
			if(use.dir->name==use.username) return;
			use.dir=use.dir->father;
			use.path.pop_back();
			while(use.path.back()!='/') use.path.pop_back();
			use.path.pop_back();
			return;
		}
		// 进入目录 
		if(use.dir->next.find(name)!=use.dir->next.end()&&use.dir->next[name].dir)
		{
			if(use.dir->next[name].lock)
			{
				res+="路径被上锁";
				return;
			}
			use.dir=&use.dir->next[name];
			use.path+="/"+name;
		}
		else
		{
			res+="路径不存在\n";
		}
	}
};

//创建目录 
struct mkdir
{
	string name;
	mkdir(string &res,users &use,vector<string> v)
	{
		file x;
		x.p=save_d.size();
		name=v[1];
		if(use.dir->next.find(name)!=use.dir->next.end())
		{
			res+="存在同名文件\n";
			return; 
		}
		x.name=name;
		x.dir=true;
		x.father=use.dir;
		use.dir->next[name]=x;
		save_d.push_back(filec(x));
		set<int> s;
		save_m.push_back(s);
		save_m[use.dir->p].insert(x.p);
	}
};

//删除文件/目录 
struct _delete
{
	string name;
	_delete(string &res,users &use,vector<string> v)
	{
		name=v[1];
		if(use.dir->next.find(name)!=use.dir->next.end())
		{
			save_m[use.dir->p].erase(use.dir->next[name].p);
			use.dir->next.erase(name);
		}
		else
		{
			res+="找不到文件";
		}
	}
};

//打开文件 
struct open
{
	string name;
	open(string &res,users &use,vector<string> v)
	{
		name=v[1];
		if(use.dir->next.find(name)!=use.dir->next.end())
		{
			if(use.dir->next[name].lock)
			{
				res+="文件被加锁";
				return; 
			}
			//进入缓冲区 
			use.std=use.dir->next[name].text.x;
			use.stf=&use.dir->next[name];
		}
		else
		{
			res+="找不到文件";
		}
	}
};

//关闭文件 
struct close
{
	close(string &res,users &use,vector<string> v)
	{
		use.stf->text=use.std;
		save_d[use.stf->p]=*use.stf;
		use.stf=NULL;
		use.std="";
		use.stdi=0;
	}
};

//读取 
struct read
{
	read(string &res,users &use,vector<string> v)
	{
		if(!use.stf)
		{
			res+="未打开文件";
			return;
		}
		while(use.stdi<use.std.length()&&use.std[use.stdi]!='\n'&&use.std[use.stdi]!='\0')
		{
			res+=use.std[use.stdi];
			use.stdi++;
		}
		if(use.std[use.stdi]=='\n') use.stdi++;
	}
};

//写入 
struct write
{
	write(string &res,users &use,vector<string> v)
	{
		if(use.stf)
		{
			use.std.insert(use.stdi,v[1]+"\n");
			use.stdi+=v[1].length()+1;
		}
		else
		{
			res+="未打开文件";
		}
	}
};
void Stringsplit(string str, const char split,vector<string>& res);
// 移动文件 
struct _move
{
	_move(string &res,users &use,vector<string> v)
	{
		if(use.dir->next.find(v[1])!=use.dir->next.end())
		{
			use.jtb=use.dir->next[v[1]];
		}
		else
		{
			res+="文件不存在";
			return;
		}
		file *tdir=&root;
		if(use.username!="root") tdir=&tdir->next[use.username];
		vector<string> ve;
		Stringsplit(v[2],'/',ve);
		for(int i=1;i<ve.size();++i)
		{
			if(tdir->next.find(ve[i])!=tdir->next.end())
			{
				tdir=&tdir->next[ve[i]];
			}
			else
			{
				res+="路径不存在";
				return;
			}
		}
		if(tdir->next.find(v[1])!=tdir->next.end())
		{
			res+="有重名文件";
			return;
		}
		file x;
		
		x=use.jtb;
		x.p=save_d.size();
		x.father=tdir;
		tdir->next[x.name]=x;
		save_d.push_back(filec(x));
		set<int> s;
		save_m.push_back(s);
		save_m[tdir->p].insert(x.p);
		save_m[use.dir->p].erase(use.jtb.p);
		use.dir->next.erase(v[1]);
		
	}
};

//复制文件 
struct _copy
{
	_copy(string &res,users &use,vector<string> v)
	{
		if(use.dir->next.find(v[1])!=use.dir->next.end())
		{
			use.jtb=use.dir->next[v[1]];
		}
		else
		{
			res+="文件不存在";
			return;
		}
		file *tdir=&root;
		if(use.username!="root") tdir=&tdir->next[use.username];
		vector<string> ve;
		Stringsplit(v[2],'/',ve);
		for(int i=1;i<ve.size();++i)
		{
			if(tdir->next.find(ve[i])!=tdir->next.end())
			{
				tdir=&tdir->next[ve[i]];
			}
			else
			{
				res+="路径不存在";
				return;
			}
		}
		if(tdir->next.find(v[1])!=tdir->next.end())
		{
			res+="有重名文件";
			return;
		}
		file x;
		x=use.jtb;
		x.p=save_d.size();
		x.father=tdir;
		tdir->next[x.name]=x;
		save_d.push_back(filec(x));
		set<int> s;
		save_m.push_back(s);
		save_m[tdir->p].insert(x.p);
	}
};
// 加锁 
struct _lock
{
	string name;
	_lock(string &res,users &use,vector<string> v)
	{
		name=v[1];
		if(use.dir->next.find(name)!=use.dir->next.end())
		{
			use.dir->next[name].lock=!use.dir->next[name].lock;
		}
		else
		{
			res+="找不到文件";
		}
	}
};

// 读取前n行 
struct head
{
	head(string &res,users &use,vector<string> v)
	{
		// 字符串转化数字 
		int n=stoi(v[1]);
		vector<string> ve;
		Stringsplit(use.std,'\n',ve);
		for(int i=0;i<n&&i<ve.size();++i)
		{
			res+=ve[i]+"\n";
		}
	}
};
//读取后n行 
struct tail
{
	tail(string &res,users &use,vector<string> v)
	{
		int n=stoi(v[1]);
		vector<string> ve;
		Stringsplit(use.std,'\n',ve);
		for(int i=max((int)ve.size()-n,0);i<ve.size();++i)
		{
			res+=ve[i]+"\n";
		}
	}
};
//移动指针 
struct lseek
{
	lseek(string &res,users &use,vector<string> v)
	{
		int n=stoi(v[1]);
		use.stdi+=n;
		use.stdi=max(0,use.stdi);
		use.stdi=min((int)use.std.length(),use.stdi);
	}
};
//显示目录内容 
struct dir
{
	dir(string &res,users &use,vector<string> v)
	{
		for(auto i:use.dir->next)
		{
			string w=i.first.x;
			if(i.second.dir) w+="/";
			res+=w+" ";
		}
	}
};
//导入文件 
struct import
{
	import(string &res,users &use,vector<string> v)
	{
		ifstream imp;
		imp.open(v[1],ios::in);
		if(imp.good())
		{
			file x;
			x.dir=false;
			x.p=save_d.size();
			vector<string> ve;
			Stringsplit(v[1],'/',ve);
			x.name=ve[ve.size()-1];
			string txt,tet="";
			while(imp>>txt)
			{
				txt+="\n";
				tet+=txt;
			}
			x.text=tet;
			use.dir->next[x.name]=x;
			save_d.push_back(filec(x));
			set<int> s;
			save_m.push_back(s);
			save_m[use.dir->p].insert(x.p);
		}
		else
		{
			res+="找不到文件"; 
		}
		imp.close();
	}
};

//导出文件 
struct _export
{
	_export(string &res,users &use,vector<string> v)
	{
		ofstream imp;
		imp.open(v[1]+v[2],ios::out);
		if(imp.good())
		{
			file x;
			x=use.dir->next[v[2]];
			imp<<x.text.x;
		}
		imp.close();
	}
};
// 切分字符串 
void Stringsplit(string str, const char split,vector<string>& res)
{
	istringstream iss(str);	// 输入流
	string token;			// 接收缓冲区
	while (getline(iss, token, split))	// 以split为分隔符
	{
		if(token!="") res.push_back(token);
	}
}


// 运行函数 
void run(string io,string &res,users &use)
{
	vector<string> vec;
	// 将指令和参数分开 
	Stringsplit(io,' ',vec);
	if(vec[0]=="adduser")
	{
		adduser(res,use,vec);
	}
	else if(vec[0]=="create") create(res,use,vec);
	else if(vec[0]=="mkdir") mkdir(res,use,vec);
	else if(vec[0]=="cd") cd(res,use,vec);
	else if(vec[0]=="delete") _delete(res,use,vec);
	else if(vec[0]=="open") open(res,use,vec);
	else if(vec[0]=="close") close(res,use,vec); 
	else if(vec[0]=="read") read(res,use,vec);
	else if(vec[0]=="write") write(res,use,vec);
	else if(vec[0]=="move") _move(res,use,vec);
	else if(vec[0]=="copy") _copy(res,use,vec);
	else if(vec[0]=="lock") _lock(res,use,vec);
	else if(vec[0]=="head") head(res,use,vec);
	else if(vec[0]=="tail") tail(res,use,vec);
	else if(vec[0]=="lseek") lseek(res,use,vec);
	else if(vec[0]=="dir") dir(res,use,vec);
	else if(vec[0]=="import") import(res,use,vec); 
	else if(vec[0]=="export") _export(res,use,vec);
	else res+="找不到指令:"+vec[0]; 
}

// 用户线程 
void scre(char *use,SOCKET cfd)
{
	char buffer[1024];
	string user,path;
	user=use;
	path="/"+user;
	users x;
	x.username=user;
	x.path=path;
	x.cfd=cfd;
	x.dir=&root;
	//创建/进入用户文件夹 
	if(user!="root")
	{
		if(root.next.find(user)!=root.next.end())
		{
			x.dir=&root.next[user];
		}
		else
		{
			file ex;
			ex.p=save_d.size();
			ex.name=user;
			ex.dir=true;
			ex.father=&root;
			root.next[user]=ex;
			save_d.push_back(filec(ex));
			set<int> se;
			save_m.push_back(se);
			save_m[0].insert(ex.p);
			x.dir=&root.next[user];
		}
	}
	// 指令循环 
	while(true)
	{
		//发送指令前缀 
		string load=user+":~"+x.path;
		if(user=="root") load+="# ";
		else load+="$ "; 
		char sto[1024];
		strcpy(sto,load.c_str());
		int sed=send(cfd,sto,sizeof(sto),0);
		if(sed == SOCKET_ERROR)
		{
			cout<<"连接丢失\n"; 
			break;
		}
		//接收指令 
		int rece=recv(cfd,buffer,sizeof(buffer),0);
		if(!(rece>=0))
		{
			cout<<"连接丢失\n"; 
			break;
		}
		string runc=buffer; 
		//运行 
		string s="";
		run(runc,s,x);
		strcpy(buffer,s.c_str());
		//发送反馈 
		send(cfd,buffer,sizeof(buffer),0);
		//保存 
		save();
	}
}

int main()
{
	system("mkdir home");
	system("mkdir user"); 
	getdata();
	scwait();
}
