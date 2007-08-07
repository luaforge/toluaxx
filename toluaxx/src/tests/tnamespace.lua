require"base"
require"mnamespace"

local t=test("Namespace")
-- test valid access
--assert(A.a==1)
--assert(A.B.b==2)
--assert(A.B.C.c==3)

-- test invalid access
--assert(A.B.a==nil)      -- no inheritance
--assert(A.B.C.a==nil)

--assert(A.b==nil)        -- no access the inner module
--assert(A.c==nil)
--assert(A.B.c==nil)

-- test valid access
t(A.a,1,"A.a==1")
t(B.b,2,"B.b==2")
t(C.c,3,"C.c==3")

-- test invalid access
t(B.a,nil,"B.a==nil")      -- no inheritance
t(C.a,nil,"C.a==nil")

t(A.b,nil,"A.b==nil")        -- no access the inner module
t(A.c,nil,"A.c==nil")
t(B.c,nil,"B.c==nil")

t(B.d[0],0,"B.d[0]==0")
t(B.d[1],1,"B.d[1]==1")
t(B.d[2],2,"B.d[2]==2")
t(B.d[3],3,"B.d[3]==3")

test()
