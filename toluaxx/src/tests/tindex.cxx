#include"tindex.hxx"

#define LOGOUT(m,t) {std::cout<<"DEBUG TRACE: "<<#m<<" "<<t<<""<<std::endl;}

string ident(void*o){
  char b[32];
  sprintf(b,"%p",o);
  return b;
}

OBJ::OBJ(){
  LOGOUT(new object,ident(this));
}

OBJ::~OBJ(){
  LOGOUT(delete object,ident(this));
}

OBJ::operator string(){
  return string("object: ")+ident(this);
}

TST::TST(){}
TST::~TST(){}

OBJ* TST::getobj(string k){
  if(obj.find(k)!=obj.end())return obj[k];
  return NULL;
}

void TST::setobj(string k,OBJ* v){
  if(v)obj[k]=v;
  else if(obj.find(k)!=obj.end())obj.erase(k);
}

ELEM* TST::getelem(string k){
  if(elem.find(k)!=elem.end())return elem[k];
  return NULL;
}

void TST::setelem(string k,ELEM* v){
  if(v)elem[k]=v;
  else if(elem.find(k)!=elem.end())elem.erase(k);
}

int TST::getnum(string k){
  if(num.find(k)!=num.end())return num[k];
  return 0;
}

void TST::setnum(string k,int v){
  if(v)num[k]=v;
  else if(num.find(k)!=num.end())num.erase(k);
}

string TST::getstr(string k){
  if(str.find(k)!=str.end())return str[k];
  return "";
}

void TST::setstr(string k,string v){
  if(v!="")str[k]=v;
  else if(str.find(k)!=str.end())str.erase(k);
}

void TST::delvar(string k,void* o){
  if(o!=NULL)return;
  if(elem.find(k)!=elem.end())elem.erase(k);
  if(obj.find(k)!=obj.end())obj.erase(k);
  if(num.find(k)!=num.end())num.erase(k);
  if(str.find(k)!=str.end())str.erase(k);
}


void MULTI::getnum(string k,int* n/**=self->nonum asnil**/){
  if(pool_num.find(k)!=pool_num.end()){
    *n=pool_num[k];
    nonum=*n+1;
    if(*n==nonum)nonum=*n+1;
  }else{
    *n=nonum;
  }
}

void MULTI::getstr(string k,string* s/**=self->nostr asnil**/){
  if(pool_str.find(k)!=pool_str.end()){
    *s=pool_str[k];
    if(*s==nostr)nostr=*s+" ";
  }else{
    *s=nostr;
  }
}

void MULTI::getbool(string k,bool* b/**=self->nostr asnil**/){
  if(pool_bool.find(k)!=pool_bool.end()){
    *b=pool_bool[k];
    if(*b==nobool)nobool=!*b;
  }else{
    *b=nobool;
  }
}

void MULTI::setnum(string k,int n){
//  cout<<"setnum"<<endl;
  delvar(k,NULL);
  pool_num[k]=n;
}
void MULTI::setstr(string k,string s){
//  cout<<"setstr"<<endl;
  delvar(k,NULL);
  pool_str[k]=s;
}
void MULTI::setbool(string k,bool b){
//  cout<<"setbool"<<endl;
  delvar(k,NULL);
  pool_bool[k]=b;
}
void MULTI::delvar(string k,void* n){
  if(n!=NULL)return;
  if(pool_str.find(k)!=pool_str.end())pool_str.erase(k);
  if(pool_num.find(k)!=pool_num.end())pool_num.erase(k);
  if(pool_bool.find(k)!=pool_bool.end())pool_bool.erase(k);
}   

bool MULTI::is(string k){
  return isnum(k)||isstr(k)||isbool(k);
}
bool MULTI::isnum(string k){
  return pool_num.find(k)!=pool_num.end();
}
bool MULTI::isstr(string k){
  return pool_str.find(k)!=pool_str.end();
}
bool MULTI::isbool(string k){
  return pool_bool.find(k)!=pool_bool.end();
}

