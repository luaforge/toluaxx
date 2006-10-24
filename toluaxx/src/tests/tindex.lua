
require"mindex"

print(collectgarbage("count"))

local e=ELEM()
e:value(1,"test value")
print(e:value())
e:value(2,"test value 2",ELEM())
print(e:value())

print(#e,e())



local nm=NUM_MAP()
local e1,e2,e3=ELEM(),ELEM(),ELEM()

nm[1]=e1
nm[2]=e2
nm[3]=e1

assert(nm[4]==nil)
assert(nm[1]==nm[3])
assert(nm[1]~=nm[2])

print "NUM_MAP{"
for i,v in pairs(getmetatable(nm)) do
  print("  [",i,"]=",v)
end
print "}"

print(nm[1],nm[2],nm[3])

local i,v=nm()
print("nm:",i,v,":end")
local i,v=nm(i)
print("nm:",i,v,":end")
local i,v=nm(i)
print("nm:",i,v,":end")
local i,v=nm(i)
print("nm:",i,v,":end")

for i,o in nm do print("Entre:",i,o) end
print(nm,#nm)

print("Index number test OK")

local sm=STR_MAP()

sm.one=e1
sm.two=e2
sm.three=e1

assert(sm.null==nil)
assert(sm.one==sm.three)
assert(sm.one~=sm.two)

for i,o in sm do print("Entre:",i,o) end
print(sm,#sm)

print "STR_MAP{"
for i,v in pairs(getmetatable(sm)) do
  print("  [",i,"]=",v)
end
print "}"

print(collectgarbage("count"))

print("Index string test OK")

