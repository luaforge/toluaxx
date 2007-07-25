require"base"
require"marray"

init()

local t=test("Array test")
--local t=tolua.test("","Array test","")

for i=0,9 do
   t(a[i],i,"a[i]==i i="..tostring(i))
end

for i=1,9 do
   t(p[i-1].y,p[i].x,"p[i-1].y==p[i].x i="..tostring(i))
end

for i=0,9 do
   t(M.a[i],i,"M.a[i]==i i="..tostring(i))
end

for i=1,9 do
   t(M.p[i-1].y,M.p[i].x,"M.p[i-1].y==M.p[i].x i="..tostring(i))
end

for i=0,9 do
   t(pp[i].x,M.p[i].x,"pp[i].x==M.p[i].x i="..tostring(i))
   t(p[i].y,M.pp[i].y,"p[i].y==M.pp[i].y i="..tostring(i))
end

for i=0,9 do
   t(array.a[i],parray.a[i],"array.a[i]==parray.a[i] i="..tostring(i))
   t(array.p[i].x,parray.pp[i].x,"array.p[i].x==parray.pp[i].x i="..tostring(i))
   t(array.p[i].y,parray.pp[i].y,"array.p[i].y==parray.pp[i].y i="..tostring(i))
end

for i=0,9 do
   array.a[i]=a[9-i]
   M.a[i]=9-i
   t(array.a[i],M.a[i],"array.a[i]==M.a[i] i="..tostring(i))
end

for i=1,9 do
   array.p[i] = array.pp[0]
   t(array.p[i].x,0,"array.p[i].x==0 i="..tostring(i))
   t(array.p[i].y,1,"array.p[i].y==1 i="..tostring(i))
end

--t:log()
test()
--print(t.result)
--print(t.errors)
--print(t.report)

