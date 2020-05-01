.PHONY: clean

CPPFLAGS=-I. -std=c++11 -Wall -Wno-strict-aliasing -Wno-narrowing -s -Os -fno-ident -fno-rtti -fno-exceptions -fmerge-all-constants
# CPPFLAGS=-I. -std=c++11 -Wall -Wno-strict-aliasing -Wno-narrowing -g3

clean:
	rm jasmine-as jasmine-dis jasmine-sim

all: jasmine-as jasmine-dis jasmine-sim

jasmine-as: as.cpp
	g++ $(CPPFLAGS) util/*.cpp as.cpp -o jasmine-as

jasmine-dis: dis.cpp
	g++ $(CPPFLAGS) util/*.cpp dis.cpp -o jasmine-dis

jasmine-sim: sim.cpp
	g++ $(CPPFLAGS) util/*.cpp sim.cpp -o jasmine-sim