-- tolua.proxy test: load group
tolua.proxy.set("object_loadfunc",function()
   local o=GROUP()
   o.live=2452
   o.mass=15.7
   return o
end)
