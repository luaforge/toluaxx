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

