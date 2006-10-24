#include<string>
#include<map>
#include<iostream>
using namespace std;

class ELEM{
 protected:
  int i;
  string v;
  ELEM* e;
 public:
  string operator~(){return v;}
  bool operator()(){return e!=NULL;}
  bool operator==(const ELEM& e){return(this==&e);}
  void value(int*_i,string*_v,ELEM*&_e){
    if(*_i!=0)i=*_i;
    if(*_v!="")v=*_v;
    if(_e!=NULL)e=_e;
    *_i=i;
    *_v=v;
    _e=e;
    cout<<"get value:"<<*_v<<endl;
  }
};

class NUM_MAP{
 protected:
  map<int,ELEM*> pool;
 public:
  NUM_MAP(){}
  NUM_MAP(int i){}
  ELEM * & operator[](int i){if(pool.find(i)==pool.end())pool[i]=NULL; return pool[i];}
  void operator()(int&i,ELEM*&e){
    map<int,ELEM*>::iterator t;
    if(i==0){
      t=pool.begin();
      if(t!=pool.end()){ i=t->first; e=t->second; return; }
    }else{
      t=pool.find(i);
      if(t!=pool.end()){
        t++;
	if(t!=pool.end()){ i=t->first; e=t->second; return; }
      }
    }
    i=0; e=NULL;
  } // next
  operator string(){
    string ret;
    for(map<int,ELEM*>::iterator i=pool.begin();i!=pool.end();i++){
      if(!i->second)continue;
      ret+="["; {char b[25]; sprintf(b,"%d",i->first); ret+=b;}
      ret+="]=";{char b[25]; sprintf(b,"0x%x",i->second);ret+=b;} ret+="\n";
    }
    return ret;
  }
  int operator~(){
    int c=0;
    for(map<int,ELEM*>::iterator i=pool.begin();i!=pool.end();i++){
      if(!i->second)continue;
      c++;
    }
    return c;
  }
};

class STR_MAP{
 protected:
  map<string,ELEM*> pool;
 public:
  ELEM * & operator[](string i){if(pool.find(i)==pool.end())pool[i]=NULL; return pool[i];}
  void operator()(string&i,ELEM*&e){
    map<string,ELEM*>::iterator t;
    if(i==""){
      t=pool.begin();
      if(t!=pool.end()){ i=t->first; e=t->second; return; }
    }else{
      t=pool.find(i);
      if(t!=pool.end()){
        t++;
	if(t!=pool.end()){ i=t->first; e=t->second; return; }
      }
    }
    i=""; e=NULL;
  } // next
  operator string(){
    string ret;
    for(map<string,ELEM*>::iterator i=pool.begin();i!=pool.end();i++){
      if(!i->second)continue;
      ret+="["; ret+=i->first; ret+="]=";
      {char b[25]; sprintf(b,"0x%x",i->second);ret+=b;} ret+="\n";
    }
    return ret;
  }
  int operator~(){
    int c=0;
    for(map<string,ELEM*>::iterator i=pool.begin();i!=pool.end();i++){
      if(!i->second)continue;
      c++;
    }
    return c;
  }
};
