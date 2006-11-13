
require"mnamespace"

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
assert(A.a==1)
assert(B.b==2)
assert(C.c==3)

-- test invalid access
assert(B.a==nil)      -- no inheritance
assert(C.a==nil)

assert(A.b==nil)        -- no access the inner module
assert(A.c==nil)
assert(B.c==nil)

assert(B.d[0]==0)
assert(B.d[1]==1)
assert(B.d[2]==2)
assert(B.d[3]==3)

print("Namespace test OK")
