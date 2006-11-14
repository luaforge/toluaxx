-- tolua.proxy test: load object
tolua.proxy.set("object_loadfunc",function()
   local o=OBJECT()
   o.live=2450
   o.mass=12.3
   return o
end)
