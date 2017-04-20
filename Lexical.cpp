#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<tuple>

using namespace std;

map<char,vector<tuple<char,char,char>>>mapG;

int main(){
 ifstream fin("Lex_Gra.txt",ios::in);
 char line[1024]={0};
 char z,x,c,v,b;
 int count=0,i=0;
 while(fin.getline(line,sizeof(line)))
 {
   ++count;
   stringstream word(line);
   word>>z>>x>>c>>v>>b;
   cout<<"z="<<z<<" "<<"x="<<x<<" "<<"c="<<c<<" "<<"v="<<v<<" "<<"b="<<b<<endl;
   mapG[i].push_back(make_tuple(z,v,b));
   i++;
 }
 fin.clear();
 fin.close();
 vector<tuple<char,char,char>>::iterator it;
 for(int j=0;j<count;j++)
 {
 for(it=mapG[j].begin();it!=mapG[j].end();it++)
 {
  cout<<get<0>(*it)<<"->"<<get<1>(*it)<<get<2>(*it)<<endl;

 }
 }

 return 0;
}
