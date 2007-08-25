#include<string>
#include<map>
#include<iostream>
using namespace std;

class ELEM{//tolua_export
 protected:
  int i;
  string v;
  ELEM* e;
 public:
  //tolua_begin
  ELEM(){}
  ~ELEM(){}
  string operator~(){return v;}
  bool operator()(){return e!=NULL;}
  bool operator==(const ELEM& e){return(this==&e);}
  void value(int*_i/*$=0$*/,string*_v/*$=""$*/,ELEM*&_e/*$=NULL$*/){
    if(*_i!=0)i=*_i;
    if(*_v!="")v=*_v;
    if(_e!=NULL)e=_e;
    *_i=i;
    *_v=v;
    _e=e;
    cout<<"get value:"<<*_v<<endl;
  }
};
//tolua_end

class NUM_MAP{//tolua_export
 protected:
  map<int,ELEM*> pool;
 public:
  //tolua_begin
  NUM_MAP(){}
  NUM_MAP(int i){}
  ELEM * & operator[](int i){if(pool.find(i)==pool.end())pool[i]=NULL; return pool[i];}
  void operator()(int&i/*$=0 asnil$*/,ELEM*&e/*$=NULL$*/){
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
//tolua_end

class STR_MAP{//tolua_export
 protected:
  map<string,ELEM*> pool;
 public:
  //tolua_begin
  STR_MAP(){}
  ~STR_MAP(){}
  ELEM * & operator[](string i){if(pool.find(i)==pool.end())pool[i]=NULL; return pool[i];}
  void operator()(string&i/*$="" asnil$*/,ELEM*&e/*$=NULL$*/){
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
//tolua_end

class OBJ{//tolua_export
public:
  //tolua_begin
  OBJ();
  virtual ~OBJ();
  operator string();
};
//tolua_end

class TST: public OBJ{//tolua_export
protected:
  map<string,ELEM*> elem;
  map<string,OBJ*> obj;
  map<string,int> num;
  map<string,string> str;
public:
  //tolua_begin
  TST();
  ~TST();
  int test(){return 1;}
  /*$tolua_getindex {$*/
  ELEM* getelem(string);
  OBJ* getobj(string);
  int getnum(string)/*$=0 asnil$*/;
  string getstr(string)/*$="" asnil$*/;
  /*$}$*/
  /*$tolua_setindex {$*/
  void setelem(string,ELEM*);
  void setobj(string,OBJ*);
  void setnum(string,int n/*$=0 asnil$*/);
  void setstr(string,string s/**="" asnil**/);
  void delvar(string,void*);
  /*$}$*/
};
//tolua_end

class MULTI{//tolua_export
protected:
  map<string,int> pool_num;
  map<string,string> pool_str;
  map<string,bool> pool_bool;
public:
  int nonum;
  string nostr;
  bool nobool;
public:
  //tolua_begin
  MULTI(){}
  ~MULTI(){}
  bool is(string);
  bool isnum(string);
  bool isstr(string);
  bool isbool(string);
  /*$tolua_getindex {$*/
  void getnum(string,int* /*$num=self->nonum asnil$*/);
  void getstr(string,string* /*$str=self->nostr asnil$*/);
  void getbool(string,bool* /*$b=self->nobool asnil$*/);
  /*$}$*/
  /*$tolua_setindex {$*/
  void setnum(string,int n);
  void setstr(string,string s);
  void setbool(string,bool b);
  void delvar(string,void*n);
  /*$}$*/
};
//tolua_end

