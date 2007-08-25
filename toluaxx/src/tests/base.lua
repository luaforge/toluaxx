test={
   package="toluaxx",
   description="basic tests",
   author="PhoeniX11 Kayo <phoenix11@users.luaforge.net>",
}
setmetatable(test,{
		__call=function(self,description)
			  if description then
			     self.__tests=self.__tests or {}
			     local t=tolua.test(self.package,description,self.author)
			     t.log=function(self)
				      print(self)
				   end
			     self.__tests[#self.__tests+1]=t
			     --print(description,#self.__tests)
			     return t
			  else
			     print(self)
			     os.exit(0)
			  end
		       end,
		__tostring=function(self)
			      local s=self.package.." :: "..self.description.." # ["..tostring(#self.__tests).."] .."
			      for i=1,#self.__tests do
				 local t=self.__tests[i]
				 --if not s then
				 --   s=tostring(t)
				 --else
				 s=s.."\n "..tostring(t)
				 --end
			      end
			      return s
			   end
	     })
tableexplore=function(table)
		if not table then return "nil" end
		local s=""
		for i,v in pairs(table) do
		   s=s.."[`"..tostring(i).."`]=`"..tostring(v).."`\n"
		end
		return s
	     end
tablesize=function(table)
	     if not table then return "nil" end
	     local n=0
	     for _ in pairs(table) do n=n+1 end
	     return n
	  end