#include "tproxy.hxx"

OBJECT::COLOR::operator string(){
  string r="COLOR{self=";
  {char b[25]; sprintf(b,"%d",self); r+=b;}
  r+=",emit=";
  {char b[25]; sprintf(b,"%d",emit); r+=b;}
  r+="}";
  return r;
}

OBJECT::operator string(){
  return string("OBJECT{name=\"")+name+string("\"}");
}

string OBJECT::fullname(string n){
  string r=name;
  if(n.length()>0){ if(r.length()>0)r+="."; r+=n; }
  if(parent){ r=parent->fullname(r); }
  return r;
}

GROUP::operator string(){
  return string("GROUP{name=\"")+name+string("\",children={\n")+children()+string("}}");
}

string GROUP::children(){
  GROUP&self=*this; string k,r; OBJECT*n;
  for(self(k,n);k!="";self(k,n)){
    r+="  "; r+=k; r+="="; r+=*n; r+=",\n";
  }
  if(r.length()>0)r.erase(r.length()-2);
  r+="\n";
  return r;
}

void GROUP::operator()(string&k,OBJECT*&n){
  map<string,OBJECT*>::iterator i;
  // iteration have been inited
  if(k.length()>0){ i=pool.find(k); if(i!=pool.end())i++; }
  else i=pool.begin();
  if(i==pool.end()){ k=""; n=NULL; }
  else{ k=i->first; n=i->second; }
}

void GROUP::setobject(string k, OBJECT* o){
  if(k.length()<=0)return;
  if(pool.find(k)!=pool.end()){
    pool[k]->name="";
    pool[k]->parent=NULL;
    pool.erase(k);
  }
  if(o){
    pool[k]=o;
    o->name=k;
    o->parent=this;
  }
}

#undef DEBUG_MODE
#undef DEBUG_STACK
#include<toluaxx.h>

OBJECT* GROUP::getobject(string k){
  if(k.length()<=0)return NULL;
  if(pool.find(k)==pool.end()){ // try load
    lua_State* L=tolua_state();
    DEBUG_STACK(inited stack);
    string file="iproxy."; file+=fullname(k); file+=".lua";
    if(luaL_loadfile(L,file.data())==LUA_ERRFILE)return NULL;
    DEBUG_STACK(load chunk);
    tolua_proxypush(L);
    DEBUG_STACK(tolua_proxypush: init new parameter stack level);
    lua_pcall(L,0,0,0);
    DEBUG_STACK(pcall loaded chunk);
    lua_pushstring(L,"object_loadfunc");
    DEBUG_STACK(push getting parameter name);
    tolua_getproxy(L,-1);
    DEBUG_STACK(tolua_getproxy: getting parameter);
    lua_pcall(L,0,1,0);
    DEBUG_STACK(pcall getted parameter for return object);
    tolua_pushusertype(L,this,"GROUP");
    //lua_pushvalue(L,1);
    lua_insert(L,-2);
    DEBUG_STACK(push self object as object group before returned object);
    tolua_pushstring(L,k.data());
    lua_insert(L,-2);
    DEBUG_STACK(push new object name before returned object);
    lua_settable(L,-3);
    DEBUG_STACK(set returned object in object group);
    lua_remove(L,-1);
    DEBUG_STACK(remove object group);
    tolua_proxypop(L);
    DEBUG_STACK(tolua_proxypop: delete current top parameter stack level);
  }
  if(pool.find(k)==pool.end())return NULL;else return pool[k];
}

SCENE::operator string(){
  return string("SCENE{")+string("children={\n")+children()+string("}}");
}
