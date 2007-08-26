#include"tgcind.hxx"
#include"toluaxx.h"

#define LOGOUT(m,t) {						\
    lua_State*L=tolua_state();					\
    lua_pushstring(L,#m);					\
    lua_pushstring(L,t.data());					\
    tolua_setproxy(L,-1);					\
  }

string ident(void*o){
  char b[32];
  sprintf(b,"%p",o);
  return b;
}

OBJ::OBJ(){
  //LOGOUT(new OBJ,ident(this));
  data=new char[1048576];
}

OBJ::~OBJ(){
  //LOGOUT(delete OBJ,ident(this));
  delete[]data;
}

OBJ::operator string(){
  return string("object: ")+ident(this);
}


GRP::GRP():OBJ(){
  //LOGOUT(new GRP,ident(this));
}

GRP::~GRP(){
  //LOGOUT(delete GRP,ident(this));
}

OBJ *& GRP::operator[](string n){
  //LOGOUT(group operator[],ident(pool[n]));
  return pool[n];
}

void GRP::operator()(string&n/*$="" asnil$*/,OBJ*&o/*$=NULL$*/){
  ITER t;
  if(n==""){
    t=pool.begin();
    if(t!=pool.end()){ n=t->first; o=t->second; return; }
  }else{
    t=pool.find(n);
    if(t!=pool.end()){
      t++;
      if(t!=pool.end()){ n=t->first; o=t->second; return; }
    }
  }
  n=""; o=NULL;
}

GRP::operator string(){
  string ret=string("group: ")+ident(this);
  for(ITER i=pool.begin();i!=pool.end();i++){
    if(!i->second)continue;
    ret+="["; ret+=i->first; ret+="]=";
    ret+=ident(i->second); ret+="\n";
  }
  return ret;
}

int GRP::operator~(){
  int c=0;
  for(ITER i=pool.begin();i!=pool.end();i++){
    if(!i->second)continue;
    c++;
  }
  return c;
}

long tolua_alloc_count;

static void* tolua_alloc(void *ud, void *ptr, size_t osize, size_t nsize) {
  long&count=*(long*)ud;
  (void)osize;  /* not used */
  /* ANSI requires that
     realloc(NULL, size) == malloc(size)
     realloc(ptr, 0) == free(ptr)
  */
  count+=nsize-osize;
  return realloc(ptr,nsize);
}

void SetAllocHook(){
  lua_State*L=tolua_state();
  tolua_alloc_count=0;
  lua_setallocf(L,tolua_alloc,&tolua_alloc_count);
}

void CreateUserdata(){
  lua_State*L=tolua_state();
  lua_newuserdata(L,4);
}
