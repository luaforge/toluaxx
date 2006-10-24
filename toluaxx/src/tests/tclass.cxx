#include "tclass.hxx"

//Test::Tst_A* Test::Tst_A::last;
//Test::Tst_B* Test::Tst_B::last;
//Test::Tst_C* Test::Tst_C::last;

Test::Tst_B* b;

void init(){
  b = new Test::Tst_B;         // instance used in Lua code
}

void dest(){
  delete b;
}
