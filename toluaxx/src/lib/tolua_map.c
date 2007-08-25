/* tolua: functions to map features
** Support code for Lua bindings.
** Written by Waldemar Celes
** TeCGraf/PUC-Rio
** Apr 2003
** $Id: tolua_map.c,v 1.11 2007-08-25 11:07:04 phoenix11 Exp $
*/

/* This code is free software; you can redistribute it and/or modify it.
** The software provided hereunder is on an "as is" basis, and
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications.
*/

#include "toluaxx.h"
#include "tolua_event.h"
#include "lauxlib.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


static lua_State* __tolua_current_state_=NULL;

/* Call method to self object from C
 */
/*
  TOLUA_API int tolua_callmethod(lua_State* L,void* self,const char* type,const char* func,int nargs,int nresults){
  tolua_pushusertype(L,self,type);
  lua_pushstring(L,func);
  lua_gettable(L,-2);
  if(!lua_isfunction(L,-1)){lua_pop(L,1);return 0;} * method is nil *
  lua_insert(L,-2-nargs);
  lua_insert(L,-1-nargs);
  lua_call(L,nargs+1,nresults);
  return 1;
  }*/

TOLUA_API lua_State* tolua_state(){ /* Get current state  */
  return __tolua_current_state_;
}
TOLUA_API void tolua_setstate(lua_State* L){ /* Set current state */
  __tolua_current_state_=L;
}

/* Create metatable
 * Create and register new metatable
 */
static int tolua_newmetatable (lua_State* L, char* name){
  int r = luaL_newmetatable(L,name);
  
#ifdef LUA_VERSION_NUM /* only lua 5.1 */
  if(r){
    lua_pushvalue(L, -1);
    lua_pushstring(L, name);
    lua_settable(L, LUA_REGISTRYINDEX); /* reg[mt] = type_name */
  };
#endif
  
  if(r)tolua_classevents(L); /* set meta events */
  lua_pop(L,1);
  return r;
}
/* Map super classes
 * It sets 'name' as being also a 'base', mapping all super classes of 'base' in 'name'
 */
static void mapsuper (lua_State* L, const char* name, const char* base) {
  /* push registry.super */
  lua_pushstring(L,"tolua_super");
  lua_rawget(L,LUA_REGISTRYINDEX);    /* stack: super */
  luaL_getmetatable(L,name);          /* stack: super mt */
  lua_rawget(L,-2);                   /* stack: super table */
  if (lua_isnil(L,-1)) {
    /* create table */
    lua_pop(L,1);
    lua_newtable(L);                    /* stack: super table */
    luaL_getmetatable(L,name);          /* stack: super table mt */
    lua_pushvalue(L,-2);                /* stack: super table mt table */
    lua_rawset(L,-4);                   /* stack: super table */
  }
  
  /* set base as super class */
  lua_pushstring(L,base);
  lua_pushboolean(L,1);
  lua_rawset(L,-3);                    /* stack: super table */
  
  /* set all super class of base as super class of name */
  luaL_getmetatable(L,base);          /* stack: super table base_mt */
  lua_rawget(L,-3);                   /* stack: super table base_table */
  if (lua_istable(L,-1)) {
    /* traverse base table */
    lua_pushnil(L);  /* first key */
    while (lua_next(L,-2) != 0) {
      /* stack: ... base_table key value */
      lua_pushvalue(L,-2);    /* stack: ... base_table key value key */
      lua_insert(L,-2);       /* stack: ... base_table key key value */
      lua_rawset(L,-5);       /* stack: ... base_table key */
    }
  }
  lua_pop(L,3);                       /* stack: <empty> */
}
/* creates a 'tolua_ubox' table for base clases, and
// expects the metatable and base metatable on the stack */
static void set_ubox(lua_State* L) {
  /* mt basemt */
  if (!lua_isnil(L, -1)) {
    lua_pushstring(L, "tolua_ubox");
    lua_rawget(L,-2);
  } else {
    lua_pushnil(L);
  }
  /* mt basemt base_ubox */
  if (!lua_isnil(L,-1)) {
    lua_pushstring(L, "tolua_ubox");
    lua_insert(L, -2);
    /* mt basemt key ubox */
    lua_rawset(L,-4);
    /* (mt with ubox) basemt */
  } else {
    /* mt basemt nil */
    lua_pop(L, 1);
    lua_pushstring(L,"tolua_ubox"); lua_newtable(L);
    /* make weak value metatable for ubox table to allow userdata to be
       garbage-collected */
    lua_newtable(L); lua_pushliteral(L, "__mode"); lua_pushliteral(L, "v"); lua_rawset(L, -3);               /* stack: string ubox mt */
    lua_setmetatable(L, -2);  /* stack:mt basemt string ubox */
    lua_rawset(L,-4);
  }
}
/* Map inheritance
 * It sets 'name' as derived from 'base' by setting 'base' as metatable of 'name'
 */
static void mapinheritance (lua_State* L, const char* name, const char* base){
  /* set metatable inheritance */
  luaL_getmetatable(L,name);
  
  if (base && *base) luaL_getmetatable(L,base); else {
    
    if (lua_getmetatable(L, -1)) { /* already has a mt, we don't overwrite it */
      lua_pop(L, 2);
      return;
    }
    luaL_getmetatable(L,"tolua_commonclass");
  }
  
  set_ubox(L);
  
  lua_setmetatable(L,-2);
  lua_pop(L,1);
}
/* Object type
 */
static int tolua_bnd_type (lua_State* L){
  tolua_typename(L,lua_gettop(L));
  return 1;
}
/* Take ownership
 */
static int tolua_bnd_takeownership (lua_State* L){
  int success = 0;
  if (lua_isuserdata(L,1)) {
    if (lua_getmetatable(L,1)) {       /* if metatable? */
      lua_pop(L,1);             /* clear metatable off stack */
      /* force garbage collection to avoid C to reuse a to-be-collected address */
#ifdef LUA_VERSION_NUM
      lua_gc(L, LUA_GCCOLLECT, 0);
#else
      lua_setgcthreshold(L,0);
#endif
      success = tolua_register_gc(L,1);
    }
  }
  lua_pushboolean(L,success!=0);
  return 1;
}
/* Release ownership
 */
static int tolua_bnd_releaseownership (lua_State* L){
  int done = 0;
  if (lua_isuserdata(L,1)) {
    void* u = *((void**)lua_touserdata(L,1));
    /* force garbage collection to avoid releasing a to-be-collected address */
#ifdef LUA_VERSION_NUM
    lua_gc(L, LUA_GCCOLLECT, 0);
#else
    lua_setgcthreshold(L,0);
#endif
    lua_pushstring(L,"tolua_gc");
    lua_rawget(L,LUA_REGISTRYINDEX);
    lua_pushlightuserdata(L,u);
    lua_rawget(L,-2);
    lua_getmetatable(L,1);
    if (lua_rawequal(L,-1,-2)) {  /* check that we are releasing the correct type */
      lua_pushlightuserdata(L,u);
      lua_pushnil(L);
      lua_rawset(L,-5);
      done = 1;
    }
  }
  lua_pushboolean(L,done!=0);
  return 1;
}
/* Type casting
 */
static int tolua_bnd_cast (lua_State* L){
  
  /* // old code
     void* v = tolua_tousertype(L,1,NULL);
     const char* s = tolua_tostring(L,2,NULL);
     if (v && s)
     tolua_pushusertype(L,v,s);
     else
     lua_pushnil(L);
     return 1;
  */
  
  void* v;
  const char* s;
  if (lua_islightuserdata(L, 1)) {
    v = tolua_touserdata(L, 1, NULL);
  } else {
    v = tolua_tousertype(L, 1, 0);
  }
  
  s = tolua_tostring(L,2,NULL);
  if (v && s) tolua_pushusertype(L,v,s);
  else lua_pushnil(L);
  return 1;
}
/* Inheritance
 */
static int tolua_bnd_inherit (lua_State* L) {

  /* stack: lua object, c object */
  lua_pushstring(L, ".c_instance");
  lua_pushvalue(L, -2);
  lua_rawset(L, -4);
  /* l_obj[".c_instance"] = c_obj */

  return 0;
}

#ifdef LUA_VERSION_NUM /* lua 5.1 */
static int tolua_bnd_setpeer(lua_State* L) {
  /* stack: userdata, table */
  if (!lua_isuserdata(L,-2)) {
    lua_pushstring(L,"Invalid argument #1 to setpeer: userdata expected.");
    lua_error(L);
  }
  if (lua_isnil(L,-1)) {
    lua_pop(L,1);
    lua_pushvalue(L,TOLUA_NOPEER);
  }
  lua_setfenv(L,-2);
  return 0;
}
static int tolua_bnd_getpeer(lua_State* L) {
  /* stack: userdata */
  lua_getfenv(L,-1);
  if (lua_rawequal(L, -1, TOLUA_NOPEER)) {
    lua_pop(L, 1);
    lua_pushnil(L);
  }
  return 1;
}
#endif

#ifndef DISABLE_TEST_FRAMEWORK /* toluaxx test framework */
typedef struct tolua_Test{
  char*name;
  char*description;
  char*author;
  int count;
  int passed;
  int progress;
} tolua_Test;
typedef struct tolua_TestPass{
  /*
  lua_Object received_value;
  lua_Object expected_value;
  char*received_type;
  char*expected_type;
  */
  char*error;
  char*description;
  int state;
} tolua_TestPass;
#define TOLUA_SET_CSTRING(s,n){						\
    if(lua_isstring(L,n)){						\
      s=malloc(lua_objlen(L,n)+1);					\
      strcpy(s,lua_tostring(L,n));					\
    }else{ s=NULL; }							\
  }
static int tolua_bnd_test_new(lua_State* L){ /* create a new test framework */
  /* arg: test name, test description, test author */
  /* stack: usertable string|novalue string|novalue string|novalue */
  tolua_Error tolua_err;
  if(!tolua_isusertable(L,1,"tolua::test",0,&tolua_err)&&
     !tolua_isnoobj(L,5,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*t=malloc(sizeof(tolua_Test));
    TOLUA_SET_CSTRING(t->name,2);
    TOLUA_SET_CSTRING(t->description,3);
    TOLUA_SET_CSTRING(t->author,4);
    t->count=0;
    t->passed=0;
    t->progress=100;
    tolua_pushusertype(L,(void*)t,"tolua::test");
    /* stack: usertable string|novalue string|novalue string|novalue obj */
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'tolua::test::new'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_del(lua_State* L){
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'test_del'",NULL);
    else{
      if(self->name)free(self->name);
      if(self->description)free(self->description);
      if(self->author)free(self->author);
      free(self);
    }
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'test_del'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_name(lua_State* L){ /* test::get_name */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_name'",NULL);
    else tolua_pushstring(L,self->name);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_name'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_description(lua_State* L){ /* test::get_description */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_description'",NULL);
    else tolua_pushstring(L,self->description);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_description'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_author(lua_State* L){ /* test::get_author */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_author'",NULL);
    else tolua_pushstring(L,self->author);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_author'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_count(lua_State* L){ /* test::get_count */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_count'",NULL);
    else tolua_pushnumber(L,self->count);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_count'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_passed(lua_State* L){ /* test::get_passed */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_passed'",NULL);
    else tolua_pushnumber(L,self->passed);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_passed'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_state(lua_State* L){ /* test::get_state */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_state'",NULL);
    else tolua_pushboolean(L,self->count==self->passed);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_state'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_progress(lua_State* L){ /* test::get_progress */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_progress'",NULL);
    else tolua_pushnumber(L,self->progress);
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_progress'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_result(lua_State* L){ /* test::get_result */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_result'",NULL);
    else{
    /*  lua_pushfstring(L,"%s :: %s @ %s - [%d/%d] %d%% passed..",
		      self->name,self->description,self->author,
    		      self->passed,self->count,self->progress);*/
	lua_pushfstring(L,"%s :: %s - [%d/%d] %d%% passed..",
		      self->name,self->description,
    		      self->passed,self->count,self->progress);
    }
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_result'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_errors(lua_State* L){ /* test::get_errors */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))
    goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_error'",NULL);
    else{
      lua_pushstring(L,"");                      /* stack: {prev} */
      lua_pushvalue(L,1);                        /* stack: {prev} self */
      if(self->passed==self->count){
      }else{ int i;
	for(i=0;i<self->count;i++){
	  lua_pushnumber(L,i+1);                 /* stack: {prev} self index */
	  lua_gettable(L,-2);                    /* stack: {prev} self pass */
	  lua_pushstring(L,"state");             /* stack: {prev} self pass key */
	  lua_gettable(L,-2);                    /* stack: {prev} self pass st */
	  if(!lua_toboolean(L,-1)){              /* check if error state */
	    lua_pop(L,1);                        /* stack: {prev} self pass */
	    lua_pushfstring(L,"  failed [%d] //",i+1); /* stack: {prev} self pass msg1 */
	    lua_pushstring(L,"description");     /* stack: {prev} self pass msg1 key   */
	    lua_gettable(L,-3);                  /* stack: {prev} self pass msg1 desc  */
	    lua_pushstring(L,"// ");             /* stack: {prev} self pass msg1 desc msg2 */
	    lua_pushstring(L,"message");         /* stack: {prev} self pass msg1 desc msg2 key */
	    lua_gettable(L,-5);                  /* stack: {prev} self pass msg1 desc msg2 msg */
	    lua_pushstring(L,"..");              /* stack: {prev} self pass msg1 desc msg2 msg msg3 */
	    lua_concat(L,5);                     /* stack: {prev} self pass msg */
	    lua_remove(L,-2);                    /* stack: {prev} self msg */
	  }else{
	    lua_pop(L,1);                     /* stack: {prev} self pass */
	    lua_pop(L,1);                     /* stack: {prev} self */
	  }
	  if(lua_isstring(L,-1)){     /* stack: {prev} self msg */
	    lua_pushvalue(L,-3);      /* stack: {prev} self msg prev */
	    lua_insert(L,-2);         /* stack: {prev} self prev msg */
	    lua_pushstring(L,lua_objlen(L,-2)?"\n":""); /* stack: {prev} self prev msg \n */
	    lua_insert(L,-2);         /* stack: {prev} self prev \n msg */
	    lua_concat(L,3);          /* stack: {prev} self msg */
	    lua_remove(L,-3);         /* stack: self msg */
	    lua_insert(L,-2);         /* stack: msg self */
	                              /* stack: {prev} self */
	  }
	}
      }
      /*lua_pushvalue(L,-2);          /* stack: {prev} self {prev} */
      /*lua_remove(L,-3);             /* stack: self {prev} */
      lua_remove(L,-1);               /* stack: {prev} */
    }
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_error'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_get_report(lua_State* L){ /* test::get_report */
  tolua_Error tolua_err;
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err)) goto tolua_lerror;
  else{
    tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
    if(!self)tolua_error(L,"invalid 'self' in function 'get_report'",NULL);
    else{
      tolua_bnd_test_get_errors(L);                            /* stack: self <str> errmsg */
      lua_pushfstring(L,"%s :: %s - [%d/%d] %d%% passed..",
		      self->name,self->description,
		      self->passed,self->count,self->progress);/* stack: self <str> errmsg resmsg */
      lua_insert(L,-2);                                        /* stack: self <str> resmsg errmsg */
      lua_pushstring(L,lua_objlen(L,-1)?"\n":"");              /* stack: self <str> resmsg errmsg \n */
      lua_insert(L,-2);                                        /* stack: self <str> resmsg \n errmsg */
      lua_concat(L,3);                                         /* stack: self <str> msg */
    }
  }
  return 1;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'get_report'.",&tolua_err);
  return 0;
}
/*typedef enum tolua_test_mode{*/
  /* x x x x          */
  /* | | | |          */
  /* | | | V          */
  /* | | | type equal */
  /* | | V            */
  /* | | value equal  */
  /* | V              */
  /* | not strictly   */
  /* V  */
  /*    */
  /**/
  /*
  type_not_equal=0x0,tn=0x0,
  type_equal    =0x7,te=0x7,
  not_equal     =0x5,ne=0x5,
  equal         =0x2,eq=0x2,
  lesser_than   =0x1,lt=0x1,
  lesser_equal  =0x3,le=0x3,
  greater_than  =0x4,gt=0x4,
  greater_equal =0x6,ge=0x6,
}tolua_test_mode;
  */
/*
  lt + eq = le
  gt + eq = ge
  lt + gt = ne
  le + ge = eq
  eq = ~ne
  lt = ~ge
  le = ~gt
  
  lt + ge = 0
  
  tn = 000
  te = 111
  ne = 101
  eq = 010
  lt = 001
  le = 011
  gt = 100
  ge = 110
*/
static int tolua_bnd_test_op_tn(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){     /* rv ev */
    /* give types */
    tolua_typename(L,1);    /* rv ev rvt */
    tolua_typename(L,2);    /* rv ev rvt evt */
    if(!lua_equal(L,3,4)){  /* check if rvt != evt */
      lua_pushboolean(L,1); /* rv ev rvt evt true */
      return 1;             /* true */
    }
    lua_pushboolean(L,0);   /* rv ev rvt evt false */
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,3);
    lua_pushstring(L,"`, some as type of expected value");
    lua_concat(L,3);
                            /* rv ev rvt evt false msg */
    return 2;               /* false msg */
  }
  tolua_error(L,"#ferror in function 'tn'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_te(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_tn(L); /* rv ev rvt evt stn *msg* */
    if(!lua_toboolean(L,5)){ /* check if rvt == evt */
      lua_remove(L,-1);      /* rv ev rvt evt false */
      lua_remove(L,-1);      /* rv ev rvt evt       */
      lua_pushboolean(L,1);  /* rv ev rvt evt true  */
      return 1;
    }                        /* rv ev rvt evt true  */
    lua_remove(L,-1);        /* rv ev rvt evt       */
    lua_pushboolean(L,0);    /* rv ev rvt evt false */
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,3);
    lua_pushstring(L,"`, but type of expected value is `");
    lua_pushvalue(L,4);
    lua_pushstring(L,"`");
    lua_concat(L,5);
                             /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'te'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_ti(lua_State* L){ /* received value, expected type */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv evt */
    tolua_typename(L,1);     /* rv evt rvt */
    if(lua_equal(L,2,3)){    /* check if rvt == evt */
      lua_pushboolean(L,1);  /* rv evt rvt true  */
      return 1;
    }                        /* rv evt rvt */
    lua_pushboolean(L,0);    /* rv evt rvt false */
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,3);
    lua_pushstring(L,"`, but expected type is `");
    lua_pushvalue(L,2);
    lua_pushstring(L,"`");
    lua_concat(L,5);
                             /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'ti'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_ni(lua_State* L){ /* received value, not expected type */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv evt */
    tolua_typename(L,1);     /* rv evt rvt */
    if(!lua_equal(L,2,3)){   /* check if rvt != evt */
      lua_pushboolean(L,1);  /* rv evt rvt true  */
      return 1;
    }                        /* rv evt rvt */
    lua_pushboolean(L,0);    /* rv evt rvt false */
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,3);
    lua_pushstring(L,"`, but expected type is not `");
    lua_pushvalue(L,2);
    lua_pushstring(L,"`");
    lua_concat(L,5);
                             /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'ni'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_ne(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(!lua_equal(L,1,2)){ /* check if rv != ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not not equal `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'ne'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_eq(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(lua_equal(L,1,2)){  /* check if rv == ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not equal `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'eq'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_se(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  lua_pushstring(L,"tostring");     /* rv ev key */
  lua_gettable(L,LUA_GLOBALSINDEX); /* rv ev *func* */
  if(!lua_isfunction(L,-1)){        /* rv ev ?func? */
    tolua_error(L,"#ferror in function 'se'. function `tostring` undefined in globals",&tolua_err);
    return 0;
  }
  if(lua_gettop(L)==3){        /* rv ev func */
    lua_pushvalue(L,3);        /* rv ev func func */
    lua_pushvalue(L,1);        /* rv ev func func rv */
    lua_pcall(L,1,1,0);        /* rv ev func srv */
    lua_insert(L,-2);          /* rv ev srv func */
    lua_pushvalue(L,2);        /* rv ev srv func ev */
    lua_pcall(L,1,1,0);        /* rv ev srv sev */
    if(lua_equal(L,-2,-1)){    /* check if srv == sev */
      lua_pushboolean(L,1);    /* rv ev srv sev true */
      return 1;                /* true */
    }
    lua_pushboolean(L,0);      /* rv ev srv sev false */
    lua_pushstring(L,"`");
    lua_pushvalue(L,3);
    lua_pushstring(L,"` (received value) not string equal `");
    lua_pushvalue(L,4);
    lua_pushstring(L,"` (expected value)");
    lua_concat(L,5);       /* rv ev srv sev false msg */
    return 2;              /* false msg */
  }
  tolua_error(L,"#ferror in function 'se'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_sn(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  lua_pushstring(L,"tostring");     /* rv ev key */
  lua_gettable(L,LUA_GLOBALSINDEX); /* rv ev *func* */
  if(!lua_isfunction(L,-1)){        /* rv ev ?func? */
    tolua_error(L,"#ferror in function 'sn'. function `tostring` undefined in globals",&tolua_err);
    return 0;
  }
  if(lua_gettop(L)==3){        /* rv ev func */
    lua_pushvalue(L,3);        /* rv ev func func */
    lua_pushvalue(L,1);        /* rv ev func func rv */
    lua_pcall(L,1,1,0);        /* rv ev func srv */
    lua_insert(L,-2);          /* rv ev srv func */
    lua_pushvalue(L,2);        /* rv ev srv func ev */
    lua_pcall(L,1,1,0);        /* rv ev srv sev */
    if(!lua_equal(L,-2,-1)){   /* check if srv == sev */
      lua_pushboolean(L,1);    /* rv ev srv sev true */
      return 1;                /* true */
    }
    lua_pushboolean(L,0);      /* rv ev srv sev false */
    lua_pushstring(L,"`");
    lua_pushvalue(L,3);
    lua_pushstring(L,"` (received value) not not string equal `");
    lua_pushvalue(L,4);
    lua_pushstring(L,"` (expected value)");
    lua_concat(L,5);       /* rv ev srv sev false msg */
    return 2;              /* false msg */
  }
  tolua_error(L,"#ferror in function 'sn'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_lt(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(lua_lessthan(L,1,2)){  /* check if rv < ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not lesser than `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'lt'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_le(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(lua_lessthan(L,1,2)||lua_equal(L,1,2)){  /* check if rv < ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not lesser equal `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'le'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_gt(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(lua_lessthan(L,2,1)){  /* check if rv < ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not greater than `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'gt'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_ge(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2){      /* rv ev */
    tolua_bnd_test_op_te(L); /* rv ev rvt evt ste *msg* */
    if(lua_toboolean(L,5)){  /* rv ev rvt evt ste?true  */
      if(lua_lessthan(L,2,1)||lua_equal(L,2,1)){  /* check if rv < ev */
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`");
      lua_pushvalue(L,1);
      lua_pushstring(L,"` (received value) not greater equal `");
      lua_pushvalue(L,2);
      lua_pushstring(L,"` (expected value)");
      lua_concat(L,5);
      return 2;              /* false msg */
    }                        /* rv ev rvt evt false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'ge'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_bw(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2&&tolua_istable(L,2,0,&tolua_err)){ /* rv tev */
    lua_pushnumber(L,1);     /* rv tev 1 */
    lua_gettable(L,2);       /* rv tev ev1 */
    lua_pushnumber(L,2);     /* rv tev ev1 2 */
    lua_gettable(L,2);       /* rv tev ev1 ev2 */
    lua_remove(L,2);         /* rv ev1 ev2 */
    tolua_typename(L,1);     /* rv ev1 ev2 rvt */
    tolua_typename(L,2);     /* rv ev1 ev2 rvt evt1 */
    tolua_typename(L,3);     /* rv ev1 ev2 rvt evt1 evt2 */
    if(lua_equal(L,4,5)&&lua_equal(L,5,6)){ /* type checking */
      if(lua_lessthan(L,2,1)&&
	 lua_lessthan(L,1,3)){  /* check if ev1 < rv < ev2 */
	lua_pushboolean(L,1);
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`"); /* rv ev rvt evt ste false "`" */
      lua_pushvalue(L,1);    /* rv ev rvt evt ste false "`" rv */
      lua_pushstring(L,"` (received value) not between { `"); /* rv ev rvt evt ste false "`" rv "not in { `" */
      lua_pushvalue(L,2);    /* rv ev rvt evt ste false "`" rv "` not in { `" ev1      */
      lua_pushstring(L,"`, `");/* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" */
      lua_pushvalue(L,3);    /* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" ev2 */
      lua_pushstring(L,"` } (expected values)");/* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" ev2 "` }" */
      lua_concat(L,7);       /* rv ev1 ev2 rvt evt1 evt2 ste false msg */
      return 2;              /* false msg */
    }                        /* rv ev1 ev2 rvt evt1 evt2 */
    lua_pushboolean(L,0);
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,4);
    lua_pushstring(L,"` but type of expected values is `");
    lua_pushvalue(L,5);
    lua_pushstring(L,"` and `");
    lua_pushvalue(L,6);
    lua_pushstring(L,"`");
    lua_concat(L,7);         /* rv ev rvt evt ste false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'bw'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_on(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err;
  if(lua_gettop(L)==2&&tolua_istable(L,2,0,&tolua_err)){ /* rv tev */
    lua_pushnumber(L,1);     /* rv tev 1 */
    lua_gettable(L,2);       /* rv tev ev1 */
    lua_pushnumber(L,2);     /* rv tev ev1 2 */
    lua_gettable(L,2);       /* rv tev ev1 ev2 */
    lua_remove(L,2);         /* rv ev1 ev2 */
    tolua_typename(L,1);     /* rv ev1 ev2 rvt */
    tolua_typename(L,2);     /* rv ev1 ev2 rvt evt1 */
    tolua_typename(L,3);     /* rv ev1 ev2 rvt evt1 evt2 */
    if(lua_equal(L,4,5)&&lua_equal(L,5,6)){ /* type checking */
      if((lua_lessthan(L,2,1)||lua_equal(L,2,1))&&
	 (lua_lessthan(L,1,3)||lua_equal(L,1,3))){  /* check if ev1 <= rv <= ev2 */
	lua_pushboolean(L,1);
	return 1;            /* true */
      }
      lua_pushboolean(L,0);  /* rv ev rvt evt ste false */
      lua_pushstring(L,"`"); /* rv ev rvt evt ste false "`" */
      lua_pushvalue(L,1);    /* rv ev rvt evt ste false "`" rv */
      lua_pushstring(L,"` (received value) not onto { `"); /* rv ev rvt evt ste false "`" rv "not in { `" */
      lua_pushvalue(L,2);    /* rv ev rvt evt ste false "`" rv "` not in { `" ev1      */
      lua_pushstring(L,"`, `");/* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" */
      lua_pushvalue(L,3);    /* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" ev2 */
      lua_pushstring(L,"` } (expected values)");/* rv ev rvt evt ste false "`" rv "` not in { `" ev1 "`, `" ev2 "` }" */
      lua_concat(L,7);       /* rv ev1 ev2 rvt evt1 evt2 ste false msg */
      return 2;              /* false msg */
    }                        /* rv ev1 ev2 rvt evt1 evt2 */
    lua_pushboolean(L,0);
    lua_pushstring(L,"type of received value is `");
    lua_pushvalue(L,4);
    lua_pushstring(L,"` but type of expected values is `");
    lua_pushvalue(L,5);
    lua_pushstring(L,"` and `");
    lua_pushvalue(L,6);
    lua_pushstring(L,"`");
    lua_concat(L,7);         /* rv ev rvt evt ste false msg */
    return 2;                /* false msg */
  }
  tolua_error(L,"#ferror in function 'on'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_oo(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err; /* one of */
  lua_pushstring(L,"tostring");     /* rv tev key */
  lua_gettable(L,LUA_GLOBALSINDEX); /* rv tev *func* */
  if(lua_isfunction(L,-1)){        /* rv tev ?func? */
    lua_insert(L,1);               /* func rv tev */
    if(lua_gettop(L)==3&&tolua_istable(L,3,0,&tolua_err)){ /* func rv tev */
      tolua_typename(L,2);       /* func rv tev rvt */
      lua_pushstring(L,"");      /* func rv tev rvt msg */
      lua_pushnil(L);            /* func rv tev rvt msg nil */
      for(;lua_next(L,3);){      /* func rv tev rvt msg key */
	tolua_typename(L,-1);    /* func rv tev rvt msg key evi evit */
	//printf("%s\n",tolua_stackexplore(L));
	if(lua_equal(L,4,-1)){   /* type checking rvt==evit */
	  if(lua_equal(L,2,-2)){ /* values checking rv==evi */
	    lua_pushboolean(L,1);
	    return 1;            /* true */
	  }
	}                        /* func rv tev rvt msg key evi evit */
	lua_remove(L,-1);        /* func rv tev rvt msg key evi */
	lua_insert(L,-2);        /* func rv tev rvt msg evi key */
	lua_insert(L,-3);        /* func rv tev rvt key msg evi */
	if(lua_objlen(L,-2)){
	  lua_pushstring(L,", `"); /* func rv tev rvt key msg evi msg1 */
	}else{
	  lua_pushstring(L,"`");   /* func rv tev rvt key msg evi msg1 */
	}
	lua_insert(L,-2);          /* func rv tev rvt key msg msg1 evi */
	//if(lua_isnil(L,-1)){
	//  lua_remove(L,-1);
	//  lua_pushstring(L,"nil");
	//}
	//printf("%s\n",tolua_stackexplore(L));
	lua_pushvalue(L,1);        /* func rv tev rvt key msg msg1 evi func */
	lua_insert(L,-2);          /* func rv tev rvt key msg msg1 func evi */
	lua_pcall(L,1,1,0);        /* func rv tev rvt key msg msg1 sevi */
	lua_pushstring(L,"`");     /* func rv tev rvt key msg msg1 sevi msg2 */
	lua_concat(L,4);           /* func rv tev rvt key msg */
	lua_insert(L,-2);          /* func rv tev rvt msg key */
      }                         /* func rv tev rvt msg */
      lua_pushboolean(L,0);  /* func rv tev rvt msg false */
      lua_pushstring(L,"`"); /* func rv tev rvt msg false msg1 */
      lua_pushvalue(L,2);    /* func rv tev rvt msg false msg1 rv */
      //if(lua_isnil(L,-1)){
      //lua_remove(L,-1);
      //lua_pushstring(L,"nil");
      //}
      //printf("%s\n",tolua_stackexplore(L));
      lua_pushvalue(L,1);    /* func rv tev rvt msg false msg1 rv func */
      lua_insert(L,-2);      /* func rv tev rvt msg false msg1 func rv */
      lua_pcall(L,1,1,0);    /* func rv tev rvt msg false msg1 srv */
      lua_pushstring(L,"` (received value) not one of { "); /* func rv tev rvt msg false msg1 rv msg2 */
      lua_pushvalue(L,5); /* func rv tev rvt msg false msg1 rv msg2 msg */
      lua_pushstring(L," } (expected values)"); /* func rv tev rvt msg false msg1 rv msg2 msg msg3 */
      lua_concat(L,5);       /* func rv tev rvt msg false msg */
      return 2;              /* false msg */
    }
  }
  tolua_error(L,"#ferror in function 'oo'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_op_no(lua_State* L){ /* received value, expected value */
  tolua_Error tolua_err; /* one of */
  lua_pushstring(L,"tostring");     /* rv tev key */
  lua_gettable(L,LUA_GLOBALSINDEX); /* rv tev *func* */
  if(lua_isfunction(L,-1)){        /* rv tev ?func? */
    lua_insert(L,1);               /* func rv tev */
    if(lua_gettop(L)==3&&tolua_istable(L,3,0,&tolua_err)){ /* func rv tev */
      tolua_typename(L,2);       /* func rv tev rvt */
      lua_pushnil(L);            /* func rv tev rvt nil */
      for(;lua_next(L,3);){      /* func rv tev rvt key */
	tolua_typename(L,-1);    /* func rv tev rvt key evi evit */
	//printf("%s\n",tolua_stackexplore(L));
	if(lua_equal(L,4,-1)){   /* type checking rvt==evit */
	  if(lua_equal(L,2,-2)){ /* values checking rv==evi */
	    lua_pushboolean(L,0);  /* func rv tev rvt key evi evit false */
	    lua_pushstring(L,"`"); /* func rv tev rvt key evi evit false msg1 */
	    lua_pushvalue(L,2);    /* func rv tev rvt key evi evit false msg1 rv */
	    lua_pushvalue(L,1);    /* func rv tev rvt key evi evit false msg1 rv func */
	    lua_insert(L,-2);      /* func rv tev rvt key evi evit false msg1 func rv */
	    lua_pcall(L,1,1,0);    /* func rv tev rvt key evi evit false msg1 srv */
	    lua_pushstring(L,"` (received value) is `");
	                           /* func rv tev rvt key evi evit false msg1 srv msg2 */
	    lua_pushvalue(L,1);    /* func rv tev rvt key evi evit false msg1 srv msg2 func */
	    lua_pushvalue(L,6);    /* func rv tev rvt key evi evit false msg1 srv msg2 func evi */
	    lua_pcall(L,1,1,0);    /* func rv tev rvt key evi evit false msg1 srv msg2 sev */
	    lua_pushstring(L,"` one of (expected values)"); /* func rv tev rvt key evi evit false msg1 srv msg2 sev msg3 */
	    lua_concat(L,5);       /* func rv tev rvt msg false msg */
	    return 2;            /* true */
	  }
	}                        /* func rv tev rvt key evi evit */
	lua_remove(L,-1);        /* func rv tev rvt key evi */
	lua_remove(L,-1);        /* func rv tev rvt key */
      }                          /* func rv tev rvt */
      lua_pushboolean(L,1);  /* func rv tev rvt msg true */
      return 1;              /* false msg */
    }
  }
  tolua_error(L,"#ferror in function 'oo'.",&tolua_err);
  return 0;
}
static int tolua_bnd_test_assert(lua_State* L){ /* test pass assert */
  tolua_Error tolua_err;                /* self rv ev *op* *desc* */
  if(!tolua_isusertype(L,1,"tolua::test",0,&tolua_err))goto tolua_lerror;
  if(lua_gettop(L)==1)return tolua_bnd_test_get_passed(L);
  if(lua_isfunction(L,4)){              /* self rv ev ?op? *desc* */
                                        /* self rv ev  op  *desc* */
  }else{                                /* self rv ev *desc*      */
    lua_pushstring(L,"eq");             /* self rv ev *desc* "op" */
    lua_gettable(L,1);                  /* self rv ev *desc*  op  */
    if(lua_gettop(L)==5){               /* check if ?desc?        */
      lua_insert(L,-2);                 /* self rv ev op  desc    */
    }                                   /* self rv ev op *desc*   */
  }
  if(tolua_isstring(L,5,0,&tolua_err)){ /* self rv ev op ?desc?   */
                                        /* self rv ev op  desc    */
  }else{                                /* self rv ev op          */
    /*
    switch(op){
    case tolua_test_mode::tn:
      lua_pushstring(L,"type of received value not same type of expected value"); break;
    case tolua_test_mode::te:
      lua_pushstring(L,"type of received value some as type of expected value"); break;
    case tolua_test_mode::ne:
      lua_pushstring(L,"received value ~= expected value"); break;
    case tolua_test_mode::eq:
      lua_pushstring(L,"received value == expected value"); break;
    case tolua_test_mode::lt:
      lua_pushstring(L,"received value < expected value"); break;
    case tolua_test_mode::le:
      lua_pushstring(L,"received value <= expected value"); break;
    case tolua_test_mode::gt:
      lua_pushstring(L,"received value > expected value"); break;
    case tolua_test_mode::ge:
      lua_pushstring(L,"received value >= expected value"); break;
    default:
      lua_pushstring(L,"user defined checking operation");
    }                                   /* self rv ev op  desc    */
    lua_pushstring(L,"default checking operation"); /* self rv ev op desc */
  }
  tolua_Test*self=(tolua_Test*)tolua_tousertype(L,1,0);
  /*char*tolua_desc=tolua_tostring(tolua_S,4,0);*/
  if(!self)tolua_error(L,"invalid 'self' in function 'assert'",NULL);
  else{
    self->count=self->count+1;             /* self rv ev op desc */
    /* create a new pass result table and place it in peer */
    lua_newtable(L);                       /* self rv ev op desc pass */
    lua_getfenv(L,1);                      /* self rv ev op desc pass env */
    lua_pushnumber(L,self->count);         /* self rv ev op desc pass env key */
    lua_pushvalue(L,-3);                   /* self rv ev op desc pass env key pass */
    lua_settable(L,-3);                    /* self rv ev op desc pass env */
    lua_pop(L,1);                          /* self rv ev op desc pass */
    /* pack description */
    lua_insert(L,-2);                      /* self rv ev op pass desc */
    lua_pushstring(L,"description");       /* self rv ev op pass desc key */
    lua_insert(L,-2);                      /* self rv ev op pass key desc */
    lua_settable(L,-3);                    /* self rv ev op pass */
    /* pack received value */
    lua_pushstring(L,"received");          /* self rv ev op pass key */
    lua_pushvalue(L,2);                    /* self rv ev op pass key rv */
    lua_settable(L,-3);                    /* self rv ev op pass */
    /* pack expected value */
    lua_pushstring(L,"expected");          /* self rv ev op pass key */
    lua_pushvalue(L,3);                    /* self rv ev op pass key ev */
    lua_settable(L,-3);                    /* self rv ev op pass */
    /* pack operation */
    lua_pushstring(L,"operation");         /* self rv ev op pass key */
    lua_pushvalue(L,4);                    /* self rv ev op pass key op */
    lua_settable(L,-3);                    /* self rv ev op pass */
    /* test value */
    lua_insert(L,-4);                      /* self pass rv ev op */
    lua_insert(L,-3);                      /* self pass op rv ev */
    lua_pcall(L,2,-1,0);                   /* self pass st *msg* */
    if(lua_isstring(L,-1)){                /* self pass st ?msg? */
      lua_pushstring(L,"message");         /* self pass st  msg key */
      lua_pushvalue(L,-2);                 /* self pass st  msg key msg */
      lua_settable(L,-5);                  /* self pass st  msg */
      lua_insert(L,-3);                    /* self msg pass st */
    }else if(!lua_toboolean(L,-1)){        /* self pass st?false */
      lua_pushstring(L,"undefined error!");/* self pass st  msg */
      lua_pushstring(L,"message");         /* self pass st  msg key */
      lua_pushvalue(L,-2);                 /* self pass st  msg key msg */
      lua_settable(L,-5);                  /* self pass st  msg */
      lua_insert(L,-3);                    /* self msg pass st */
    }
    /* count passed */
    self->passed+=lua_toboolean(L,-1)?1:0;
    /* persent passed (progress) */
    self->progress=self->passed*100/self->count;
    lua_pushstring(L,"state");             /* self *msg* pass st key */
    lua_pushvalue(L,-2);                   /* self *msg* pass st key st */
    lua_settable(L,-4);                    /* self *msg* pass st */
    if(!lua_isstring(L,-3))return 1;       /* self ?msg? pass st */
    lua_pushvalue(L,-3);                   /* self *msg* pass st *msg* */
    return 2;
  }
  return 0;
 tolua_lerror:
  tolua_error(L,"#ferror in function 'assert'.",&tolua_err);
  return 0;
}
#endif /* #ifndef DISABLE_TEST_FRAMEWORK */

TOLUA_API const char* tolua_stackexplore(lua_State* L){
  unsigned int i;
  static char m[256]="";
  sprintf(m,"[%d](",lua_gettop(L));
  for(i=1;i<=lua_gettop(L)&&i<LUA_MINSTACK;i++){
    sprintf(m+strlen(m)," [%d|%d]{%s|",i,i-lua_gettop(L),tolua_typename(L,i));
    lua_pop(L,1);
    switch(lua_type(L,i)){
    case LUA_TNUMBER: {lua_Number n=lua_tonumber(L,i); 
	sprintf(m+strlen(m),(n-(lua_Number)(long)n)?"%f":"%d",(long)n);} break;
    case LUA_TBOOLEAN:  sprintf(m+strlen(m),"%s",lua_toboolean(L,i)?"true":"false"); break;
    case LUA_TSTRING:   sprintf(m+strlen(m),"\"%s\"",lua_tostring(L,i)); break;
    case LUA_TUSERDATA: sprintf(m+strlen(m),"`%s`",tolua_typename(L,i)); lua_pop(L,1); break;
    case LUA_TTABLE:    sprintf(m+strlen(m),"0x%x",lua_topointer(L,i)); break;
    case LUA_TFUNCTION: sprintf(m+strlen(m),"0x%x",lua_topointer(L,i)); break;
    default: sprintf(m+strlen(m),"nv");
    }
    sprintf(m+strlen(m),"}");
  }
  sprintf(m+strlen(m)," )");
  return m;
}

#ifndef DISABLE_PROXY_TECHNIQUES /* toluaxx proxy techniques */
TOLUA_API void tolua_proxystack(lua_State* L){
  /* stack: */
  lua_pushstring(L,"tolua_proxy");  /* stack: "tolua_proxy" */
  lua_rawget(L,LUA_REGISTRYINDEX);  /* stack: ?proxy_stack_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_stack_table? is not table */
    lua_remove(L,-1);               /* stack: */
    lua_pushnil(L);                 /* stack: nil */
    return;
  }
  /* stack: proxy_stack_table */
}
TOLUA_API int tolua_proxytop(lua_State* L){
  int __tolua_proxy_top_;
  int __top_;
  __top_=lua_gettop(L);              /* stack: */
  lua_pushstring(L,"tolua_proxy");   /* stack: "tolua_proxy" */
  lua_rawget(L,LUA_REGISTRYINDEX);   /* stack: ?proxy_stack_table? */
  if(lua_istable(L,-1)){    /* check if ?proxy_stack_table? is table */
    lua_pushnumber(L,0);             /* stack: proxy_stack_table 0 */
    lua_rawget(L,-2);                /* stack: proxy_stack_table ?proxy_stack_top? */
    if(lua_isnumber(L,-1)){ /* check if ?proxy_stack_top? is number */
      __tolua_proxy_top_=lua_tonumber(L,-1);
    }else __tolua_proxy_top_=-2;
  }else __tolua_proxy_top_=-1;
  lua_settop(L,__top_);
  return __tolua_proxy_top_;
}
TOLUA_API int tolua_proxypush(lua_State* L){
  int __tolua_proxy_top_;
  
  /* get proxy stack table */           /* stack: */
  lua_pushstring(L,"tolua_proxy");      /* stack: "tolua_proxy" */
  lua_rawget(L,LUA_REGISTRYINDEX);      /* stack: ?proxy_stack_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_stack_table? is not table */
    lua_remove(L,-1);                   /* stack: */
    return 0;
  }

  /* get stack level top counter */     /* stack: proxy_stack_table */
  lua_pushnumber(L,0);                  /* stack: proxy_stack_table 0 */
  lua_rawget(L,-2);                     /* stack: proxy_stack_table ?proxy_stack_top? */
  if(!lua_isnumber(L,-1)){ /* check if ?proxy_stack_top? is not number */
    lua_remove(L,-1);                   /* stack: proxy_stack_table */
    lua_remove(L,-1);                   /* stack: */
    return 0;
  }
  __tolua_proxy_top_=lua_tonumber(L,-1);
  lua_remove(L,-1);                     /* stack: proxy_stack_table */

  /* decrement stack level top counter */
  __tolua_proxy_top_=__tolua_proxy_top_+1;
  lua_pushnumber(L,0);                  /* stack: proxy_stack_table 0 */
  lua_pushnumber(L,__tolua_proxy_top_); /* stack: proxy_stack_table 0 new_proxy_stack_top */
  lua_rawset(L,-3);                     /* stack: proxy_stack_table */
  
  /* create a new proxy stack level */
  lua_pushnumber(L,__tolua_proxy_top_); /* stack: proxy_stack_table new_proxy_stack_top */
  lua_newtable(L);                      /* stack: proxy_stack_table new_proxy_stack_level */
  lua_rawset(L,-3);                     /* stack: proxy_stack_table */
  
  /* clear lua stack */
  lua_remove(L,-1);                     /* stack: */
  return 1;
}
TOLUA_API int tolua_proxypop(lua_State* L){
  int __tolua_proxy_top_;
  
  /* get proxy stack table */           /* stack: */
  lua_pushstring(L,"tolua_proxy");      /* stack: "tolua_proxy" */
  lua_rawget(L,LUA_REGISTRYINDEX);      /* stack: ?proxy_stack_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_stack_table? is not table */
    lua_remove(L,-1);                   /* stack: */
    return 0;
  }
  
  /* get stack level top counter */     /* stack: proxy_stack_table */
  lua_pushnumber(L,0);                  /* stack: proxy_stack_table 0 */
  lua_rawget(L,-2);                     /* stack: proxy_stack_table ?proxy_stack_top? */
  if(!lua_isnumber(L,-1)){ /* check if ?proxy_stack_top? is not number */
    lua_remove(L,-1);                   /* stack: proxy_stack_table */
    lua_remove(L,-1);                   /* stack: */
    return 0;
  }
  __tolua_proxy_top_=lua_tonumber(L,-1);
  lua_remove(L,-1);                     /* stack: proxy_stack_table */
  if(__tolua_proxy_top_<1){ /* check if proxy top counter is smaller one */
    lua_remove(L,-1);                   /* stack: */
    return 0;
  }
  /* remove proxy stack level */
  lua_pushnumber(L,__tolua_proxy_top_); /* stack: proxy_stack_table proxy_stack_top */
  lua_pushnil(L);                       /* stack: proxy_stack_table  */
  lua_rawset(L,-3);                     /* stack: proxy_stack_table */
  
  /* decrement stack level top counter */
  __tolua_proxy_top_=__tolua_proxy_top_-1;
  lua_pushnumber(L,0);                  /* stack: proxy_stack_table 0 */
  lua_pushnumber(L,__tolua_proxy_top_); /* stack: proxy_stack_table 0 new_proxy_stack_top */
  lua_rawset(L,-3);                     /* stack: proxy_stack_table */
  
  /* clear lua stack */
  lua_remove(L,-1);                     /* stack: */
  return 1;
}
TOLUA_API void tolua_proxylevel(lua_State* L, int level){
  /* stack: */
  int __tolua_proxy_top_=tolua_proxytop(L);
  if( level>+__tolua_proxy_top_ ||
      level<-__tolua_proxy_top_ ||
      level==0 ){ /* check if level exceded */
    lua_pushnil(L);                 /* stack: nil */
    return;
  }                                 /* stack: */
  lua_pushstring(L,"tolua_proxy");  /* stack: "tolua_proxy" */
  lua_rawget(L,LUA_REGISTRYINDEX);  /* stack: ?proxy_stack_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_stack_table? is not table */
    lua_remove(L,-1);               /* stack: */
    lua_pushnil(L);                 /* stack: nil */
    return;
  }
  /* stack: proxy_stack_table */
  level=level>0?level:__tolua_proxy_top_+level+1;
  lua_pushnumber(L,level);          /* stack: proxy_stack_table proxy_stack_level */
  lua_rawget(L,-2);                 /* stack: proxy_stack_table ?proxy_level_table? */
  lua_remove(L,-2);                 /* stack: ?proxy_level_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_level_table? is not table */
    lua_remove(L,-1);               /* stack: */
    lua_pushnil(L);                 /* stack: nil */
    return;
  }
  return;                           /* stack: proxy_level_table */
}
TOLUA_API void tolua_getproxy(lua_State* L, int level){
  /* stack: key */
  tolua_proxylevel(L,level);        /* stack: key ?proxy_level_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_level_table? is not table */
    lua_remove(L,-1);               /* stack: key */
    lua_remove(L,-1);               /* stack: */
    lua_pushnil(L);                 /* stack: nil */
    return;
  }                                 /* stack: key proxy_level_table */
  lua_insert(L,-2);                 /* stack: proxy_level_table key */
  lua_rawget(L,-2);                 /* stack: proxy_level_table value */
  lua_remove(L,-2);                 /* stack: value */
  return;
}
TOLUA_API void tolua_setproxy(lua_State* L, int level){
  /* stack: key val */
  tolua_proxylevel(L,level);        /* stack: key val ?proxy_level_table? */
  if(!lua_istable(L,-1)){  /* check if ?proxy_level_table? is not table */
    lua_remove(L,-1);               /* stack: key val */
    lua_remove(L,-1);               /* stack: key */
    lua_remove(L,-1);               /* stack: */
    return;
  }                                 /* stack: key val proxy_level_table */
  lua_insert(L,-3);                 /* stack: proxy_level_table key val */
  lua_rawset(L,-3);                 /* stack: proxy_level_table */
  lua_remove(L,-2);                 /* stack: */
  return;
}
/* local state = tolua.proxy.push() */
static int tolua_bnd_proxy_push(lua_State* L){
  int __ret_code_;
  __ret_code_=tolua_proxypush(L);
  lua_pushboolean(L,__ret_code_);
  return 1;
}
/* local state = tolua.proxy.pop() */
static int tolua_bnd_proxy_pop(lua_State* L){
  int __ret_code_;
  __ret_code_=tolua_proxypop(L);
  lua_pushboolean(L,__ret_code_);
  return 1;
}
/* local level_table = tolua.proxy.level(<level>) */
static int tolua_bnd_proxy_level(lua_State* L){
  /* stack: <level> */
  int __proxy_level_=TOLUA_PROXY_TOP;
  if(lua_isnumber(L,1)){ /* check if level has been retrieved */
    /* stack: level */
    __proxy_level_=lua_tonumber(L,1);
    lua_remove(L,1);                 /* stack: */
  }                                  /* stack: */
  tolua_proxylevel(L,__proxy_level_);
  return 1;
}
/* local val = tolua.proxy.get(key<,level>) */
static int tolua_bnd_proxy_get(lua_State* L){
  /* stack: key <level> */
  int __proxy_level_=TOLUA_PROXY_TOP;
  if(lua_gettop(L)<1 || lua_isnil(L,1)){  /* check if key hasn't been retrieved */
    lua_pushnil(L);
    return 1;
  }
  if(lua_isnumber(L,2)){ /* check if level has been retrieved */
    /* stack: key level */
    __proxy_level_=lua_tonumber(L,2);
    lua_remove(L,2);                 /* stack: key */
  }                                  /* stack: key */
  tolua_getproxy(L,__proxy_level_);  /* stack: val */
  return 1;
}
/* tolua.proxy.set(key,value<,level>) */
static int tolua_bnd_proxy_set(lua_State* L){
  /* stack: key val <level> */
  int __proxy_level_=TOLUA_PROXY_TOP;
  if(lua_gettop(L)<1 || lua_isnil(L,1)){  /* check if key hasn't been retrieved */
    return 0;
  }
  if(lua_gettop(L)<2){  /* check if val hasn't been retrieved */
    return 0;
  }
  if(lua_isnumber(L,3)){ /* check if level has been retrieved */
    /* stack: key val level */
    __proxy_level_=lua_tonumber(L,3);
    lua_remove(L,3);                 /* stack: key val */
  }                                  /* stack: key val */
  tolua_setproxy(L,__proxy_level_);  /* stack: */
  return 0;
}
/* local top = tolua.proxy.top() */
static int tolua_bnd_proxy_top(lua_State* L){
  int __top_=tolua_proxytop(L);
  lua_pushnumber(L,__top_);
  return 1;
}
#endif /* ifndef DISABLE_PROXY_TECHNIQUES */

#ifndef DISABLE_WATCH_OBJECT /* for testing only >>>> */
static int tolua_watch_object_len(lua_State* L){
  unsigned long i;                              /* stack: table|ud .. */
  if(lua_isuserdata(L,1)){                      /* stack: ?userdata? */
    lua_getfenv(L,1);                           /* stack: userdata table */
  }
  if(lua_istable(L,-1)){                          /* stack: .. table */
    lua_pushnil(L);                               /* stack: .. table nil */
    for(i=0;lua_next(L,-2);lua_remove(L,-1),i++); /* stack: .. table key */
    lua_pushnumber(L,i);                          /* stack: .. table key len */
    return 1;                                     /* stack: len */
  }
  return 0;
}
static int tolua_watch_object_next(lua_State* L){
  if(lua_isuserdata(L,1)){
    lua_getfenv(L,1);
  }
  if(lua_istable(L,-1)){
    lua_pushvalue(L,3);
    if(!lua_next(L,-2))return 0;
    return 2;
  }
  return 0;
}
static int tolua_watch_object_get(lua_State* L){
  unsigned long i;                              /* stack: table|ud key .. */
  if(lua_isuserdata(L,1)){                      /* stack: ?userdata? */
    lua_getfenv(L,1);                           /* stack: userdata key table */
    lua_insert(L,-2);                           /* stack: userdata table key */
  }
  if(lua_istable(L,-2)){                        /* stack: .. table key */
    lua_gettable(L,-2);                         /* stack: .. table key true */
    return 1;                                   /* stack: true */
  }
  return 0;
}
#endif /* #ifndef DISABLE_WATCH_OBJECT */ /* for testing only <<<< */

/* static int class_gc_event (lua_State* L); */

TOLUA_API void tolua_open (lua_State* L){
  int top = lua_gettop(L);
  lua_pushstring(L,"tolua_opened");
  lua_rawget(L,LUA_REGISTRYINDEX);
  if (!lua_isboolean(L,-1)){
    lua_pushstring(L,"tolua_opened"); lua_pushboolean(L,1); lua_rawset(L,LUA_REGISTRYINDEX);
    
#ifndef LUA_VERSION_NUM /* only prior to lua 5.1 */
    /* create peer object table */
    lua_pushstring(L, "tolua_peers"); lua_newtable(L);
    /* make weak key metatable for peers indexed by userdata object */
    lua_newtable(L); lua_pushliteral(L, "__mode"); lua_pushliteral(L, "k"); lua_rawset(L, -3);                /* stack: string peers mt */
    lua_setmetatable(L, -2);   /* stack: string peers */
    lua_rawset(L,LUA_REGISTRYINDEX);
#endif
    
    /* create object ptr -> udata mapping table */
    lua_pushstring(L,"tolua_ubox"); lua_newtable(L);
    /* make weak value metatable for ubox table to allow userdata to be
       garbage-collected */
    lua_newtable(L); lua_pushliteral(L, "__mode"); lua_pushliteral(L, "v"); lua_rawset(L, -3);
    /* stack: string ubox mt */
    lua_setmetatable(L, -2);  /* stack: string ubox */
    lua_rawset(L,LUA_REGISTRYINDEX);
    
    lua_pushstring(L,"tolua_super"); lua_newtable(L); lua_rawset(L,LUA_REGISTRYINDEX);
    lua_pushstring(L,"tolua_gc");    lua_newtable(L); lua_rawset(L,LUA_REGISTRYINDEX);
    lua_pushstring(L,"tolua_node");  lua_newtable(L); lua_rawset(L,LUA_REGISTRYINDEX);
    
#ifndef DISABLE_WATCH_OBJECT /* for testing only >>>> */
    lua_newtable(L);                            /* stack: .. mt */
    lua_pushliteral(L,"__mode");                /* stack: .. mt key */
    lua_pushliteral(L,"kv");                    /* stack: .. mt key val */
    lua_rawset(L,-3);                           /* stack: .. mt */
    /* env table */
    lua_pushstring(L,"tolua_envtable");         /* stack: .. mt name */
    lua_newtable(L);                            /* stack: .. mt name table */
    lua_pushvalue(L,-3);                        /* stack: .. mt name table mt */
    lua_setmetatable(L,-2);                     /* stack: .. mt name table */
    lua_rawset(L,LUA_REGISTRYINDEX);            /* stack: .. mt */
    /* obj table */
    lua_pushstring(L,"tolua_objtable");         /* stack: .. mt name */
    lua_newtable(L);                            /* stack: .. mt name table */
    lua_pushvalue(L,-3);                        /* stack: .. mt name table mt */
    lua_setmetatable(L,-2);                     /* stack: .. mt name table */
    lua_rawset(L,LUA_REGISTRYINDEX);            /* stack: .. mt */
    lua_remove(L,-1);                           /* stack: .. */
#endif /*DISABLE_WATCH_OBJECT*/ /* for testing only <<<< */

#ifndef DISABLE_PROXY_TECHNIQUES
    /* create proxy table */
    lua_pushstring(L,"tolua_proxy");
    lua_newtable(L);
    /* insert counter into proxy table */
    lua_pushnumber(L,0);
    lua_pushnumber(L,0);
    lua_rawset(L,-3);
    /* register proxy table */
    lua_rawset(L,LUA_REGISTRYINDEX);
#endif
    
    /* create gc_event closure */
    lua_pushstring(L, "tolua_gc_event");
    lua_pushstring(L, "tolua_gc");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushstring(L, "tolua_super");
    lua_rawget(L, LUA_REGISTRYINDEX);
    lua_pushcclosure(L, class_gc_event, 2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    
    tolua_newmetatable(L,"tolua_commonclass");
    
    tolua_module(L,NULL,0);
    tolua_beginmodule(L,NULL);
    tolua_module(L,"tolua",0);
    tolua_beginmodule(L,"tolua");
    tolua_function(L,"type",tolua_bnd_type);
    tolua_function(L,"takeownership",tolua_bnd_takeownership);
    tolua_function(L,"releaseownership",tolua_bnd_releaseownership);
    tolua_function(L,"cast",tolua_bnd_cast);
    tolua_function(L,"inherit",tolua_bnd_inherit);
#ifdef LUA_VERSION_NUM /* lua 5.1 */
    tolua_function(L,"setpeer",tolua_bnd_setpeer);
    tolua_function(L,"getpeer",tolua_bnd_getpeer);
#endif

#ifndef DISABLE_TEST_FRAMEWORK
    /* test framework class */
    tolua_usertype(L,"tolua::test");
    tolua_cclass(L,"test","tolua::test","",tolua_bnd_test_del);
    tolua_beginmodule(L,"test");
    tolua_function(L,"new",        tolua_bnd_test_new);
    tolua_function(L,".call",      tolua_bnd_test_new);
    tolua_function(L,"delete",     tolua_bnd_test_del);
    tolua_function(L,".len",       tolua_bnd_test_get_count);
    tolua_function(L,"assert",     tolua_bnd_test_assert);
    tolua_function(L,".callself",  tolua_bnd_test_assert);
    tolua_function(L,".string",    tolua_bnd_test_get_report);
    /* test variables */
    tolua_variable(L,"name",       tolua_bnd_test_get_name,NULL);
    tolua_variable(L,"author",     tolua_bnd_test_get_author,NULL);
    tolua_variable(L,"description",tolua_bnd_test_get_description,NULL);
    tolua_variable(L,"count",      tolua_bnd_test_get_count,NULL);
    tolua_variable(L,"passed",     tolua_bnd_test_get_passed,NULL);
    tolua_variable(L,"progress",   tolua_bnd_test_get_progress,NULL);
    tolua_variable(L,"state",      tolua_bnd_test_get_state,NULL);
    tolua_variable(L,"report",     tolua_bnd_test_get_report,NULL);
    tolua_variable(L,"result",     tolua_bnd_test_get_result,NULL);
    tolua_variable(L,"errors",     tolua_bnd_test_get_errors,NULL);
    /* test operation */
    /* type specific */
    tolua_function(L,"type_is",          tolua_bnd_test_op_ti);
    tolua_function(L,"ti",               tolua_bnd_test_op_ti);
    tolua_function(L,"type_not_is",      tolua_bnd_test_op_ni);
    tolua_function(L,"tni",              tolua_bnd_test_op_ni);
    tolua_function(L,"ni",               tolua_bnd_test_op_ni);
    /* type equivalence */
    tolua_function(L,"type_not_equal",   tolua_bnd_test_op_tn);
    tolua_function(L,"tneq",             tolua_bnd_test_op_tn);
    tolua_function(L,"tne",              tolua_bnd_test_op_tn);
    tolua_function(L,"tn",               tolua_bnd_test_op_tn);
    tolua_function(L,"type_equal",       tolua_bnd_test_op_te);
    tolua_function(L,"teq",              tolua_bnd_test_op_te);
    tolua_function(L,"te",               tolua_bnd_test_op_te);
    /* equivalence */
    tolua_function(L,"not_equal",        tolua_bnd_test_op_ne);
    tolua_function(L,"neq",              tolua_bnd_test_op_ne);
    tolua_function(L,"ne",               tolua_bnd_test_op_ne);
    tolua_function(L,"equal",            tolua_bnd_test_op_eq);
    tolua_function(L,"eq",               tolua_bnd_test_op_eq);
    /* lesser */
    tolua_function(L,"lesser_than",      tolua_bnd_test_op_lt);
    tolua_function(L,"lessthan",         tolua_bnd_test_op_lt);
    tolua_function(L,"lt",               tolua_bnd_test_op_lt);
    tolua_function(L,"lesser_equal",     tolua_bnd_test_op_le);
    tolua_function(L,"lessequal",        tolua_bnd_test_op_le);
    tolua_function(L,"le",               tolua_bnd_test_op_le);
    /* greater */
    tolua_function(L,"greater_than",     tolua_bnd_test_op_gt);
    tolua_function(L,"greatthan",        tolua_bnd_test_op_gt);
    tolua_function(L,"gt",               tolua_bnd_test_op_gt);
    tolua_function(L,"greater_equal",    tolua_bnd_test_op_ge);
    tolua_function(L,"greatequal",       tolua_bnd_test_op_ge);
    tolua_function(L,"ge",               tolua_bnd_test_op_ge);
    /* interval */
    tolua_function(L,"between",          tolua_bnd_test_op_bw);
    tolua_function(L,"bw",               tolua_bnd_test_op_bw);
    tolua_function(L,"onto",             tolua_bnd_test_op_on);
    tolua_function(L,"on",               tolua_bnd_test_op_on);
    /* one of / not one of*/
    tolua_function(L,"one_of",           tolua_bnd_test_op_oo);
    tolua_function(L,"oneof",            tolua_bnd_test_op_oo);
    tolua_function(L,"oo",               tolua_bnd_test_op_oo);
    tolua_function(L,"not_one_of",       tolua_bnd_test_op_no);
    tolua_function(L,"notoneof",         tolua_bnd_test_op_no);
    tolua_function(L,"no",               tolua_bnd_test_op_no);
    /* tostring equivalence */
    tolua_function(L,"string_equal",     tolua_bnd_test_op_se);
    tolua_function(L,"streq",            tolua_bnd_test_op_se);
    tolua_function(L,"se",               tolua_bnd_test_op_se);
    tolua_function(L,"string_not_equal", tolua_bnd_test_op_sn);
    tolua_function(L,"strneq",           tolua_bnd_test_op_sn);
    tolua_function(L,"sn",               tolua_bnd_test_op_sn);
    /* test node class */
    /*tolua_cclass(L,"node","tolua::test::node","",NULL);
    //tolua_beginmodule(L,"node");
    //tolua_variable(L,"description",tolua_bnd_test_node_get_description,NULL);
    //tolua_variable(L,"",tolua_bnd_test_node_get_name,NULL);
    //tolua_endmodule(L);*/
    tolua_endmodule(L);
#endif

#ifndef DISABLE_PROXY_TECHNIQUES
    tolua_module(L,"proxy",0);
    tolua_beginmodule(L,"proxy");
    tolua_function(L,"top",  tolua_bnd_proxy_top);
    tolua_function(L,"push", tolua_bnd_proxy_push);
    tolua_function(L,"pop",  tolua_bnd_proxy_pop);
    tolua_function(L,"set",  tolua_bnd_proxy_set);
    tolua_function(L,"get",  tolua_bnd_proxy_get);
    tolua_function(L,"level",tolua_bnd_proxy_level);
    tolua_endmodule(L);
#endif

#ifndef DISABLE_WATCH_OBJECT
    tolua_module(L,"watch",0);
    tolua_beginmodule(L,"watch");
    /* create metatable */                      /* stack: .. mod */
    lua_newtable(L);                            /* stack: .. mod mt  */
    lua_pushliteral(L,"__len");                 /* stack: .. mod mt key */
    lua_pushcfunction(L,tolua_watch_object_len);/* stack: .. mod mt key func */
    lua_rawset(L,-3);                           /* stack: .. mod mt */
    lua_pushliteral(L,"__call");                   /* stack: .. mod mt key */
    lua_pushcfunction(L,tolua_watch_object_next);  /* stack: .. mod mt key func */
    lua_rawset(L,-3);                              /* stack: .. mod mt */
    lua_pushliteral(L,"__index");                  /* stack: .. mod mt key */
    lua_pushcfunction(L,tolua_watch_object_get);   /* stack: .. mod mt key func */
    lua_rawset(L,-3);                              /* stack: .. mod mt */
    /* create object access point */    /* stack: .. mod mt */
    lua_pushstring(L,"object");         /* stack: .. mod mt name */
    lua_newuserdata(L,1);               /* stack: .. mod mt name point */
    lua_pushstring(L,"tolua_objtable"); /* stack: .. mod mt name point key */
    lua_gettable(L,LUA_REGISTRYINDEX);  /* stack: .. mod mt name point objtable */
    lua_setfenv(L,-2);                  /* stack: .. mod mt name point */
    lua_pushvalue(L,-3);                /* stack: .. mod mt name point mt */
    lua_setmetatable(L,-2);             /* stack: .. mod mt name point */
    lua_settable(L,-4);                 /* stack: .. mod mt */
    /* create objenv access point */    /* stack: .. mod mt */
    lua_pushstring(L,"objenv");         /* stack: .. mod mt name */
    lua_newuserdata(L,1);               /* stack: .. mod mt name point */
    lua_pushstring(L,"tolua_envtable"); /* stack: .. mod mt name point key */
    lua_gettable(L,LUA_REGISTRYINDEX);  /* stack: .. mod mt name point envtable */
    lua_setfenv(L,-2);                  /* stack: .. mod mt name point */
    lua_pushvalue(L,-3);                /* stack: .. mod mt name point mt */
    lua_setmetatable(L,-2);             /* stack: .. mod mt name point */
    lua_settable(L,-4);                 /* stack: .. mod mt */
    lua_remove(L,-1);                   /* stack: .. mod */
    tolua_endmodule(L);
#endif
    
    tolua_endmodule(L);
    tolua_endmodule(L);
  }
  lua_settop(L,top);
  tolua_setstate(L);
}
/* Copy a C object
 */
TOLUA_API void* tolua_copy (lua_State* L, void* value, unsigned int size){
  void* clone = (void*)malloc(size);
  if (clone) memcpy(clone,value,size);
  else tolua_error(L,"insuficient memory",NULL);
  return clone;
}
/* Default collect function
 */
TOLUA_API int tolua_default_collect (lua_State* tolua_S){
  void* self = tolua_tousertype(tolua_S,1,0);
  free(self);
  return 0;
}
/* Do clone
 */
TOLUA_API int tolua_register_gc (lua_State* L, int lo){
  int success = 1;
  void *value = *(void **)lua_touserdata(L,lo);
  lua_pushstring(L,"tolua_gc");
  lua_rawget(L,LUA_REGISTRYINDEX);
  lua_pushlightuserdata(L,value);
  lua_rawget(L,-2);
  if (!lua_isnil(L,-1)){ /* make sure that object is not already owned */
    success = 0;
  } else {
    lua_pushlightuserdata(L,value);
    lua_getmetatable(L,lo);
    lua_rawset(L,-4);
  }
  lua_pop(L,2);
  return success;
}
/* Register a usertype
 * It creates the correspoding metatable in the registry, for both 'type' and 'const type'.
 * It maps 'const type' as being also a 'type'
 */
TOLUA_API void tolua_usertype (lua_State* L, char* type)
{
  char ctype[128] = "const ";
  strncat(ctype,type,120);

  /* create both metatables */
  if (tolua_newmetatable(L,ctype) && tolua_newmetatable(L,type))
    mapsuper(L,type,ctype);             /* 'type' is also a 'const type' */
}
/* Begin module
 * It pushes the module (or class) table on the stack
 */
TOLUA_API void tolua_beginmodule(lua_State* L, char* name){
  if(name){
    lua_pushstring(L,name);
    lua_rawget(L,-2);
  }else lua_pushvalue(L,LUA_GLOBALSINDEX);
}
/* End module
 * It pops the module (or class) from the stack
 */
TOLUA_API void tolua_endmodule (lua_State* L){
  lua_pop(L,1);
}
/* Map module
 * It creates a new module
 */
TOLUA_API void tolua_module (lua_State* L, char* name, int hasvar){
  if(name){
    /* tolua module */
    lua_pushstring(L,name);
    lua_rawget(L,-2);
    if(!lua_istable(L,-1)){  /* check if module already exists */
      lua_pop(L,1);
      lua_newtable(L);
      lua_pushstring(L,name);
      lua_pushvalue(L,-2);
      lua_rawset(L,-4);       /* assing module into module */
    }
  }else{
    /* global table */
    lua_pushvalue(L,LUA_GLOBALSINDEX);
  }
  if(hasvar){
    if(!tolua_ismodulemetatable(L)){  /* check if it already has a module metatable */
      /* create metatable to get/set C/C++ variable */
      lua_newtable(L);
      tolua_moduleevents(L);
      if(lua_getmetatable(L,-2))lua_setmetatable(L,-2);  /* set old metatable as metatable of metatable */
      lua_setmetatable(L,-2);
    }
  }
  lua_pop(L,1);               /* pop module */
}
/* Push Collector */
static void push_collector(lua_State* L, const char* type, lua_CFunction col) {

  /* push collector function, but only if it's not NULL, or if there's no
     collector already */
  if (!col) return;
  luaL_getmetatable(L,type);
  lua_pushstring(L,".collector");
  /*
    if (!col) {
    lua_pushvalue(L, -1);
    lua_rawget(L, -3);
    if (!lua_isnil(L, -1)) {
    lua_pop(L, 3);
    return;
    };
    lua_pop(L, 1);
    };
    //	*/
  lua_pushcfunction(L,col);

  lua_rawset(L,-3);
  lua_pop(L, 1);
};
/* Map C class
 * It maps a C class, setting the appropriate inheritance and super classes.
 */
TOLUA_API void tolua_cclass (lua_State* L, char* lname, char* name, char* base, lua_CFunction col)
{
  char cname[128] = "const ";
  char cbase[128] = "const ";
  strncat(cname,name,120);
  strncat(cbase,base,120);

  mapinheritance(L,name,base);
  mapinheritance(L,cname,name);

  mapsuper(L,cname,cbase);
  mapsuper(L,name,base);

  lua_pushstring(L,lname);
	
  push_collector(L, name, col);
  /*
    luaL_getmetatable(L,name);
    lua_pushstring(L,".collector");
    lua_pushcfunction(L,col);

    lua_rawset(L,-3);
  */

  luaL_getmetatable(L,name);
  lua_rawset(L,-3);              /* assign class metatable to module */

  /* now we also need to store the collector table for the const
     instances of the class */
  push_collector(L, cname, col);
  /*
    luaL_getmetatable(L,cname);
    lua_pushstring(L,".collector");
    lua_pushcfunction(L,col);
    lua_rawset(L,-3);
    lua_pop(L,1);
  */
}
/* Add base
 * It adds additional base classes to a class (for multiple inheritance)
 * (not for now)
 TOLUA_API void tolua_addbase(lua_State* L, char* name, char* base) {

 char cname[128] = "const ";
 char cbase[128] = "const ";
 strncat(cname,name,120);
 strncat(cbase,base,120);

 mapsuper(L,cname,cbase);
 mapsuper(L,name,base);
 };
*/

/* Map function
 * It assigns a function into the current module (or class)
 */
TOLUA_API void tolua_function (lua_State* L, char* name, lua_CFunction func){
  lua_pushstring(L,name);
  lua_pushcfunction(L,func);
  lua_rawset(L,-3);
}
/* sets the __call event for the class (expects the class' main table on top) */
/*	never really worked :(
	TOLUA_API void tolua_set_call_event(lua_State* L, lua_CFunction func, char* type) {

	lua_getmetatable(L, -1);
	//luaL_getmetatable(L, type);
	lua_pushstring(L,"__call");
	lua_pushcfunction(L,func);
	lua_rawset(L,-3);
	lua_pop(L, 1);
	};
*/

/* Map constant number
 * It assigns a constant number into the current module (or class)
 */
TOLUA_API void tolua_constant (lua_State* L, char* name, double value){
  lua_pushstring(L,name);
  tolua_pushnumber(L,value);
  lua_rawset(L,-3);
}
/* Map variable
 * It assigns a variable into the current module (or class)
 */
TOLUA_API void tolua_variable (lua_State* L, char* name, lua_CFunction get, lua_CFunction set){
  /* get func */
  lua_pushstring(L,".get");
  lua_rawget(L,-2);
  if (!lua_istable(L,-1)) {
    /* create .get table, leaving it at the top */
    lua_pop(L,1);
    lua_newtable(L);
    lua_pushstring(L,".get");
    lua_pushvalue(L,-2);
    lua_rawset(L,-4);
  }
  lua_pushstring(L,name);
  lua_pushcfunction(L,get);
  lua_rawset(L,-3);                  /* store variable */
  lua_pop(L,1);                      /* pop .get table */
  
  /* set func */
  if (set) {
    lua_pushstring(L,".set");
    lua_rawget(L,-2);
    if (!lua_istable(L,-1)) {
      /* create .set table, leaving it at the top */
      lua_pop(L,1);
      lua_newtable(L);
      lua_pushstring(L,".set");
      lua_pushvalue(L,-2);
      lua_rawset(L,-4);
    }
    lua_pushstring(L,name);
    lua_pushcfunction(L,set);
    lua_rawset(L,-3);                  /* store variable */
    lua_pop(L,1);                      /* pop .set table */
  }
}
/* Access const array
 * It reports an error when trying to write into a const array
 */
static int const_array (lua_State* L){
  luaL_error(L,"value of const array cannot be changed");
  return 0;
}
/* Map an array
 * It assigns an array into the current module (or class)
 */
TOLUA_API void tolua_array (lua_State* L, char* name, lua_CFunction get, lua_CFunction set){
  lua_pushstring(L,".get");
  lua_rawget(L,-2);
  if (!lua_istable(L,-1)) {
    /* create .get table, leaving it at the top */
    lua_pop(L,1);
    lua_newtable(L);
    lua_pushstring(L,".get");
    lua_pushvalue(L,-2);
    lua_rawset(L,-4);
  }
  lua_pushstring(L,name);
  
  lua_newtable(L);           /* create array metatable */
  lua_pushvalue(L,-1);
  lua_setmetatable(L,-2);    /* set the own table as metatable (for modules) */
  lua_pushstring(L,"__index");
  lua_pushcfunction(L,get);
  lua_rawset(L,-3);
  lua_pushstring(L,"__newindex");
  lua_pushcfunction(L,set?set:const_array);
  lua_rawset(L,-3);
  
  lua_rawset(L,-3);                  /* store variable */
  lua_pop(L,1);                      /* pop .get table */
}
TOLUA_API void tolua_dobuffer(lua_State* L, char* B, unsigned int size, const char* name) {
#ifdef LUA_VERSION_NUM /* lua 5.1 */
  int t=lua_gettop(L);
  //if(luaL_loadbuffer(L, B, size, name))for(;lua_gettop(L)>t;printf("%s",lua_tostring(L,-1)),lua_pop(L,1));
  //else if(lua_pcall(L, 0, 0, 0))for(;lua_gettop(L)>t;printf("%s",lua_tostring(L,-1)),lua_pop(L,1));
  if(luaL_loadbuffer(L, B, size, name)){
    printf("%s",lua_tostring(L,-1));
    lua_settop(L,t);
  }else{
    //if(lua_pcall(L, 0, 0, 0)){
    //  
    //}
    lua_call(L,0,0);
    if(lua_gettop(L)>t)printf("%s",lua_tostring(L,-1));
  }
#else
  lua_dobuffer(L, B, size, name);
#endif
};

