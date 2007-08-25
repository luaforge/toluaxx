
PACKAGE = $(wildcard t*.lua)
MTARGET = $(patsubst t%.lua,m%.so,$(PACKAGE)) mmain.x86
TTARGET = $(patsubst t%.lua,test.%,$(PACKAGE)) test.main
TARGET  = test

PRERUN=LD_LIBRARY_PATH=../../lib
COPTS += -g -D DEBUG_MODE=1

BOPTS += -n $(patsubst t%.pkg,m%,$^) -c
LOPTS += --shared `pkg-config --libs lua5.1` -L../../lib -ltoluaxx5.1
XOPTS += `pkg-config --libs lua5.1` -L../../lib -ltoluaxx5.1
COPTS += `pkg-config --cflags lua5.1` -I../../include/toluaxx5.1

ECHO=echo
VME=$(PRERUN) lua5.1
CB=$(PRERUN) ../../bin/toluaxx5.1.run --no-cleanup -I ../../ -I../../bin
CX=g++ -c -g
CXX=g++ -g
RM=rm -f

all: $(TARGET)

tlrb:
	@cd ../../ && make clean all

build: $(MTARGET) 
	@$(ECHO) "Make all .."

test.main: mmain.x86
#	@$(ECHO) Test main..
	@$(PRERUN) ./$< -o2 v2 file

test: $(TTARGET)
	@$(ECHO) "Test all .."

probe:
	@$(ECHO) "MTARGET: $(MTARGET) .."
	@$(ECHO) "TTARGET: $(TTARGET) .."

m%.so: t%.bind.oxx t%.oxx
	@$(ECHO) "Linking $^ to $@ .."
	@$(CXX) $^ -o $@ $(LOPTS)

m%.x86: t%.bind.oxx t%.oxx
	@$(ECHO) "Linking $^ to $@ .."
	@$(CXX) $^ -o $@ $(XOPTS)

test.%: t%.lua m%.so
#	@$(ECHO) Test $* ..
	@$(VME) $<

tmain.bind.cxx: tmain.pxx
	@$(CB) -n mmain -o $@ $^

t%.bind.cxx: t%.pxx
	@$(CB) -n m$* -o $@ $^

t%.bind.test: t%.pxx
	@$(CB) -P -n m$* $^ > $@

%.oxx: %.cxx
	@$(CX) $(COPTS) -o $@ $^

clean:
	@$(ECHO) Clean all ..
	@$(RM) *.oxx *~ *.bind.cxx *.cache $(MTARGET)

.SUFFIXES: t%.bind.cxx %.oxx
.PRECIOUS: t%.bind.cxx %.oxx
