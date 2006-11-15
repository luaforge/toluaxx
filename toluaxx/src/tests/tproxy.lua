
require"mproxy"

local g=GROUP()
g.name="grp"
local o=OBJECT()
g.nod=o
print(o:fullname())
print("g{")
for k,n in g do
  print(k,"=",n)
end
print("}g")
print(g:children())

--os.exit()

local s=SCENE()
print(s,s.mass,s.live,s.color)
local o=s.group.object
print(o,o.mass,o.live,o.color)
local g=s.group
print(g,g.mass,g.live,g.color)
print(s,s.mass,s.live,s.color)


print("Proxy test OK")
