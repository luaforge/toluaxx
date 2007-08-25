-- tolua: commandline help file
-- Written by PhoeniX11 Kayo
-- RareSky
-- Jul 2007
-- $Id: tlx_main.lua,v 1.4 2007-08-25 11:07:04 phoenix11 Exp $

-- This code is free software; you can redistribute it and/or modify it.
-- The software provided hereunder is on an "as is" basis, and
-- the author has no obligation to provide maintenance, support, updates,
-- enhancements, or modifications.


function _(s) return s end

function cmdline_process(a)
   local c=Cmd(a) -- Check command line arguments
   c("version",     {"-v","--version"       }, _("print version information"))
   c("author",      {"-d","--authors"       }, _("print developer information"))
   c("license",     {"-l","--license"       }, _("print license information"))
   c("luaversion",  {"-i","--lua-version"   }, _("print lua version information"))
   c("help",        {"-h","--help"          }, _("print this help page"))
   c("output",      {"-o","--output"        }, _("set output file (default is stdout)"), _("path/to/file"))
   c("header",      {"-H","--header"        }, _("create include header file"),          _("path/to/file"))
   c("pkgname",     {"-n","--pkgname"       }, _("set package name (default is input file name without extension and path)"), _("package_name"))
   c("include_path",{"-I","--include-path"  }, _("add the directory include_dir to the list of directories to be searched for ${c,h,l,p,i}file\"...\" files"), _("include_dir"))
   c("parseonly",   {"-p","--parse-only"    }, _("parse only and not produce c++ output (for debug)"))
   c("parseinfo",   {"-P","--parse-info"    }, _("parse and print structure information (for debug)"))
   c("maingen",     {"-m","--main-gen"      }, _("produce for build executble (int main(){..}, i.e.)"))
   c("nostdtring",  {"-S","--no-std-string" }, _("disable support for c++ strings"))
   c("subst1index", {"-1","--subst1index"   }, _("substract 1 to operator[] index (for compatibility with tolua5)"))
   c("luafile",     {"-L","--lua-file"      }, _("run lua file (with dofile()) before doing anything"), _("path/to/file"))
   c("noautodest",  {"-D","--no-auto-dest"  }, _("disable automatic exporting of destructors for classes that have constructors (for compatibility with tolua5)"))
   c("nowarnings",  {"-W","--no-warnings"   }, _("disable warnings for unsupported features (for compatibility with tolua5)"))
   c("nocleanup",   {"-C","--no-cleanup"    }, _("disable cleanup of included lua code (for easier debugging)"))
   c("luaembed",    {"-e","--lua-embed"     }, _("type of embedding algorithm:\n".."b|bc|bytecode - bytecode compilated code\n".."c|cc|charcode (optionaly) (be default)\n".."o|oc|origcode"), _("opcode"))
   c("autogen",     {"-a","--auto-gen"      }, _("automatically generated constructor, destructor, and sizeof function"))
   c("extraval",    {"-E","--extra"         }, _("add extra values to the luastate"), _("string_value"))
   c("typeidlist",  {"-t","--typeid-list"   }, _("export a list of types asociates with the C++ typeid name"))
   c(                                       )
   --protected_destructor?
   --
   --c:print()
   
   return c
end

function cmdline_postproc(c)
   _extra_parameters=_extra_parameters or {}
   flags=flags or {}
   
   if c.opt.extraval then
      for k,v in pairs(c.opt.extraval) do
	 _extra_parameters[k]=v
      end
   end
   if c.opt then
      for k,v in pairs(c.opt) do
	 flags[k]=v
      end
   end
end

function main(a)
   local c=cmdline_process(a)
   
   tolua.name=c.code
   tolua.lua_name=string.match(_VERSION,"%S-(.+)%s+")
   tolua.lua_version=string.match(_VERSION,"%s+([%d%.]+)%S-")
   
   for __,v in pairs(c.line) do -- Check input files
      c.opt.input=c.opt.input or {}
      c.opt.input[#c.opt.input+1]=v
   end
   
   for __,o in pairs(c.unrec) do -- Check unrecognized commandline arguments
      print(_("Warning: unrecognized option").." `"..o.."`..")
   end
   
   if c.opt.version then -- Check version query
      print(tolua.name.." "..tolua.version)
      return 0
   end

   if c.opt.author or c.opt.authors then -- Check author query
      print(tolua.name.._(" Written by").." "..tolua.authors)
      return 0
   end
   
   if c.opt.luaversion then -- Check lua version query
      print(tolua.lua_name.." "..tolua.lua_version)
      return 0
   end
   
   if c.opt.license then -- Check licension query
      print(tolua.name.." "..tolua.version.."\n\n"..tolua.license)
      return 0
   end
   
   if c.opt.help then -- Check help query
      print("\n".._("usage: ")..tolua.name.._(" [options] input_file").."\n\n"..
	    _("Command line options are:").."\n"..c:info().."\n"..
	       _("Should the input file be omitted, stdin is assumed;\nin that case, the package name must be explicitly set")..
	       "\n\n")
      return 0
   end
   
   if not c.opt.input then -- Check if input files not given
      print("\n"..tolua.name..": ".._("no input files").."\n  ".._("use -h|--help for more information").."\n\n")
      return 0
   end
   
   cmdline_postproc(c)

   local err,msg = xpcall(doit, debug.traceback)
   if not err then
      --print("**** msg is "..tostring(msg))
      local _,_,label,msg = strfind(msg,"(.-:.-:%s*)(.*)")
      tolua_error(msg,label)
   end
   
   return 0
end

function run()
   os.exit(main(arg))
end

run()