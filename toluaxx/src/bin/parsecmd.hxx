/* tolua
** Support code for Lua bindings.
** Written by Phoenix IV
** RareSky/Community
** Sep 2006
** $Id: parsecmd.hxx,v 1.1 2006-10-24 14:33:01 phoenix11 Exp $
*/

/* This code is free software; you can redistribute it and/or modify it.
** The software provided hereunder is on an "as is" basis, and
** the author has no obligation to provide maintenance, support, updates,
** enhancements, or modifications.
*/

#pragma once
#include"platform.hxx"
#include<map>

class PARSECMD{
 public:
  typedef vector<string> CORT;
  typedef map<string,CORT> POOL;
  typedef map<string,CORT>::iterator ITER;
 protected:
  POOL arg;
  CORT&line;
  CORT&exec;
  ITER iter;
 public:
  PARSECMD(int argc,char **argv); // Parse cmd line
  bool operator()(string name); // Test existing arg
  void operator()(string name, string argum, bool value=false); // Parse arg
  CORT&operator[](string name); // Get arg value
  string cnstr();
  string next();
  bool begin();
  bool end();
  inline void operator--(){begin();}
  inline string operator++(){return next();}
  inline bool operator!(){return !end();}
};

