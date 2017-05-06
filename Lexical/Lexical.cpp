#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<tuple>
#include<set>
#include<algorithm>
#include<iterator>
using namespace std;

map<char,vector<tuple<char,char,char> >>NFA;//存放NFA
map<char,vector<tuple<int,char,int> >>DFA;//存放DFA
map<char,bool>Terminal,N_Terminal;//终结符集，非终结符集
vector<pair<vector<char>, int> >ivec;//最终构造的子集
set<char>move1;//存放求闭包后的结果
map<string,int>Key,Operator;//关键字，运算符
map<char,int>Delimiter;//分界符
vector<pair<string,int> >Out;//输出单词和对应的类型
int dcount;//用来存放DFA的子集个数
int dfa_num;//记录DFA中开始状态的集合下标
char Start;//开始符号

void bulid_NFA()//构造NFA
{
    ifstream fin("Lex_Gra.txt",ios::in);
    char line[1024]={0};
    char z,x,c,v,b;
    int i=0;
    while(fin.getline(line,sizeof(line)))
    {
        stringstream word(line);
        word>>z>>x>>c>>v>>b;
        NFA[i].push_back(make_tuple(z,v,b));//正规文法转换成了NFA
        i++;
    }
    fin.clear();
    fin.close();
    vector<tuple<char,char,char> >::iterator it;
}

void bulid_Tem_and_NTem()//NFA用子集法构造DFA
{
    vector<tuple<char,char,char> >::iterator it;
    map<char,bool>::iterator Te;
    //构造终结符集,非终结符
    int flag=0;
    for(int j=0;j<NFA.size();j++)
    {
        for(it=NFA[j].begin();it!=NFA[j].end();it++)
        {
            if(flag==0)
            {
                Start=get<0>(*it);
                flag=1;
            }
            Terminal[get<1>(*it)]=true;
            N_Terminal[get<0>(*it)]=true;
        }
    }
    N_Terminal[Start]=false;
    //验证终结符是否进入map容器
    for(Te=Terminal.begin();Te!=Terminal.end();Te++)
    {
        cout<<"<终结符，bool>=<"<<Te->first<<","<<Te->second<<">"<<endl;
    }
    //验证非终结符是否进入map容器
    for(Te=N_Terminal.begin();Te!=N_Terminal.end();Te++)
    {
        cout<<"<非终结符，bool>=<"<<Te->first<<","<<Te->second<<">"<<endl;
    }
}

void NFA_TO_DFA()
{
    vector<tuple<char,char,char> >::iterator it;
    vector<tuple<int,char,int> >::iterator dt;
    map<char,bool>::iterator Te;
    set<char>::iterator mo;
    it=NFA[0].begin();
    ivec.push_back(make_pair(vector<char>(),0));
    ivec[0].first.push_back(Start);
        for(int k=0;k<NFA.size();k++) //循坏NFA找到原来状态经过终结符a后
        {
            it=NFA[k].begin();
            if(get<0>(*it)==Start&&get<1>(*it)=='@')
            {
                ivec[0].first.push_back('#');
            }
            it++;
        }
    int add=1;//用来增加子集
    for(mo=move1.begin();mo!=move1.end();mo++)
    {
        printf("move=%c ",*mo);
    }
    printf("\n");
    for(int j=0;j<ivec.size();j++)
    {
        if(ivec[j].second==0)
        {
            ivec[j].second=1;
            for(Te=Terminal.begin();Te!=Terminal.end();Te++)//每输入一个终结符a
            {
                int sum=0;
                for(int q=0;q<ivec[j].first.size();q++)//循环找每个集合元素的子集
                {
                    for(int k=0;k<NFA.size();k++) //循坏NFA找到原来状态经过终结符a后
                    {
                        it=NFA[k].begin();
                        if(get<1>(*it)!='@'&&get<0>(*it)==ivec[j].first[q]&&get<1>(*it)==Te->first)
                        {
                            move1.insert(get<2>(*it));
                        }

                    }
                }
                //求闭包
                for(mo=move1.begin();mo!=move1.end();mo++)
                {
                    for(int d=0;d<NFA.size();d++)
                    {
                        it=NFA[d].begin();
                        if(get<0>(*it)==*mo&&get<1>(*it)=='@')
                        {
                            move1.insert(get<2>(*it));
                        }
                    }
                }
                sum=0;
                for(int e=0;e<ivec.size();e++)
                {
                    set<char>x(ivec[e].first.begin(), ivec[e].first.end());
                    if(move1==x)
                    {
                        dt=DFA[dcount].begin();
                        DFA[dcount].push_back(make_tuple(j,Te->first,e));
                        dt=DFA[dcount].begin();
                        dcount++;
                    }
                    else if(move1!=x)//判断move1和原有的子集是否有相同
                    {
                        sum++;
                    }
                    x.clear();
                }
                if(move1.size()!=0&&sum==ivec.size())//sum为ivec的大小时说明move1是原来子集没有的，可以插入
                {
                   /* for(mo=move1.begin();mo!=move1.end();mo++)
                    {
                        printf("move1=%c ",*mo);
                    }
                    printf("\n");
                    */
                    DFA[dcount].push_back(make_tuple(j,Te->first,add));
                    dt=DFA[dcount].begin();
                    dcount++;
                    ivec.push_back(make_pair(vector<char>(),false));

                    for(mo=move1.begin();mo!=move1.end();mo++)
                    {
                        ivec[add].first.push_back(*mo);

                    }
                    add++;
                }
                move1.clear();
            }
        }
    }
    //仅是终态的子集状态为0;
    for(int i=0;i<ivec.size();i++)
    {
        for(int j=0;j<ivec[i].first.size();j++)
        {
            if(ivec[i].first[j]=='#')
            {
                ivec[i].second=0;//0状态是终态
            }
            if(ivec[i].first[j]==Start)
            {
                ivec[i].second=2;//2状态是开始状态
                dfa_num=i;
            }
        }
    }
    for(int i=0;i<ivec.size();i++)
    {
         printf("状态<%d,%d>",i,ivec[i].second);
         printf("\n");
    }
    for(int i=0;i<ivec.size();i++)
    {
        printf("ivec[%d]=",i);
        for(int j=0;j<ivec[i].first.size();j++)
        {
            printf("%c ",ivec[i].first[j]);
        }
        printf("\n");
    }
}


void showNFA()
{
    vector<tuple<char,char,char> >::iterator it;
    for(int j=0;j<NFA.size();j++)//测试数据有没有传进map容器中
    {
        for(it=NFA[j].begin();it!=NFA[j].end();it++)
        {
            cout<<get<0>(*it)<<"->"<<get<1>(*it)<<get<2>(*it)<<endl;

        }
    }
}

void showDFA()
{
    vector<tuple<int,char,int> >::iterator dt;
    for(int j=0;j<dcount;j++)
    {
        for(dt=DFA[j].begin();dt!=DFA[j].end();dt++)
        {
            cout<<"DFA<"<<get<0>(*dt)<<","<<get<1>(*dt)<<","<<get<2>(*dt)<<">"<<endl;
        }
    }

}

bool isKey(string str)//判断是否是关键字
{
    if(Key.find(str)!=Key.end())
        return true;
    else
        return false;
}
bool isDel(char c)//判断是否是分界符
{
    if(Delimiter.find(c)!=Delimiter.end())
        return true;
    else
        return false;
}
bool isOpe(string str)//判断是否是运算符
{
    if(Operator.find(str)!=Operator.end())
        return true;
    else
        return false;
}

bool isIde(string str)//判断是否是标识符
{
    vector<tuple<int,char,int> >::iterator dt;
    int dfa_num1=dfa_num;
    int flag=0;
    for(int i=0;i<str.length();i++)
    {
        for(int j=0;j<DFA.size();j++)
        {
            dt=DFA[j].begin();
            if(get<0>(*dt)==dfa_num1&&get<1>(*dt)==str[i])//找到下一个状态
            {
                flag=1;
                if(ivec[get<2>(*dt)].second==0&&i==str.length()-1)//扫描完字符串，并且到达终态
                    return true;
                else
                {
                    dfa_num1=get<2>(*dt);
                    break;
                }
            }
        }
        if(flag==0)
            dfa_num1=dfa_num;
    }
    return false;
}
bool isConst(string str)//是否是常量
{
    int count=0;
    for(int i=0;i<str.length();i++)
    {
        if(str[i]>='0'&&str[i]<='9')
            count++;
    }
    if(count==str.length())
        return true;
    else
        return false;
}
void scan()//利用已有的DFA来扫描程序
{
    ifstream file;
    string filename="Lex_code.txt";
    file.open(filename.c_str(),ios::in);
    string input;
    while(getline(file,input))
    {
        int x=0,y=0;
        string str;
        for(int i=0;i<input.length();)
        {
            if(input[i]==' ')
            {
                x++;
                i=x;
            }
            else
            {
                str=input.substr(x,2);
                if(isOpe(str))
                {
                    Out.push_back(make_pair(str,1));
                    x+=2;
                    i=x;
                }
                else
                {
                    str=input.substr(x,1);
                    if(isOpe(str))
                    {
                        Out.push_back(make_pair(str,1));
                        x+=1;
                        i=x;
                    }
                    else
                    {
                        if(isDel(str[0]))
                        {
                            Out.push_back(make_pair(str,2));
                            x+=1;
                            i=x;
                        }
                        else
                        {
                            for(int j=0;j<input.length();j++)
                            {
                                if((input[i]>='0'&&input[i]<='9')||(input[i]>='a'&&input[i]<='z')) //如果开头是数字或字母就往下走，分离出一个字符串
                                {
                                    y++;
                                    i++;

                                }
                            }
                                //分离出一个个子串
                                str=input.substr(x,y-x);
                                if(isKey(str))
                                {
                                    Out.push_back(make_pair(str,0));
                                    x=y;
                                    i=x;
                                }
                                else
                                {
                                    if(isIde(str))
                                    {
                                        Out.push_back(make_pair(str,3));
                                        x=y;
                                        i=x;
                                    }
                                    else
                                    {
                                        if(isConst(str))
                                        {
                                            Out.push_back(make_pair(str,4));
                                            x=y;
                                            i=x;
                                        }
                                        else
                                        {
                                            printf("Error,Can not be Identified!");
                                        }
                                    }

                            }
                        }
                    }
                }
            }
            y=x;
        }
    }
    file.close();
}

int main()
{
    //0是关键字，1是运算符，2是分界符，3是标识符，4是常量
    ifstream fin("keys_table.txt",ios::in);
    string keys;
    while(getline(fin,keys))
    {
        Key[keys]=0;
    }
    fin.close();
    map<string,int>::iterator ke;
    for(ke=Key.begin();ke!=Key.end();ke++)
    {
        printf("Key<%s,%d>\n",ke->first.c_str(),ke->second);
    }
    Operator["="]=1;
    Operator["+"]=1;
    Operator["-"]=1;
    Operator["*"]=1;
    Operator["/"]=1;
    Operator["%"]=1;
    Operator["+="]=1;
    Operator["-="]=1;
    Operator["*="]=1;
    Operator[">"]=1;
    Operator["<"]=1;
    Operator[">="]=1;
    Operator["<="]=1;
    Operator["/="]=1;
    Operator["!="]=1;
    Delimiter['{']=2;
    Delimiter['}']=2;
    Delimiter[',']=2;
    Delimiter[':']=2;
    Delimiter[';']=2;
    Delimiter['(']=2;
    Delimiter[')']=2;
    Delimiter['\'']=2;
    Delimiter['\"']=2;
    Delimiter['\\']=2;
    for(ke=Operator.begin();ke!=Operator.end();ke++)
    {
        printf("Operator<%s,%d>\n",ke->first.c_str(),ke->second);
    }
    map<char,int>::iterator de;
    for(de=Delimiter.begin();de!=Delimiter.end();de++)
    {
        printf("Delimiter<%c,%d>\n",de->first,de->second);
    }
    bulid_NFA();
    bulid_Tem_and_NTem();
    NFA_TO_DFA();
    showNFA();
    showDFA();
    scan();
    vector<pair<string,int> >::iterator out;
    for(out=Out.begin();out!=Out.end();out++)
    {
        printf("%d        %s\n",get<1>(*out),get<0>(*out).c_str());
    }
    return 0;
}

