#include"tgcind.hxx"
#include<iostream>

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


GRP::GRP():OBJ(){
  
}

GRP::~GRP(){
  
}

OBJ *& GRP::operator[](string n){
  //LOGOUT(group operator[],ident(pool[n]));
  return pool[n];
}

void GRP::operator()(string&n/**="" asnil**/,OBJ*&o/**=NULL**/){
  ITER t;
  if(n==""){
    t=pool.begin();
    if(t!=pool.end()){ n=t->first; o=t->second; return; }
  }else{
    t=pool.find(n);
    if(t!=pool.end()){
      t++;
      if(t!=pool.end()){ n=t->first; o=t->second; return; }
    }
  }
  n=""; o=NULL;
}

GRP::operator string(){
  string ret=string("group: ")+ident(this);
  for(ITER i=pool.begin();i!=pool.end();i++){
    if(!i->second)continue;
    ret+="["; ret+=i->first; ret+="]=";
    ret+=ident(i->second); ret+="\n";
  }
  return ret;
}

int GRP::operator~(){
  int c=0;
  for(ITER i=pool.begin();i!=pool.end();i++){
    if(!i->second)continue;
    c++;
  }
  return c;
}

TST::TST(){}
TST::~TST(){}

OBJ* TST::get(string k){
  LOGOUT(TST::get,k);
  return NULL;
}

void TST::set(string k,OBJ* v){
  LOGOUT(TST::set,k<<v);
}


