classCmd = {}
classCmd.__index = classCmd

function classCmd:print()
   print("argc="..tostring(#self.arg))
   print("argv={")
   for k,v in pairs(self.arg) do
      local ic=""
      if self:iscast(k) then ic="[cast]" end
      print("  "..tostring(k).."="..tostring(v).." "..ic)
   end
   print("}")
   print("exec="..self.exec)
   print("code="..self.code)
   print("opt={")
   for k,v in pairs(self.opt) do
      if type(v)=="table" then
	 print("  "..tostring(k).."={")
	 for k,v in pairs(v) do print("    "..tostring(k).."="..tostring(v)) end
	 print("  }")
      else
	 print("  "..tostring(k).."="..tostring(v))
      end
   end
   print("}")
   print("line={")
   for k,v in pairs(self.line) do print("  "..tostring(k).."="..tostring(v)) end
   print("}")
   print("unrec={")
   for k,v in pairs(self.unrec) do print("  "..tostring(k).."="..tostring(v)) end
   print("}")
end

function classCmd:construct()
   self.exec=self.arg[-1]
   self.code=self.arg[0]
   self.arg[-1]=nil
   self.arg[0]=nil
   self.opt={}
   self.__casted={}
   self.line={}
   self.unrec={}
end

function classCmd:cast(param,names,value)
   for i,v in pairs(self.arg) do
      if not self:iscast(i) then
	 for _,c in pairs(names) do
	    _c=c
	    for _,magic in pairs{'^','$','(',')','%','.','[',']','*','+','-','?'} do 
	       _c=string.gsub(_c,"%"..magic,"%%"..magic)
	    end
	    if string.find(v,_c)==1 then
	       if value then
		  self:casted(i)
		  if #v>#c then
		     local s=#c+1
		     while string.sub(v,s,s)=="=" do s=s+1 end
		     self.opt[param]=self.opt[param]or{}
		     self.opt[param][#self.opt[param]+1]=string.sub(v,s)
		  else
		     local n=i
		     while self:iscast(n) and n<=#self.arg do
			n=n+1
		     end
		     if self.arg[n] then
			self:casted(n)
			self.opt[param]=self.opt[param]or{}
			self.opt[param][#self.opt[param]+1]=self.arg[n]
		     end
		  end
	       else
		  if #v==#c then
		     self:casted(i)
		     self.opt[param]=true
		  end
	       end
	    end
	 end
      end
   end
end

function classCmd:casted(i)
   self.__casted[#self.__casted+1]=i
end

function classCmd:iscast(i)
   for _,v in pairs(self.__casted) do
      if i==v then return true end
   end 
   return false 
end

function classCmd:final()
   for i,v in pairs(self.arg) do
      if not self:iscast(i) then
	 if string.sub(v,1,1)=="-" then
	    self.unrec=self.unrec or {}
	    self.unrec[#self.unrec+1]=v
	 else
	    self.line[#self.line+1]=v
	 end
      end
   end
end

function classCmd:__call(name,options,description,values)
   -- param - internal name of parameter  (for example: output)
   -- names - external names of parameter (for example: {-o,--out,--output})
   -- value - default value of parameter
   --         available values: nil,bool (novalue) - no value exist (true) or noexist (false))
   --                           string - default value for valuated parameters
   if not (name or options or values) then
      return self:final()
   end
   if not (options or values) and type(name)=="string" then
      return self.opt[name]
   end
   if description then
      self:info(options,values,description)
   end
   return self:cast(name,options,values)
end

function classCmd:info(options,values,description)
   if options then -- compile help
      self.__info=self.__info or {}
      local i={
	 --o="", -- options
	 --v="", -- values
	 d=description
      }
      self.__info[#self.__info+1]=i
      if type(options)=="table" then
	 for _,o in pairs(options) do
	    if i.o then i.o=i.o.."|"..o
	    else i.o=o end
	 end
      elseif type(options)=="string" then
	 i.o=options
      else
	 i.o=""
      end
      if type(values)=="table" then
	 i.v="[| |=]"
	 for _,o in pairs(values) do
	    if i.v then i.v=i.v.."|"..o
	    else i.v=o end
	 end
      elseif type(values)=="string" then
	 i.v="[| |=]"..values
      else
	 i.v=""
      end
   else -- put help
      local s=""
      local l={ -- length structure
	 o=0,
	 v=0,
	 d=0
      }
      local function strt(l,s) local s=s or"" if l<#s then l=#s end return l end
      for _,i in pairs(self.__info) do -- quantify
	 l.o=strt(l.o,i.o)
	 l.v=strt(l.v,i.v)
	 l.d=strt(l.d,i.d)
      end
      if self.__info then
	 local tl,ti=86,5
	 local function adds(l,s) local s=s or"" return s..string.rep(" ",l-#s) end
	 for _,i in pairs(self.__info) do
	    s=s.."  "..adds(l.o,i.o)..""..adds(l.v,i.v).." : "..string.gsub(i.d,"\n","\n"..adds(l.o+l.v+ti,"")).."\n"
	    --s=s.."  "..i.o..""..i.v.." : "..i.d.."\n"
	 end
	 local li=1
	 while true do
	    local ni=string.find(s,"\n",li,true)
	    if not ni then break end
	    if ni-li>tl then
	       ni=li+tl
	       s=string.sub(s,1,ni).."\n"..adds(l.o+l.v+ti,"")..string.sub(s,ni+1)
	       li=li+tl+1
	    else
	       s=string.sub(s,1,ni)..adds(0*(l.o+l.v+ti),"")..string.sub(s,ni+1)
	       li=li+tl+1
	    end
	 end
      end
      return s
   end
end

-- Internal constructor
function _Cmd (t)
   setmetatable(t,classCmd)
   --append(t)
   t:construct()
   return t
end

-- Constructor
-- Expects a string representing the variable declaration.
function Cmd (a)
   return _Cmd{arg=a}
end

if classCmd.debug then
   c=Cmd(arg)
   c("test",{"-t","--test","--test-mode"})
   c("param",{"-p","--param"},true)
   c("output",{"-o"})
   c()
   
   c:print()
end
