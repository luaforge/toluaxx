#pragma once

#include<stdio.h>
#include<lua.h>
#include"toluaxx.h"

namespace Test {
  class Tst_Dummy{
  };
  
  class Tst_A{
    int number;
  public:
    static Tst_A* last;
    Tst_A () {last = this;}
    virtual char* a () { return "A"; }
    class Tst_AA{
    public:
      Tst_AA () {}
      ~Tst_AA () { }
      char* aa () { return "AA"; }
    };
    class Tst_BB : public Tst_AA{
    public:
      Tst_BB () {}
      ~Tst_BB () {}
      Tst_AA* Base () { return this; }
    };
    void set_number(int p_number) { number = p_number;};
    int get_number() {return number*2;};
    virtual ~Tst_A() {};
  };
  
  class Tst_B : public Tst_A{
  public:
    static Tst_B* last;
    Tst_B () {last = this;}
    virtual char* b () { return "B"; }
    
    static Tst_A* create() {return new Tst_B;};
    static void* create_void() {return new Tst_B;};
    
    virtual ~Tst_B() {};
  };
  
  class Tst_C : public Tst_B{
    int i;
  public:
    static Tst_C* last;
    Tst_C (int n) : i(n) {last = this;}
    virtual ~Tst_C () {
      lua_State*L=tolua_state();
      lua_pushfstring(L,"deleting C: %d",i);
      lua_pushboolean(L,true);
      tolua_setproxy(L,-1);
    }
    virtual char* c () { return "C"; }
  };
  
  inline Tst_A::Tst_AA* Tst_create_aa (){
    return new Tst_A::Tst_AA();
  }
  
  inline bool Tst_is_aa (Tst_A::Tst_AA* obj){
    return true;
  }
  
  class Tst_E {
    void* ptr;
  public:
    enum Pete {
      ONE,
      TWO,
    } pete;
    
    void get_pete(Pete p) {};
    
    template <class T>
    T get_t() {T a=0; return a;};
    
    Tst_E& operator+(const Tst_E& rvalue) {return *this;};
    
    void pars(int a=0, int b=0) {};
    void get_pointer(void* a) {};
    
    Tst_A a;
    
    void set_ptr(void* p_ptr){
      lua_State*L=tolua_state();
      lua_pushstring(L,"this");
      lua_pushfstring(L,"%p",this);
      tolua_setproxy(L,-1);
      lua_pushstring(L,"ptr");
      lua_pushfstring(L,"%p",p_ptr);
      tolua_setproxy(L,-1);
      ptr=p_ptr;
    };
    void* get_ptr(){return ptr;};
    
    Tst_E(int) {};
  };
  
  class Tst_Outside {
  public:
    Tst_Outside() {};
  };
  
}; // end of namespace

static void outside_func(Test::Tst_Outside* p_out, lua_State* ls){
  lua_State*L=tolua_state();
  lua_pushstring(L,"flag");
  if(p_out)lua_pushstring(L,"method");
  else lua_pushstring(L,"static");
  tolua_setproxy(L,-1);
  lua_pushstring(L,"luastate");
  lua_pushfstring(L,"%i",ls);
  tolua_setproxy(L,-1);
};

void init();
void dest();

//tolua_begin
class HiddenFields{
public:
  int pa;
protected:
  int pa1;
 private:
  int pa2;
 public:
  int pa3;
private:
protected:
 private:
  int pb1;
 protected:
  int pb2;
 protected:
 private:
public:
  int pb3;
 protected:
 private:
  int pc1;
private:
 protected:
  int pc2;
 protected:
public:
  int pc3;
 public:
  HiddenFields():pa(0),pa1(1),pa2(2),pa3(3),pb1(11),pb2(22),pb3(33),pc1(111),pc2(222),pc3(333){}
};
//tolua_end
