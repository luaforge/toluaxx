require"base"
require"mdirective"

t=test("Directive")
t(a,3,"a == 3 ?")
t(A,4,"A == 4 ?")
t(func(),5,"func() == 5 ?")

test()

