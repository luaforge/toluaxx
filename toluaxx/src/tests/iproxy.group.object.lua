-- tolua.proxy test: load object
tolua.proxy.set("object_loadfunc",function()
--   local o=OBJECT()
--   o.live=2450
--   o.mass=12.3
--   return o
   return OBJECT{
     live=2450,
     mass=12.3,
     color={
       self=14,
       emit=-9
     }
   }
end)
