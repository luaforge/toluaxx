require"base"
require"mdirective"

t=test("Directive")

t(a,3,"embedded cxx code $< .... $>")
t(A,4,"embedded lua code $[ .... $]")
t(func(),5,"$lfile")
t(d,13,"$hfile")

test()

