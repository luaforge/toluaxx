require"base"
require"mconst"

local t=test("Const test")

t(FIRST,M.FIRST,"FIRST==M.FIRST")
t(FIRST,A.FIRST,"FIRST==A.FIRST")
t(SECOND,M.SECOND,"SECOND==M.SECOND")
t(SECOND,A.SECOND,"SECOND==A.SECOND")

t(ONE,M.ONE,"ONE==M.ONE")
t(ONE,A.ONE,"ONE==A.ONE")
t(TWO,M.TWO,"TWO==M.TWO")
t(TWO,A.TWO,"TWO==A.TWO")

test()
