local t2=test("Main emulation test #2")
t2(#arg,3,"#arg==3")
t2(arg[1],"-o2","arg[1]==\"-o2\"")
t2(arg[2],"v2","arg[2]==\"v2\"")
t2(arg[3],"file","arg[3]==\"file\"")

local t3=test("AutoGen methods")
local agt=AutoGenTest()
t3(agt,"AutoGenTest",t3.ti,"Constructor passed")

test()
