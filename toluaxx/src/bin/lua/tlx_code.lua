-- tolua: code class
-- Written by Waldemar Celes
-- TeCGraf/PUC-Rio
-- Jul 1999
-- $Id: tlx_code.lua,v 1.2 2007-07-23 18:57:29 phoenix11 Exp $

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications.

-- global
code_n = 1

-- Code class
-- Represents Lua code to be compiled and included
-- in the initialization function.
-- The following fields are stored:
--   text = text code
classCode = {
   text = '',
}
classCode.__index = classCode
setmetatable(classCode,classFeature)

-- register code
function classCode:register (pre)
   pre = pre or ''
   -- clean Lua code
   local s = clean(self.text)
   if not s then
      --print(self.text)
      error("parser error in embedded code")
   end

   -- get first line
   local _, _, first_line=string.find(self.text, "^([^\n\r]*)")
   if string.find(first_line, "^%s*%-%-") then
      if string.find(first_line, "^%-%-##") then
	 first_line = string.gsub(first_line, "^%-%-##", "")
	 if flags.nocleanup then
	    s = string.gsub(s, "^%-%-##[^\n\r]*\n", "")
	 end
      end
   else
      first_line = ""
   end

   -- convert to C
   local t={n=0}
   local b,bn,bt,bl,bi='','','','',''
   --local embed_macros='tolua_embeded'
   
   
   if     flags.luaembed and (flags.luaembed[1]=="bytecode" or
			      flags.luaembed[1]=="b" or flags.luaembed[1]=="bc") then
      bn=' (bytecode compiled)' bt='unsigned char'
   elseif flags.luaembed and (flags.luaembed[1]=="origcode" or
			      flags.luaembed[1]=="o" or flags.luaembed[1]=="oc") then
      bn=' (original source)' bt='unsigned char' bl='-1'
   else bn=' (charcode source)' bt='unsigned char' end
   output('\n'..pre..'{ /* begin embedded'..bn..' lua code */\n')
   output(pre..' int top = lua_gettop(tolua_S);')
   output(pre..' static '..bt..' B[] = {\n'..pre..' ')
   
   if     flags.luaembed and (flags.luaembed[1]=="bytecode" or
			      flags.luaembed[1]=="b" or flags.luaembed[1]=="bc") then
      s=string.dump(loadstring(s))
      b=' '..gsub(s,'(.)',function(c)
			     c=format('%x',strbyte(c))
			     if #c==1 then c='0'..c end
			     --if t.n%4==0 then c='0x'..c end
			     --if t.n%4==3 then c=c..',' end
			     local e='' t.n=t.n+1 if t.n==16 then t.n=0 e='\n'..pre..'  ' end
			     return '0x'..c..','..e
			  end)..'\n'
   elseif flags.luaembed and (flags.luaembed[1]=="origcode" or
			      flags.luaembed[1]=="o" or flags.luaembed[1]=="oc") then
      b=gsub(s,"([^\n]*)\n?",function(l)
			      return '"'..gsub(l,'(.)',function(c)
							  if     c=="\\" then c="\\\\"
							  elseif c=="\"" then c="\\\""
							  elseif c=="\'" then c="\\\'"
							  elseif c=="\n" then c=""
							  end
							  return c
						       end)..'\\n"\n'..pre..' '
			   end)
      --b=embed_macros..'((\n'..s..'\n))'
      --b=string.gsub(embed_macros)
      --bi='+2' bl='-4'
   else
      b=' '..gsub(s,'(.)',function(c)
			     local e='' t.n=t.n+1 if t.n==16 then t.n=0 e='\n'..pre..'  ' end
			     return format('%3u,%s',strbyte(c),e)
			  end)..'\n'
   end
   
   output(b)
   output(''..pre..' };\n')
   if first_line and first_line ~= "" then
      output(pre..' tolua_dobuffer(tolua_S,(char*)B'..bi..',sizeof(B)'..bl..',"tolua embedded'..bn..': '..first_line..'");')
   else
      output(pre..' tolua_dobuffer(tolua_S,(char*)B'..bi..',sizeof(B)'..bl..',"tolua: embedded'..bn..' Lua code '..code_n..'");')
   end
   output(pre..' lua_settop(tolua_S, top);')
   output(pre..'} /* end of embedded'..bn..' lua code */\n\n')
   code_n=code_n+1
end


-- Print method
function classCode:print (ident,close)
   print(ident.."Code{")
   print(ident.." text = [["..self.text.."]],")
   print(ident.."}"..close)
end


-- Internal constructor
function _Code (t)
   setmetatable(t,classCode)
   append(t)
   return t
end

-- Constructor
-- Expects a string representing the code text
function Code (l)
   return _Code {
      text = l
   }
end


