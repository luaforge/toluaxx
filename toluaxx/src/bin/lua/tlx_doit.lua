-- Generate binding code
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 1998
-- Last update: Apr 2003
-- $Id: tlx_doit.lua,v 1.3 2007-07-23 18:57:29 phoenix11 Exp $


-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications.

function parse_extra()
   for k,v in ipairs(_extra_parameters or {}) do
      local b,e,name,value = string.find(v, "^([^=])=(.*)$")
      if b then
	 _extra_parameters[name] = value
      else
	 _extra_parameters[v] = true
      end
   end
end

function doit ()
   -- define package name, if not provided
   flags.pkgname=flags.pkgname or {}
   if not flags.pkgname[1] then
      if flags.input then
	 flags.pkgname[1] = gsub(flags.input[1],"%..*$","")
	 _,_,flags.pkgname[1] = string.find(flags.pkgname[1], "([^/\\]*)$")
      else
	 error("#no package name nor input file provided")
      end
   end
   
   -- parse table with extra paramters
   parse_extra()
   
   -- do this after setting the package name
   if flags.luafile then
      dofile(flags.luafile[1])
   end
   
   -- add cppstring
   if not flags.nostdstring then
      _basic['string'] = 'cppstring'
      _basic['std::string'] = 'cppstring'
      _basic_ctype.cppstring = 'const char*'
      --_basic_ctype.cppstring = 'string'
   end
   
   -- proccess package
   local p = Package(flags.pkgname[1],flags.input)
   
   if flags.parseonly then -- only parse
      return
   end
   
   if flags.output then
      local st,msg = writeto(flags.output[1])
      if not st then
	 error('#'..msg)
      end
   end
   
   p:decltype()
   if flags.parseinfo then
      p:print()
   else
      p:preamble()
      p:supcode()
      p:register()
      push(p)
      post_output_hook(p)
      pop()
   end
   
   if flags.output then
      writeto()
   end
   
   -- write header file
   if not flags.parseinfo then
      if flags.header then
	 local st,msg = writeto(flags.header[1])
	 if not st then
	    error('#'..msg)
	 end
	 p:header()
	 writeto()
      end
   end
end

