--- 
--- Platform independent realisation
--- 
do
   local so__model=string.find(package.cpath,"?.so")~=nil
   local dll_model=string.find(package.cpath,"?.dll")~=nil

   local ext=""
   if     so__model then ext="so"
   elseif dll_model then ext="dll"
   else assert(false,"FATAL ERROR: System module model has not been detected correctly....")
   end

   package.cpath=package.cpath..";lib/?."..ext
end
