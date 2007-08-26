#pragma once
#include<string>
using namespace std;

class OBJ{//tolua_export
protected:
  char*data;
public:
  //tolua_begin
  OBJ();
  virtual ~OBJ();
  operator string();
};
//tolua_end

#include<map>

class GRP: public OBJ{//tolua_export
protected:
  typedef std::map<string,OBJ*> POOL;
  typedef POOL::iterator ITER;
  POOL pool;
public:
  //tolua_begin
  GRP();
  virtual ~GRP();
  OBJ *& operator[](string);
  void operator()(string&n/*$="" asnil$*/,OBJ*&o/*$=NULL$*/);
  operator string();
  int operator~();
};

extern long tolua_alloc_count;
void SetAllocHook();
void CreateUserdata();

string ident(void*);
//tolua_end

