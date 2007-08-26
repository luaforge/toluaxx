/*
namespace A {
  enum {FIRST=1};
  extern int a;
  namespace B {
    extern int b;
    namespace C {
      extern int c;
    }
  }
}
*/

/*
no support for recursive namespace
*/
//tolua_begin
namespace A {
  enum {FIRST=1};
  extern int a;
}

namespace B {
  extern int b;
  extern int d[4];
}
  
namespace C {
  extern int c;
}

namespace D {
  class D{
  public:
    int d;
    D(){}
    ~D(){}
    int getd()const{return d;}
    void setd(int d){D::d=d;}
  };
}
//tolua_end
