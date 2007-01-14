
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

assert(#sm==3)

assert(sm.null==nil)
assert(sm.one==sm.three)
assert(sm.one~=sm.two)

print("Test set/get with peer")
sm.num=3
assert(sm.num==3)

for i,o in sm do print("Entre:",i,o) end
print(sm,#sm)

print "STR_MAP{"
for i,v in pairs(getmetatable(sm)) do
  print("  [",i,"]=",v)
end
print "}"

print(collectgarbage("count"))

print("Index string test OK")


g2=TST()
--print(g2.o1,g2.o2,g2.o3,g2.o4)
assert(g2.o1==nil,g2.o2==nil,g2.o3==nil,g2.o4==nil)
assert(g2.o1==nil,g2.o2==nil,g2.o3==nil,g2.o4==nil)
g2.o1=OBJ()
--print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o2="Hello"
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o3=ELEM()
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o4=14
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o1=nil
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o2=nil
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o3=nil
print(g2.o1,g2.o2,g2.o3,g2.o4)
g2.o4=nil
print(g2.o1,g2.o2,g2.o3,g2.o4)

print("MultiType indexing test OK")

local m=MULTI()
m.one=12
m.two="help"
m.three=false
--print(m:is("one"),m:isnum("one"),m:isstr("one"),m:isbool("one"),m.one)
--print(m:is("two"),m:isnum("two"),m:isstr("two"),m:isbool("two"),m.two)
--print(m:is("three"),m:isnum("three"),m:isstr("three"),m:isbool("three"),m.three)
--assert(m:is("one") and m:isnum("one") and m.one==12)
--assert(m:is("one") and m:isnum("one") and m.one==12)
assert(m:is("two") and m:isstr("two") and m.two=="help")
assert(m:is("three") and m:isbool("three") and m.three==false)
m.two=m.one
--assert(m:is("two") and m:isnum("two") and not m:isstr("two") and m.two==12)
m.one=nil
--assert(not m:is("one") and not m:isnum("one") and m.one==nil)
m.three=true
assert(m:is("three") and m:isbool("three") and m.three==true)

print("MultiType2 indexing test OK")

print("All index tests passed....")


