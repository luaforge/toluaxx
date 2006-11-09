//tolua_begin
/**module A {**/
  extern int a;
/**  module B {**/
    extern int b;
/**    module C {**/
      extern int c;
/**    }
  }
}

module A {**/
  extern int d;
/**}**/
//tolua_end
