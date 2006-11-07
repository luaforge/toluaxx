##                                    ##
##  Copyright (C) 2006 by PhoeniX IV  ##
##                                    ##
##  RareSky Community 2012            ##
##                                    ##
##             Platform GNU Makefile  ##
##                                    ##

##
## Enhanced build system main
##

all: prepare build

include config

ifdef platform
PLATFORM=$(platform)
endif
ifdef compiler
COMPILER=$(compiler)
endif
ifdef mode
MODE=$(mode)
endif
ifdef debug
DEBUG=$(debug)
endif
ifdef prefix
PREFIX=$(prefix)
endif

-include opt/$(PLATFORM)-$(COMPILER).local
include opt/$(PLATFORM)-$(COMPILER)


PREFIX.bin=bin
PREFIX.include=include
PREFIX.lib=lib

TARGET.MODS=$(patsubst src/%/config,%,$(wildcard src/*/config))

# Full name
FNAME=$(NAME)$(VERSION)

ifeq ($(MODE),test)                # Test builder mode
  EXEC=$(ECHO) "[shell:]"
else
  
endif
ifeq ($(DEBUG),enable)             # Debug mode
  CFLAGS+=$(CFLAGS.DEBUG)
  BIND=disable
else
  CFLAGS+=$(CFLAGS.RELEASE)
  BIND=enable
endif
ifdef CFLAGS.I                     # Add global include path
  ifneq ($(CFLAGS.I),)
    CFLAGS+=$(call C.I,$(CFLAGS.I))
  endif
endif
ifdef LOPTS.L                     # Add global library path
  ifneq ($(LOPTS.L),)
    LOPTS+=$(call L.L,$(LOPTS.L))
  endif
endif
ifdef LOPTS.l                     # Add global libs
  ifneq ($(LOPTS.l),)
    LOPTS+=$(call L.l,$(LOPTS.l))
  endif
endif
TMPPAT=*.bak *~ *.cache
TMPBAK=$(addprefix $(1)/,$(TMPPAT))

define TARGET.TEMPLATE
  include src/$(1)/config          # Include module config
  ifeq ($$($(1).state),enable)     # Module enable for build
    ifeq ($$($(1).debug),enable)   # Debug enable
      $(1).cflags+=$(CFLAGS.DEBUG)
    endif
    ifdef $(1).I                   # Add local include path for module
      ifneq ($$($(1).I),)
        $(1).cflags+=$(call C.I,$$($(1).I))
      endif
    endif
    ifdef $(1).L                   # Add local library path for module
      ifneq ($$($(1).L),)
        $(1).lopts+=$(call L.L,$$($(1).L))
      endif
    endif
    ifdef $(1).libs                # Add local library path for module
      ifneq ($$($(1).libs),)
        $(1).lopts+=$(call L.l,$$($(1).libs))
      endif
    endif
    ifdef $(1).dep                 # Add module depends
      ifneq ($$($(1).dep),)
        $(1).modep+=$(addprefix build.,$$($(1).dep))
      endif
    endif
    ## << sources and objects search
    $(1).ccsrc+=$(wildcard src/$(1)/*.c)
    $(1).cxsrc+=$(wildcard src/$(1)/*.cxx)
    $(1).pkbnd+=$(wildcard src/$(1)/*.pkg)
    $(1).pksrc+=$(patsubst src/$(1)/%.pkg,src/$(1)/%.bind.cxx,$(wildcard src/$(1)/*.pkg))
    $(1).pkobj+=$(patsubst src/$(1)/%.pkg,obj/$(1).%.bind.oxx,$(wildcard src/$(1)/*.pkg))
    $(1).src+=$$($(1).ccsrc) $$($(1).cxsrc)
    ifeq ($(BIND),enable)
      $(1).src+=$$($(1).pksrc)
    endif
    $(1).ccobj+=$(patsubst src/$(1)/%.c,obj/$(1).%.o,$(wildcard src/$(1)/*.c))
    $(1).cxobj+=$(patsubst src/$(1)/%.cxx,obj/$(1).%.oxx,$(wildcard src/$(1)/*.cxx))
    $(1).obj+=$$($(1).ccobj) $$($(1).cxobj)
    ifeq ($(BIND),enable)
      $(1).obj+=$$($(1).pkobj)
    endif
    ## >> sources and objects search
    ifeq ($$($(1).runable),enable)      # Runable target build path
bin/$$($(1).name).$(RXT): $$($(1).obj)
	@$(ECHO) Build runable module $(1)....
	@$(EXEC) $(call RL,$$^,$$@,$(LOPTS) $$($(1).lopts))
build.bin.$(1): bin/$$($(1).name).$(RXT)
      $(1).target+=bin/$$($(1).name).$(RXT)
      TARGET.DIRS+=bin
$(PREFIX)/bin/$$($(1).name).$(RXT): bin/$$($(1).name).$(RXT) # Install rules
	@$(ECHO) Install $$^ to $$@....
	@$(EXEC) $$(call CP,$$^ $$@)
install.bin.$(1): $(PREFIX)/bin/$$($(1).name).$(RXT)
      $(1).install+=install.bin.$(1)
uninstall.bin.$(1): # Uninstall rules
	@$(ECHO) Uninstall $(PREFIX)/bin/$$($(1).name).$(RXT)....
	@$(EXEC) $$(call RM,$(PREFIX)/bin/$$($(1).name).$(RXT))
      $(1).uninstall+=uninstall.bin.$(1)
    endif
    ifeq ($$($(1).shared),enable)       # Shared library target build path
lib/$$($(1).name).$(SXT): $$($(1).obj)
	@$(ECHO) Build shared library $(1)....
	@$(EXEC) $(call SL,$$^,$$@,$(LOPTS) $$($(1).lopts))
build.libs.$(1): lib/$$($(1).name).$(SXT)
      $(1).target+=lib/$$($(1).name).$(SXT)
      TARGET.DIRS+=lib
$(PREFIX)/lib/$$($(1).name).$(SXT): lib/$$($(1).name).$(SXT) # Install rules
	@$(ECHO) Install $$^ to $$@....
	@$(EXEC) $$(call CP,$$^ $$@)
install.libs.$(1): $(PREFIX)/lib/$$($(1).name).$(SXT)
      $(1).install+=install.libs.$(1)
uninstall.libs.$(1): # Uninstall rules
	@$(ECHO) Uninstall $(PREFIX)/lib/$$($(1).name).$(SXT)....
	@$(EXEC) $$(call RM,$(PREFIX)/lib/$$($(1).name).$(SXT))
      $(1).uninstall+=uninstall.libs.$(1)
    endif
    ifeq ($$($(1).static),enable)       # Static library target build path
lib/$$($(1).name).$(AXT): $$($(1).obj)
	@$(ECHO) Build archive library $(1)....
	@$(EXEC) $(call AR,$$^,$$@)
build.liba.$(1): lib/$$($(1).name).$(AXT)
      $(1).target+=lib/$$($(1).name).$(AXT)
      TARGET.DIRS+=lib
$(PREFIX)/lib/$$($(1).name).$(AXT): lib/$$($(1).name).$(AXT) # Install rules
	@$(ECHO) Install $$^ to $$@....
	@$(EXEC) $$(call CP,$$^ $$@)
install.liba.$(1): $(PREFIX)/lib/$$($(1).name).$(AXT)
      $(1).install+=install.liba.$(1)
uninstall.liba.$(1): # Uninstall rules
	@$(ECHO) Uninstall $(PREFIX)/lib/$$($(1).name).$(AXT)....
	@$(EXEC) $$(call RM,$(PREFIX)/lib/$$($(1).name).$(AXT))
      $(1).uninstall+=uninstall.liba.$(1)
    endif
    ifeq ($$($(1).header),enable)       # Header file
$(PREFIX)/$$($(1).path)/$$($(1).name): $$($(1).path)/$$($(1).name)
	@$(ECHO) Install $$^ to $$@....
	@$(EXEC) $$(call CP,$$^ $$@)
install.inc.$(1): $(PREFIX)/$$($(1).path)/$$($(1).name)
      $(1).install+=install.inc.$(1)
uninstall.inc.$(1):
	@$(ECHO) Uninstall $(PREFIX)/$$($(1).path)/$$($(1).name)....
	@$(EXEC) $$(call RM,$(PREFIX)/$$($(1).path)/$$($(1).name))
      $(1).uninstall+=uninstall.inc.$(1)
    endif
.PHONY: $(1).prepare $(1).postpare
prepare.$(1): $(1).prepare $$($(1).dir) $$($(1).modep)
	@$(ECHO) Prepare module $(1)....
    TARGET.prepare+=prepare.$(1)
postpare.$(1): $(1).postpare
	@$(ECHO) Postpare module $(1)....
    TARGET.postpare+=postpare.$(1)
build.$(1): prepare.$(1) $$($(1).target) postpare.$(1) # Build all targets for module
	@$(ECHO) Build $(1) complete....
    TARGET.build+=build.$(1)
clean.$(1):                              # Clean last build module
	@$(ECHO) Clean module $(1)....
	@$(EXEC) $$(call RM,$$($(1).obj) $$($(1).target) $(call TMPBAK,src/$(1)))
    TARGET.clean+=clean.$(1)
probe.$(1):                             # Probe config for module
	@$(ECHO) Probe....
	@$(ECHO) Module $(1):
	@$(ECHO) \^
	@$(ECHO) \|-state=$$($(1).state)
	@$(ECHO) \|-runable=$$($(1).runable)
	@$(ECHO) \|-static=$$($(1).static)
	@$(ECHO) \|-shared=$$($(1).shared)
	@$(ECHO) \|-target=$$($(1).target)
	@$(ECHO) \|-source=$$($(1).src)
	@$(ECHO) \|-object=$$($(1).obj)
	@$(ECHO) \|-depend=$$($(1).dep)
	@$(ECHO) \|-pkbnd=$$($(1).pkbnd)
	@$(ECHO) \|-pksrc=$$($(1).pksrc)
	@$(ECHO) \|-library=$$($(1).libs)
	@$(ECHO) \|-modep=$$($(1).modep)
	@$(ECHO) \|-cflags=$$($(1).cflags)
	@$(ECHO) \|-lopts=$$($(1).lopts)
	@$(ECHO) \v
    TARGET.probe+=probe.$(1)
install.$(1): $(1).install.prep $$($(1).install) $(1).install.post # Install module rule
	@$(ECHO) Install $(1) complete....
    TARGET.install+=install.$(1)
uninstall.$(1): $(1).uninstall.prep $$($(1).uninstall) $(1).uninstall.post # Uninstall module rule
	@$(ECHO) Uninstall $(1) complete....
    TARGET.uninstall+=uninstall.$(1)
.PHONY: $(1).install.prep $(1).install.post $(1).uninstall.prep $(1).uninstall.post
obj/$(1).%.o: src/$(1)/%.c              # Compile .c   => .o
	@$(ECHO) Compile $$^ to $$@..
	@$(EXEC) $(call CC,$$^,$$@,$$(CFLAGS) $$($(1).cflags))
    TARGET.DIRS+=obj
obj/$(1).%.oxx: src/$(1)/%.cxx          # Compile .cxx => .oxx
	@$(ECHO) Compile $$^ to $$@..
	@$(EXEC) $(call CX,$$^,$$@,$$(CFLAGS) $$($(1).cflags))
    TARGET.DIRS+=obj
.PRECIOUS: src/$(1)/%.bind.cxx
src/$(1)/%.bind.cxx: src/$(1)/%.pxx     # Bind codegen .pxx => .cxx
	@$(ECHO) Binding $$^ to $$@..
	@$(EXEC) $(call CB,$$^,$$@,$$(BFLAGS) $$($(1).bflags))
  else # if module disabled
prepare.$(1):
	@$(ECHO) Prepare module $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.prepare+=prepare.$(1)
build.$(1):
	@$(ECHO) Build module $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.build+=build.$(1)
clean.$(1):
	@$(ECHO) Clean module $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.clean+=clean.$(1)
probe.$(1):
	@$(ECHO) Probe module $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.probe+=probe.$(1)
install.$(1):
	@$(ECHO) Install $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.install+=install.$(1)
uninstall.$(1):
	@$(ECHO) UnInstall $(1)....
	@$(ECHO) Module $(1) is disabled....
    TARGET.uninstall+=install.$(1)
  endif
endef

$(foreach cortege,$(TARGET.MODS),$(eval $(call TARGET.TEMPLATE,$(cortege))))

.PHONY: prepare.% build.% clean.% probe.%
.PHONY: prepare   build   clean   probe   all   local.prepare

global.prepare:
	@$(ECHO) Preparing build for PLATFOMR=$(PLATFORM) COMPILER=$(COMPILER)....

prepare: global.prepare local.prepare $(TARGET.prepare)

build: $(TARGET.build)
	@$(ECHO) Build all complete....

clean: $(TARGET.clean)
	@$(ECHO) Clean all complete....

probe: $(TARGET.probe)
	@$(ECHO) TARGET.MODS = $(TARGET.MODS)
	@$(ECHO) TARGET.DIRS = $(TARGET.DIRS)
	@$(ECHO) TARGET.{mkdir,rmdir} = {$(TARGET.mkdir),$(TARGET.rmdir)}

install: $(TARGET.install)
	@$(ECHO) Install all complete....

uninstall: $(TARGET.uninstall)
	@$(ECHO) Uninstall all complete....


