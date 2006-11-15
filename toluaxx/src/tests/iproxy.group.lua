-- tolua.proxy test: load group
tolua.proxy.set("object_loadfunc",function()
   --local o=GROUP()
   --o.live=2452
   --o.mass=15.7
   --return o
   return GROUP{
     intern=OBJECT(),
     mass=16.7,
     live=2410
   }
end)
