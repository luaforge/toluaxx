require"base"
require"mgcind"

SetAllocHook()

local t=test("Garbage collecting indexed")

t(#tolua.watch.object,1,"Num of objects must be 1")
t(#tolua.watch.objenv,1,"Num of objenvs must be 1")

local g1=GRP()
t(g1,"GRP",t.ti,"local g1=GRP()")

t(#tolua.watch.object,2,"Num of objects must be 2")
t(#tolua.watch.objenv,1,"Num of objenvs must be 2")

local o1=OBJ()
t(o1,"OBJ",t.ti,"local o1=OBJ()")

t(#tolua.watch.object,3,"Num of objects must be 3")
t(#tolua.watch.objenv,1,"Num of objenvs must be 2")

t(tolua.watch.object[o1],true,"o1 exist")
t(tolua.watch.object[o2],nil,"o2 not exist")
t(tolua.watch.object[g1],true,"g1 exist")
t(tolua.watch.object[g2],nil,"g2 not exist")
t(tolua.watch.object[t],true,"t exist")

g1.o1=o1
t(g1.o1,o1,"g1.o1=o1")

--for i,o in tolua.watch.object do
--   print(i,o)
--end

for i,o in tolua.watch.object do
   t(i,l,t.oo,"Object `"..tostring(i).."` watched")
end

g1.o2=OBJ()
t(g1.o2,"OBJ",t.ti,"g1.o2=OBJ()")

collectgarbage"collect"
--t(collectgarbage"count",38.748046875,"Allocated memory 38.748046875 Kb")

for i=0,100 do
  collectgarbage"collect"
  --t(collectgarbage"count",38.748046875,"Allocated memory 38.748046875 Kb")
end

collectgarbage"collect"
print(collectgarbage"count",tolua_alloc_count)

g1.o1=nil
t(g1.o1,nil,"g1.o1=nil")

collectgarbage"collect"
print(collectgarbage"count",tolua_alloc_count)

g1.o2=nil
t(g1.o2,nil,"g1.o2=nil")

collectgarbage"collect"
print(collectgarbage"count",tolua_alloc_count)

function test_collector(msg)
   print(msg,"\n","objects #",#tolua.watch.object,"envs #",#tolua.watch.objenv,"GCcount:",collectgarbage"count","Kb","TLcount:",tolua_alloc_count/1024,"Kb")
end

local M,N=10,1000

test_collector("Before creating")
print("create",N,"objects (operation may be take more time, please wait)....")
for j=1,M do
   --print("Collector test cycle",j)
   collectgarbage"stop"
   for i=1,N do
      local o=OBJ()
      t(tolua.watch.object[o],true,"local o=OBJ()")
   end
   --test_collector("After creating")
   --collectgarbage"restart"
   collectgarbage"collect"
end
test_collector("After collecting")

local M,N=10,1000
print("Collector test cycle",j)
collectgarbage"stop"
test_collector("Before creating")
print("create",N,"objects (operation may be take more time, please wait)....")
for j=1,M do
   local n=18
   local d="safjsnfnsdfsdf"
   for i=1,N do
      local o={n=18,d="safjsnfnsdfsdf"}
      --t(tolua.watch.object[o],true,"local o={}")
   end
   local k={n=18,d="safjsnfnsdfsdf"}
end
test_collector("After creating")
--collectgarbage"restart"
collectgarbage"collect"
test_collector("After collecting")


local M,N=100,100000
print("Collector test cycle",j)
collectgarbage"stop"
test_collector("Before creating")
print("create",N,"objects (operation may be take more time, please wait)....")
for j=1,M do
   for i=1,N do
      CreateUserdata()
      --t(tolua.watch.object[o],true,"local o={}")
   end
end
test_collector("After creating")
--collectgarbage"restart"
collectgarbage"collect"
test_collector("After collecting")


test()
