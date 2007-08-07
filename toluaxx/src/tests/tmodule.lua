require"base"
require"mmodule"

local t=test("Module")
-- test valid access
t(A.a,1,"A.a==1")
t(A.B.b,2,"A.B.b==2")
t(A.B.C.c,3,"A.B.C.c==3")

-- test invalid access
t(A.B.a,nil,"A.B.a==nil")      -- no inheritance
t(A.B.C.a,nil,"A.B.C.a==nil")

t(A.b,nil,"A.b==nil")        -- no access the inner module
t(A.c,nil,"A.c==nil")
t(A.B.c,nil,"A.B.c==nil")

-- test variables appended to existing modules
t(A.d,4,"A.d==4")

test()
