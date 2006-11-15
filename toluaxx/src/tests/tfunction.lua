
require"mfunction"

local p1,p2=Point(2,3),Point(4,5)
--for i,v in pairs(Point) do
--  print("method: ",i,v)
--end
assert(p1^p2==Point(10,12))
assert(p1%p2==Point(7,7))
assert(p1..p2==4)
assert(#p1==5)
assert(-p1==Point(-2,-3))

local p = Point:new(1,2)
local q = Point:new()

local x, y = p:get()
local c = {}; q:get(c)

assert(x==1 and y==2)
assert(c[1]==0 and c[2]==0)

q:set(4,5)
q:setname("mypoint")
p:setconst(q)
local r = p:getpointer();
x,y = r:get()
assert(x==4 and y==5)
assert(r:getname()=="mypoint")

local s = Point:new()
s:setref(r)
local t = s:getref()
t:set(5,6)              -- t is s
x,y = s:get()
assert(x==5 and y==6)

local u = s:getvalue()
u:set(4,1)              -- u represents a copy of s
x,y = s:get()
assert(x~=4 and y~=1)

local x1,y1 = getpoint(u)
local x2,y2 = getpoint(s)

local p1 = alg.add(u,s)
x,y = p1:get()
assert(x==x1+x2 and y==y1+y2)

local p2 = alg.sub(u,s)
x,y = p2:get()
assert(x==x1-x2 and y==y1-y2)

local p3 = alg.mult(u,s)
x,y = p3:get()
assert(x==x1*x2 and y==y1*y2)

local p4 = alg.div(s,2)
x,y = p4:get()
assert(x==x2/2 and y==y2/2)

setpoint(p4)
x,y = p4:get()
assert(x==0 and y==0)

local cp = ColorPoint:MakeRed(3,4)
local p = cp+cp
x,y = p:get()
assert(x==6 and y==8)
local q = p-cp
x,y = q:get()
assert(x==3 and y==4)
local r = p/2
x,y = r:get()
assert(x==3 and y==4)
local t = r*cp
x,y = t:get()
assert(x==9 and y==16)
assert(t==r*r)
-- assert(q==cp)      -- the eq does not deal with different metatables
assert(t~=q)
assert(q<t)
assert(q<=t)
assert(t>q)
assert(t>=q)

local p = Point:new(1,2)
--error-- assert(p[1]==1 and p[2]==2)
p[1]=3; p[2] = p[2]+2
local x, y = p:get()
--error-- assert(x==3 and y==4)

local n = 3
local v = {Point:new(0,1), Point:new(2,3), Point:new(4,5)}

local m = average(n,v)
local c = averagepointer(n,v)
local t = {}
--error-- copyvector(n,v,t)

local l = Point:new()
for i=1,n do
--error-- assert(v[i]==t[i])
 l[1] = l[1] + v[i][1] 
 l[2] = l[2] + v[i][2] 
end
l = l/n
--error-- assert(m==l)
--error--assert(c==l)

assert(Point.SUCCESS==Point:echo(Point.SUCCESS))
assert(Point.ERROR==Point:echo(Point.ERROR))
assert(FIRST==invert(SECOND))
assert(SECOND==invert(FIRST))

--assert(Point(1,0)+Point.one==Point(2,1))
--assert(Point(0,0)==Point.null)

-- call lua method from c++
local o=Object()
local handler_called=false
function o:handler(period,time)
    self.time=period+time
    handler_called=true
end
local oldtime=o.time
o:step(10)
assert(handler_called==true,"handler not called!")
assert(o.time==oldtime+10)
assert(o()==nil)

assert(get_number(4)==4)
assert(get_number(0)==nil)
assert(get_number()==nil)

print("Function test OK")
