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
  
  OBJECT():parent(NULL),name("noname"),live(0),mass(0.0f){}
  virtual ~OBJECT(){}
  
  virtual operator string();
};

class SCENE: public OBJECT{
 protected:
  map<string,OBJECT*> pool;//tolua_noexport
 public:
  SCENE():OBJECT(){}
  ~SCENE(){}
  /**tolua_getindex {**/
  OBJECT* getobject(string);
  /**}**/
  /**tolua_setindex {**/
  void setobject(string, OBJECT*);
  /**}**/
  virtual operator string();
};

//tolua_end
