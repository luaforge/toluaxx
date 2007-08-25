#include "tvariable.hxx"

int i = 1;
float f = 2.0f;
double d = 3.0;
char* s = "Hello world";
void* v = (void*)1;
string n = "Hi there";

A a = {11,12.0f,13.0,"Hello world from class",(void*)1,"Hi there from class"};
B* b;
U u;

int mi = 21;
float mf = 22.0f;
double md = 23.0;
char* ms = "Hello world in module";
void* mv = NULL;
string mn = "Hi there in module";
A ma={31,32.0f,33.0,"Hello world from class in module",
      NULL,"Hi there from class in module"};
B* mb;

B bb;
B bbb;

void init(){
  bb.a=a;   bb.b=NULL;
  bbb.a=ma; bbb.b=&bb;
  b=&bb;
  mb=&bbb;
}
