/* tolua
** Support code for Lua bindings.
** Written by Phoenix IV
** RareSky/Community
** Sep 2006
** $Id: parsecmd.cxx,v 1.1 2006-10-24 14:33:00 phoenix11 Exp $
*/

/* This code is free software; you can redistribute it and/or modify it.
** The software provided hereunder is on an "as is" basis, and
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications.
*/

#include"parsecmd.hxx"

string PARSECMD::next(){
  string re=iter->first;
  iter++; return re;
}
bool PARSECMD::begin(){
  iter=arg.begin();
  return arg.begin()!=arg.end();
}
bool PARSECMD::end(){
  return iter==arg.end();
}

PARSECMD::PARSECMD(int argc,char **argv):exec(arg["exec"]),line(arg["line"]){ // Parse cmd line
  exec.push_back(argv[0]);
  {
    int bt=exec[0].rfind("/");
    if(bt<exec[0].length()){
      exec.push_back(exec[0].substr(0,bt));
      exec.push_back(exec[0].substr(bt+1));
    }
  }
  for(unsigned int i=1;i<argc;i++)line.push_back(argv[i]);
}
bool PARSECMD::operator()(string name){
  if(arg.find(name)!=arg.end())return true;
  return false;
}
void PARSECMD::operator()(string name,string cmd,bool value){
  if(name==""||cmd=="")return;
  vector<string> cmdv;
  {
    unsigned int one,two;
    for(one=0,two=0;(two=cmd.find("|",one))<cmd.length();cmdv.push_back(cmd.substr(one,two-one)),one=two+1);
    cmdv.push_back(cmd.substr(one));
  }
  {
    unsigned int cmdi,wrdi;
    for(cmdi=0;cmdi<cmdv.size();cmdi++){
      for(wrdi=0;wrdi<line.size();wrdi++){
	unsigned int one,two=cmdv[cmdi].length();
	if((one=line[wrdi].find(cmdv[cmdi]))<two){
	  arg[name];
	  if(value){
	    if(two<line[wrdi].length()){
	      arg[name].push_back(line[wrdi].substr(one));
	    }else{
	      if(wrdi+1<line.size()){
	        arg[name].push_back(line[wrdi+1]);
	        line[wrdi+1]="";
	      }
	    }
	  }
	  line[wrdi]="";
	}
      }
    }
    for(int i=0;i<line.size();i++){
      if(line[i]==""){
	line.erase(line.begin()+i);
      }
    }
  }
}
vector<string>&PARSECMD::operator[](string name){
  //if(arg.find(name)!=arg.end())return arg[name];
  //return line;
  return arg[name];
}
string PARSECMD::cnstr(){
  string ret;
  for(ITER i=arg.begin();i!=arg.end();i++){
    ret+="arg["; ret+=(*i).first; ret+="]={";
    {
      int j;
      for(j=0;j<(*i).second.size();j++){
	char b[25]; sprintf(b,"%d",j);
	ret+="["+string(b)+"]=\""+(*i).second[j]+"\",";
      }
      if(j>0)ret.erase(ret.length()-1);
    }
    ret+="};\n";
  }
  return ret;
}
