#include<stdio.h>
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
set<string>Prod,Temp;//产生式
map<char,int>Teminal,NTeminal;//记录终结符和非终结符
map<char,set<char> >First,Follow;//First,Follow集
map<string,set<char> >Select;//Selec集
map<pair<char,char>,string>Table;//预测分析表
vector<char>ana_stack;
char start;//记录开始符号，因为map会排序
void toEmpty()
{
    set<string>::iterator pr;
    map<char,int>::iterator te;
    ifstream file;
    string filename="pha_gra.txt";
    file.open(filename.c_str(),ios::in);
    if(file.fail())
    {
        cout<<"此文件不存在。"<<endl;
        file.close();
    }
    string prod;
    int flag=-1;
    while(getline(file,prod))
    {
        if(flag==-1)//标记开始符号
        {
            start=prod[0];
            flag=1;
        }
        Prod.insert(prod);
        Temp.insert(prod);
        NTeminal[prod[0]]=2;
        //cout<<"prod="<<prod<<endl;
        ////cout<<"Pord="<<Prod[prod]<<endl;
    }
    file.close();
    //cout<<"count="<<count<<endl;
    //验证文法是否被传进容器中
    for(pr=Prod.begin();pr!=Prod.end();pr++)
    {
        cout<<"产生式为："<<*pr<<endl;
    }
    /*for(pr=Temp.begin();pr!=Temp.end();pr++)
      {
      cout<<"产生式为："<<*pr<<endl;
      }*/
    for(te=NTeminal.begin();te!=NTeminal.end();te++)
    {
        cout<<"非终结符有："<<te->first<<"状态为："<<te->second<<endl;
    }
    //提取出终结符
    for(pr=Prod.begin();pr!=Prod.end();pr++)
    {
        string str=*pr;
        for(int i=3;i<str.length();i++)
        {
            //cout<<pr->first[i]<<endl;
            if(str[i]>90||str[i]<65)
                Teminal[str[i]]=0;
        }
    }
    for(te=Teminal.begin();te!=Teminal.end();te++)
    {
        cout<<"终结符有:"<<te->first<<endl;
    }
    //找出可以直接推出@的非终结符
    for(pr=Prod.begin();pr!=Prod.end();pr++)
    {
        string str=*pr;
        char NT='0';
        if(str[3]=='@')
            NT=str[0];
        for(te=NTeminal.begin();te!=NTeminal.end();te++)
        {
            if(te->first==NT)
                te->second=1;
        }
    }
    for(te=NTeminal.begin();te!=NTeminal.end();te++)
    {
        cout<<"非终结符为："<<te->first<<"   状态为："<<te->second<<endl;
    }
    //找出产生式右边都是非终结符的产生式，若右边的非终结符都可以推出@,则对应的产生时左边的非终结也可以推出@
    for(pr=Prod.begin();pr!=Prod.end();pr++)
    {
        char NT='0';
        int sum=0;
        string str=*pr;
        sum=0;
        for(int i=3;i<str.length();i++)
        {
            if(str[i]>=65&&str[i]<=90)
            {
                sum++;
            }
        }
        if(sum==str.length()-3)//说明该产生式右边全是非终结符
        {
            int j=3;
            int sum2=0;
            sum2=0;
            NT=str[0];
            while(j<str.size())
            {
                for(te=NTeminal.begin();te!=NTeminal.end();te++)
                {
                    if(te->first==str[j]&&te->second==1)
                    {
                        sum2++;
                    }
                }
                j++;
            }
            if(sum2==sum)
            {
                for(te=NTeminal.begin();te!=NTeminal.end();te++)
                    if(te->first==NT)
                        te->second=1;
            }
        }
    }
}

void getFirst(char nt)
{
    //求单个非终结符的first集
    int tag=3,flag=0;
    for(string str:Prod)
    {
        if(str[0]==nt)
        {
            if(str[3]<'A'||str[3]>'Z')//产生式右部第一个字符是终结符，直接把该终结符加入到左部的First集
            {
                First[str[0]].insert(str[3]);
            }
            else
            {
                for(int j=3;j<str.length();j++)
                {
                    if(str[3]<'A'||str[3]>'Z')
                    {
                        First[str[0]].insert(str[3]) ;
                        break;
                    }
                    getFirst(str[j]);
                    //printf("当前的非终结符为:%c,下一个非终结符为：%c\n",str[0],str[j]);
                    if(First[str[0]].find('@')!=First[str[0]].end())
                    {
                        First[str[0]].insert(First[str[j]].begin(),First[str[j]].end());
                    }
                    else
                    {
                        if(First[str[j]].find('@')!=First[str[j]].end())
                        {
                            First[str[0]].insert(First[str[j]].begin(),First[str[j]].end());
                            First[str[0]].erase('@');
                        }
                        else
                        {
                            First[str[0]].insert(First[str[j]].begin(),First[str[j]].end());
                        }
                    }
                    if(First[str[j]].find('@')!=First[str[j]].end())
                    {
                        flag=1;
                    }
                    if(flag==0)
                    {
                       // printf("flag=0\n");
                        break;
                    }
                    else
                    {
                        tag+=flag;
                        flag=0;
                    }
                    if(tag==str.length())
                    {
                        First[str[0]].insert('@');
                    }
                }
            }
        }
    }

}

set<char> getStrFirst(string str)//求字符串X0X1X2...的First集
{
    set<char>str_first;
    if(str.length() == 0) {
        return {'@'};
    }

    int i=0;
    while((str[i]<'A'||str[i]>'Z')&&i<str.length())
    {
         str_first.insert(str[i]);
         i++;
    }
    if(str[0]>='A'&&str[0]<='Z')
    {
        if(First[str[0]].find('@')==First[str[0]].end())//x0不能推出空，则First[x0x1x2...]=First[x0]
        {
            str_first.insert(First[str[0]].begin(),First[str[0]].end());
        }
        else//x0可以推出空
        {
            int j=0;
            for(j=0;j<str.length()&&First[str[j]].find('@')!=First[str[j]].end();j++)
            {
                ;
            }
            if(j!=str.length())
            {
                int k=0;
                for(k=0;k<str.length()&&First[str[k]].find('@')!=First[str[k]].end();k++)
                {
                    First[str[k]].erase('@') ;
                    str_first.insert(First[str[k]].begin(),First[str[k]].end());
                }
                for(int l=0;l<k;l++)
                {
                    First[str[l]].insert('@');
                }
            }
            else//全部字符都可以推出空@
            {
                for(int m=0;m<str.length();m++)
                {
                    str_first.insert(First[str[m]].begin(),First[str[m]].end());
                }
                str_first.insert('@');
            }

        }
    }
    return str_first;
}

void showFirst()
{
    map<char,set<char> >::iterator f;
    set<char>::iterator se;
    for(auto f:First)
    {
        printf("FIRST[%c]=",f.first);
        for(auto se:f.second)
        {
            printf("%c  ",se);
        }
        printf("\n");
    }

}

void getFollow()
{
    //printf("start=%c\n",start);
    Follow[start].insert('#');
    //for(char s:Follow[start])
       // printf("start里有：%c",s);
    for(string str:Prod)//直到Follow集不再增大
    {
        int size=0;
        //puts("1");
        while(size!=Follow[str[0]].size())
        {
            // puts("2");
            size=Follow[str[0]].size();
            for(auto te:NTeminal)//获取每个非终结符的Follow集
            {
               // puts("3");
                for(string str1:Prod)//循环每个产生式，找到产生式右部存在当前非终结符te的产生式
                {
                   // puts("4");
                    for(int i=3;i<str1.length();i++)//循环每个产生式的右部
                    {
                       // puts("5");
                        if(str1[i]==te.first)//te在产生式右部非最后一个字符
                        {
                           // cout<<"6"<<endl;
                            //printf("str1=%s",str1);
                            //cout<<"str1="<<str1<<endl;
                            //printf("str1[i]=%c\n",str1[i]);
                            //cout<<string(str1.begin()+i+1,str1.end())<<endl;
                            set<char>x=getStrFirst(string(str1.begin() + i + 1,str1.end()));
                           /* for(auto xx:x)
                            {
                                printf("x=%c\n",xx);
                            {
                            */
                            //puts("7");
                            Follow[te.first].insert(x.begin(),x.end());
                            if(x.find('@')!=x.end())
                            {
                               // puts("8");
                                set<char>::iterator xx;
                            /*    for(xx=x.begin();xx!=x.end();xx++)
                                {
                                     printf("x=%c",*xx);
                                }
                                */
                                Follow[te.first].erase('@');
                                Follow[te.first].insert(Follow[str1[0]].begin(),Follow[str1[0]].end());
                            }
                            break;
                        }
                    }
                }
            }
         }
    }
}

void showFollow()
{
    for(auto a:Follow)
    {
        printf("Follow[%c]=",a.first);
        for(auto b:a.second)
        {
            printf("%c ",b);
        }
        printf("\n");
    }
}

void getSelect()
{
    for(string str : Prod)
    {
        if(str[0]>'Z'||str[0]<'A')//如果产生式右边是以终结符开始的
        {
            Select[str].insert(str[3]);
            break;
        }
        else
        {
            //cout<<string(str.begin()+3,str.end())<<endl;
            set<char>a=getStrFirst(string(str.begin()+3,str.end()));
            if(a.find('@')!=a.end())//说明右部可以推出空
            {
               // cout<<string(str.begin()+3,str.end())<<endl;
                Select[str].insert(a.begin(),a.end());
                if(Select[str].find('@')!=Select[str].end())
                {
                    Select[str].erase('@');
                }
                Select[str].insert(Follow[str[0]].begin(),Follow[str[0]].end());
            }
            else//产生式右部不能推出空
            {
               // cout<<string(str.begin()+3+1,str.end())<<endl;
                Select[str].insert(a.begin(),a.end());
            }
        }
    }

}


void showSelect()
{
    for(auto a:Select)
    {
        printf("Select[%s]",a.first.c_str());
        for(auto b:a.second)
        {
            printf("%c ",b);
        }
        printf("\n");
    }
}

void ana_table()
{
    for(auto a:Select)
    {
        for(auto b:a.second)
        {
            Table[make_pair(a.first[0],b)]=a.first;
        }
    }

}

void showTable()//显示分析表
{
    for(auto a:Table)
    {
        printf("<%c,%c>=%s\n",a.first.first,a.first.second,a.second.c_str());
    }
}

bool analyInputStr()
{
    ifstream file;
    string filename="input.txt";
    file.open(filename.c_str(),ios::in);
    if(file.fail())
    {
        cout<<"此文件不存在。"<<endl;
        file.close();
    }
    string input;
    getline(file,input);
    file.close();
    ana_stack.push_back('#');//把#和开始符号放进去
    ana_stack.push_back(start);
    for(int i=0;i<ana_stack.size();i++)
    {
        printf("stack:%c ",ana_stack[i]);
    }
        printf("\n");
    int index=0;//标识是否找到合适的产生式的下标
    for(int j=1;j<input.length();)//扫描输入串
    {
        printf("----------------------------------------\n");
        printf("j=%d\n",j);
        for(auto a:Table)
        {
           // printf("a.first.first=%c   ana_stack=%c\n",a.first.first,ana_stack.back());
            //printf("a.first.second=%c   input[i]=%c\n",a.first.second,input[j]);
           // printf("----------------------------------------\n");
            if(a.first.first==ana_stack.back()&&a.first.second==input[j])
            {
                index++;
                if((a.second[3]<'A'||a.second[3]>'Z')&&a.second[3]!='@'&&a.second.size()==4)//判断找到的产生式右部的第一个字符是不是终结符，如果是就匹配成功
                {
                    //产生式右部只有 一个终结符
                    printf("此时找到的产生式右部只有一个终结符并且不是@\n");
                    printf("此时的产生式为：%s\n",a.second.c_str());
                    ana_stack.pop_back();
                    ana_stack.push_back(a.second[3]);
                    printf("此时输入串为：");
                    for(int p=j;p<input.length();p++)
                    {
                        printf("%c",input[p]);
                    }
                    printf("\n");
                    printf("此时分析栈为：");
                    for(int n=0;n<ana_stack.size();n++)
                        printf("%c ",ana_stack[n]);
                    printf("\n");
                    break;
                }
                else
                {
                    if((a.second[3]<'A'||a.second[3]>'Z')&&(a.second[3]!='@')&&(a.second.size()>4))
                    {
                        //产生式右部是以终结符开头的一串字符串
                        printf("此时找到的终结符符的右部第一个字符是非终结符\n");
                        printf("此时的产生式为：%s\n",a.second.c_str());
                        ana_stack.pop_back();
                        for(int k=a.second.size()-1;k>2;k--)
                        {
                            ana_stack.push_back(a.second[k]);
                        }
                        printf("此时输入串为：");
                        for(int p=j;p<input.length();p++)
                        {
                            printf("%c",input[p]);
                        }
                        printf("\n");
                        printf("此时分析栈为：") ;
                            for(int m=0;m<ana_stack.size();m++)
                                printf("%c ",ana_stack[m]);
                        printf("\n");
                        break;
                    }
                    else
                    {
                        if(a.second[3]=='@')
                        {
                            ana_stack.pop_back();
                            printf("此时输入串为：");
                            for(int p=j;p<input.length();p++)
                            {
                                printf("%c",input[p]);
                            }
                            printf("\n");
                            printf("此时分析栈为：");
                            for(int m=0;m<ana_stack.size();m++)
                                printf("%c ",ana_stack[m]);
                            printf("\n");
                            break;
                        }
                        else//此时找到的产生式的右部第一个字符是非终结符
                        {
                            printf("此时找到的终结符符的右部第一个字符是非终结符\n");
                            printf("此时的产生式为：%s\n",a.second.c_str());
                            ana_stack.pop_back();
                            for(int z=a.second.size()-1;z>2;z--)
                            {
                                ana_stack.push_back(a.second[z]);
                            }
                            printf("此时输入串为：");
                            for(int p=j;p<input.length();p++)
                            {
                                printf("%c",input[p]);
                            }
                            printf("\n");
                            printf("此时分析栈为：");
                            for(int m=0;m<ana_stack.size();m++)
                                printf("%c ",ana_stack[m]);
                            printf("\n");
                            break;
                        }
                    }
                }
            }
        }
        printf("j=%d\n",j);
        printf("ana_stack.back()=%c  input[j]=%c\n",ana_stack.back(),input[j]);
        if(ana_stack.back()==input[j]&&j<input.length()&&ana_stack.size()>1)
        {
            ana_stack.pop_back();
            printf("匹配\n");
            printf("此时分析栈为：");
            for(int m=0;m<ana_stack.size();m++)
                printf("%c ",ana_stack[m]);
            printf("\n");
            j++;
        }

        if(j==input.length()-1)
        {
            if(ana_stack.back()=='#')
            {
                printf("YES\n");
                break;
            }
        }
        if(index==0)
        {
            printf("Error,no!\n");
            break;
        }
        index=0;
    }
}

int main()
{
    toEmpty();
    for(auto te: NTeminal)
    {
        getFirst(te.first);
    }
    showFirst();
    getFollow();
    showFollow();
 /*   set<char>x=getStrFirst(")");
    set<char>y=getStrFirst("*FW");
    set<char>z=getStrFirst("TQ");
    for(auto s:x)
    {
        printf("xx=%c\n",s);
    }
    for(auto s:y)
    {
        printf("y=%c\n",s);
    }
    for(auto s:z)
    {
        printf("z=%c\n",s);
    }*/
    getSelect();
    showSelect();
    ana_table();
    showTable();
    analyInputStr();
    return 0;
}

