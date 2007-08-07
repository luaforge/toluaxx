require"base"
require"mclass"

init()

local t=test("Class :: rawset methods")
a = {}
rawset(a, ".c_instance", "something")

function hello()
   return "Hello World!"
end

rawset(Test.B,"hello",hello)

local b=Test.B()
t(b:hello(),"Hello World!","call changed method")


local t=test("Class :: type conversion") -- type convertion tests
local a=Test.A:new()
t(tolua.type(Test.A.last),'Test::Tst_A',"type convertion: same type") -- no type convertion: same type
local b=Test.B:new()
t(tolua.type(Test.A.last),'Test::Tst_B',"convertion: obj already mapped as B") -- no convertion: obj already mapped as B
local c=Test.luaC:new(0)
t(tolua.type(Test.A.last),'Test::Tst_C',"convertion: obj already mapped as C") -- no convertion: obj already mapped as C
t(tolua.type(Test.luaC.last),'Test::Tst_C',"convertion: obj already mapped as C")


local aa = Test.A.AA:new()
local bb = Test.A.BB:new()
local xx = Test.create_aa()

local t=test("Class :: method calling") -- method calling tests
t(a:a(),'A',"call a:a()")
t(b:a(),'A',"call b:a()")
t(b:b(),'B',"call b:b()")
t(c:a(),'A',"call c:a()")
t(c:b(),'B',"call c:b()")
t(c:c(),'C',"call c:c()")
t(aa:aa(),'AA',"call aa:aa()")
t(bb:aa(),bb:Base():aa(),"call bb:aa() and bb:Base():aa()")
t(xx:aa(),'AA',"call xx:aa()")
t(Test.is_aa(bb),true,"Test bb is aa")


local t=test("Class :: ownershipping handling") -- test ownershipping handling
tolua.proxy.push()
-- should delete objects: 6 7 8 9 10 (it may vary!)
local set = {}
for i=1,10 do
   local c=Test.luaC:new(i)
   if i>5 then
      tolua.takeownership(c)
   end
   --set[i] = c
end
for i=6,9 do t(tolua.proxy.get("deleting C: "..tostring(i)),true,"deleting C: "..tostring(i)) end
tolua.proxy.pop()

local t=test("Class :: type getting")

local e=Test.B:new_local()
t(tolua.type(e),"Test::Tst_B","e is type Test::B")
t(tolua.type(ae),"nil","ae is type nil")
e=nil --e:delete()
t(tolua.type(e),"nil","e is not type Test::B")


----------
local t=test("Class :: outside method & static")

tolua.proxy.push()
local out=Test.Outside:new_local()
out:outside()
t(tolua.proxy.get("flag"),"method","is outside method?")
Test.Outside:outside_static(nil)
t(tolua.proxy.get("flag"),"static","is outside static?")
tolua.proxy.pop()


----- casting
local t=test("Class :: type() casting")
local acast=Test.B:create_void()
t(tolua.type(acast),"userdata","acast is void userdata?")
local bcast=tolua.cast(acast,"Test::Tst_B")
t(tostring(bcast):find("userdata: "),1,"bcast is userdata?")
t(tolua.type(bcast),"Test::Tst_B","bcast's type is Test::Tst_B?")
t(bcast:b(),"B","bcast:b() is B?")


-- test properies
local t=test("Class :: properties")

local n=7
a.number=n
t(a.number,n*2,"a.number==7*2")

local c=Test.luaC(0)
c{number=11,i=18}
t(c.number+c.i,40,"Reinit properties")

Test.A.pete = {}
table.insert(Test.A.pete, a)


-- constructors
local t=test("Class :: constructors")

t(getmetatable(Test.A),getmetatable(Test.E),"metatable Test.A) == metatable Test.E")
t(tostring(getmetatable(Test.B)),"userdata: 0x0","metatable Test.B == 0x0")

local a=Test.A()
local b=Test.B()
local e=Test.E(5)

t(tostring(getmetatable(Test.B).__call):find("function: "),1,"getmetatable(Test.B).__call is function?")
t(tostring(Test.B.__call):find("function: "),1,"Test.B.__call is function?")
t(tostring(Test.B.__call(Test.B)):find("userdata: "),1,"Test.B.__call(Test.B) is userdata?")
t(tolua.type(b),"Test::Tst_B","type b is Test::Tst_B?")

tolua.proxy.push()
e:set_ptr(e)
t(tolua.proxy.get("this"),tolua.proxy.get("ptr"),"this == ptr?")
local ve=tolua.cast(e:get_ptr(),"Test::Tst_E")
ve:set_ptr(ve)
t(tolua.proxy.get("this"),tolua.proxy.get("ptr"),"this == ptr?")
tolua.proxy.pop()


for i=1,100000 do
   la={}
   tolua.inherit(la,a)
end

dest()

test()
