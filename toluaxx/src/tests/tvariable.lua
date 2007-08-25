require"base"
require"mvariable"

local t=test("Variable")

init()

t(i,1,"get int variable")
t(f,2,"get float variable")
t(d,3,"get double variable")
t(s,"Hello world","get char* variable")
t(n,"Hi there","get std::string variable")
n="Hello"
t(n,"Hello","set std::string variable")

t(a.i,11,"get int from struct")
t(a.f,12,"get float from struct")
t(a.d,13,"get double from struct")
t(a.s,"Hello world from class","get char* from struct")
t(a.n,"Hi there from class","get std::string from struct")
a.n="Hello from class"
t(a.n,"Hello from class","set std::string in struct")

t(v,a.v,"check void* pointer")

u.i=2
t(u.i,2,"set int in union")
u.f=2
t(u.f,2,"set float in union")
t(u.i,2,t.neq,"check (int ~= float) in union")

t(M.mi,21,"get int from module")
t(M.mf,22,"get float from module")
t(M.md,23,"get double from module")
t(M.ms,"Hello world in module","get char* from module")
t(M.mn,"Hi there in module","get std::string from module")
M.mn="Hello in module"
t(M.mn,"Hello in module","set std::string in module")
t(M.mv,nil,"check void* in module")

t(M.ma.i,31,"get int from module struct")
t(M.ma.f,32,"get float from module struct")
t(M.ma.d,33,"get double from module struct")
t(M.ma.s,"Hello world from class in module","get char* from module struct")
t(M.ma.n,"Hi there from class in module","get std::string from module struct")
M.ma.n="Hello from class in module"
t(M.ma.n,"Hello from class in module","set std::string in module struct")
t(M.ma.v,nil,"check void* in module struct")

t(a.i,b.a.i,"get int from struct pointer")
t(a.f,b.a.f,"get float from struct pointer")
t(a.d,b.a.d,"get double from struct pointer")
t(a.s,b.a.s,"get char* from struct pointer")
t(a.v,b.a.v,"check void* in struct pointer")
t(b.b,nil,"check next struct pointer in struct pointer")

t(M.ma.i,M.mb.a.i,"get int from struct pointer in module")
t(M.ma.f,M.mb.a.f,"get float from struct pointer in module")
t(M.ma.d,M.mb.a.d,"get double from struct pointer in module")
t(M.ma.s,M.mb.a.s,"get char* from struct pointer in module")
t(M.ma.v,M.mb.a.v,"check void* in struct pointer in module")

t(a.i,M.mb.b.a.i,"get int from next struct pointer in module")
t(a.f,M.mb.b.a.f,"get float from next struct pointer in module")
t(a.d,M.mb.b.a.d,"get double from next struct pointer in module")
t(a.s,M.mb.b.a.s,"get char* from next struct pointer in module")
t(a.v,M.mb.b.a.v,"check void* in next struct pointer in module")
t(M.mb.b.b,nil,"check next struct pointer in struct pointer in module")

t(s,rawget(_G,"s"),t.tneq,"because s represents a C variable")
s="Hello"
t(s,rawget(_G,"s"),t.tneq,"because s is mapped as const")

f=25.0
t(f,rawget(_G,"f"),t.tneq,"because f represents a C variable")

b.a.i=5
t(b.a.i,M.mb.b.a.i,"set int through struct pointer")

test()
