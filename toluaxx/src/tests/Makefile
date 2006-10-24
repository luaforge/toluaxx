
PACKAGE = $(wildcard t*.lua)
MTARGET = $(patsubst t%.lua,m%.so,$(PACKAGE))
TTARGET = $(patsubst t%.lua,test.%,$(PACKAGE))
TARGET  = test

COPTS += -g -D DEBUG_MODE=1

BOPTS += -n $(patsubst t%.pkg,m%,$^)
LOPTS += --shared `pkg-config --libs lua5.1` -L../../lib -ltoluaxx5.1
COPTS += `pkg-config --cflags lua5.1` -I../../include/toluaxx5.1

ECHO=echo
VME=lua5.1
CB=../../bin/toluaxx5.1.run
CX=g++ -c
RM=rm -f

all: $(TARGET)

tlrb:
	@cd ../../ && make clean all

build: $(MTARGET)
	@$(ECHO) "Make all .."

test: $(TTARGET)
	@$(ECHO) "Test all .."

probe:
	@$(ECHO) "MTARGET: $(MTARGET) .."
	@$(ECHO) "TTARGET: $(TTARGET) .."

m%.so: t%.bind.oxx t%.oxx
	@$(ECHO) "Linking $^ to $@ .."
	@$(CXX) $^ -o $@ $(LOPTS)

test.%: t%.lua m%.so
	@$(ECHO) Test $* ..
	@$(VME) $<

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
