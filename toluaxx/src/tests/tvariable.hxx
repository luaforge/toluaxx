#include<string>
using namespace std;

typedef struct A A;
typedef struct B B;
typedef union U U;

struct A{
  int i;
  float f;
  double d;
  char* s;
  void* v;
  string n;
};

union U{
  int i;
  float f;
};

struct B{
  A a;
  B* b;
};

extern int i;
extern float f;
extern double d;
extern char* s;
extern void* v;
extern string n;
extern A a;
extern B* b;
extern U u;

extern int mi;
extern float mf;
extern double md;
extern char* ms;
extern void* mv;
extern string mn;
extern A ma;
extern B* mb;

extern B bb;
extern B bbb;

#ifndef NULL
#  define NULL 0x0000000
#endif

void init();
