#include "tproxy.hxx"

OBJECT::operator string(){
  return string("OBJECT{name=\"")+name+string("\")");
}

SCENE::operator string(){
  return string("SCENE{name=\"")+name+string("\")");
}

#include<toluaxx.h>

OBJECT* SCENE::getobject(string name){
  if(name.length()<=0)return NULL;
  if(pool.find(name)==pool.end()){ // try load
    lua_State* L=tolua_state();
    string file="i"; file+=name; file+=".lua";
    if(luaL_loadfile(L,file.data())==LUA_ERRFILE)return NULL;
    tolua_proxypush(L);
    lua_pcall(L,0,0,0);
    lua_pushstring(L,"object_loadfunc");
    tolua_getproxy(L,-1);
    lua_pcall(L,0,1,0);
    tolua_pushusertype(L,this,"SCENE");
    lua_insert(L,-2);
    tolua_pushstring(L,name.data());
    lua_insert(L,-2);
    lua_settable(L,-3);
    tolua_proxypop(L);
  }
  if(pool.find(name)==pool.end())return NULL;else return pool[name];
}

void SCENE::setobject(string name, OBJECT* obj){
  if(name.length()<=0)return;
  if(pool.find(name)!=pool.end()){
    pool[name]->name="noname";
    pool[name]->parent=NULL;
    pool.erase(name);
  }
  if(obj){
    pool[name]=obj;
    obj->name=name;
    obj->parent=this;
  }
}
