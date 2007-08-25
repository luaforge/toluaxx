require"base"
require"mproxy"

local t=test("Proxy")

local g=GROUP()
g.name="grp"
local o=OBJECT()
g.nod=o
t(o:fullname(),'grp.nod',"indexing `grp.nod` key exist")
for k,n in g do
   t(k,'nod',"only `nod` key exist")
   t(n,'OBJECT{name="nod"}',t.se,"`nod` value is OBJECT{name=\"nod\"}")
end
t(g:children(),'  nod=OBJECT{name="nod"}\n',"only nod=OBJECT{name=\"nod\"} exist")


local s=SCENE()
t(s,"SCENE",t.ti,"s is SCENE?")
t(s.mass,0,"s.mass is 0?")
t(s.live,0,"s.live is 0?")
t(s.color,"COLOR{self=1,emit=0}",t.se,"s.color is COLOR{self=1,emit=0}?")
local o=s.group.object
t(o,"OBJECT",t.ti,"o is OBJECT?")
t(o.mass,12.3,"o.mass is 12.3?")
t(o.live,2450,"o.live is 2410?")
t(o.color,"COLOR{self=14,emit=-9}",t.se,"o.color is COLOR{self=14,emit=-9}?")
local g=s.group
t(g,"GROUP",t.ti,"g is GROUP?")
t(g.mass,16.7,"g.mass is 16.7?")
t(g.live,2410,"g.live is 2410?")
t(g.color,"COLOR{self=1,emit=0}",t.se,"g.color is COLOR{self=1,emit=0}?")
t(s,"SCENE",t.ti,"s is SCENE?")
t(s.mass,0,"s.mass is 0?")
t(s.live,0,"s.live is 0?")
t(s.color,"COLOR{self=1,emit=0}",t.se,"s.color is COLOR{self=1,emit=0}?")

local o=s.group.object
o{
   live=4801,
   color=OBJECT.COLOR{
      emit=11,
      self=-3
   },
   mass=14.9,
}
t(o,"OBJECT",t.ti,"o is OBJECT?")
t(o.mass,14.9,"o.mass is 14.9?")
t(o.live,4801,"o.live is 4801?")
t(o.color,"COLOR{self=-3,emit=11}",t.se,"o.color is COLOR{self=-3,emit=11}?")

o.color{
   emit=1,
   self=3,
}
t(o.color.emit,1,"o.color.emit is 1?")
t(o.color.self,3,"o.color.self is 3?")


test()
