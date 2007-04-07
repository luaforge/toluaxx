
require"mgcind"

local g1=GRP()
print("local g1=GRP()",g1)

local o1=OBJ()
print("local o1=OBJ()",o1)

g1.o1=o1
print("g1.o1=o1",g1.o1)

g1.o2=OBJ()
print("g1.o2=OBJ()",g1.o2)

collectgarbage"collect"
print(collectgarbage"count")

for i=0,100 do
  collectgarbage"collect"
  print(collectgarbage"count")
end

g1.o1=nil
print("g1.o1=nil",g1.o1)

collectgarbage"collect"
print(collectgarbage"count")

g1.o2=nil
print("g1.o2=nil",g1.o2)

collectgarbage"collect"
print(collectgarbage"count")

print("OK")


