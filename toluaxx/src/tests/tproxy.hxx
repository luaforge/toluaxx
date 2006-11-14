#include<iostream>
#include<string>
#include<map>
using namespace std;
//tolua_begin

class OBJECT{
 public:
  OBJECT* parent;
  string name;
  int live;
  float mass;
  
  OBJECT():parent(NULL),live(0),mass(0.0f){}
  virtual ~OBJECT(){}
  
  string fullname(string n=""/** asnil**/);
  
  virtual operator string();
};

class GROUP: public OBJECT{
 protected:
  map<string,OBJECT*> pool;//tolua_noexport
 public:
  GROUP():OBJECT(){}
  virtual ~GROUP(){}
  string children();
  
  virtual void operator()(string&/**k="" asnil**/,OBJECT*&);
  /**tolua_getindex {**/
  virtual OBJECT* getobject(string);
  /**}**/
  /**tolua_setindex {**/
  virtual void setobject(string, OBJECT*);
  /**}**/
  virtual operator string();
};

class SCENE: public GROUP{
 public:
  SCENE():GROUP(){}
  virtual ~SCENE(){}
  
  virtual operator string();
};

//tolua_end
