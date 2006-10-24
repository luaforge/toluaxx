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

namespace A {
  enum {FIRST=1};
  extern int a;  
}

namespace B {
  extern int b;
}
  
namespace C {
  extern int c;
}
