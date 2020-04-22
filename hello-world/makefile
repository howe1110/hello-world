src_files = $(wildcard *.cpp)
head_files = $(wildcard *.h)
objects = $(src_files:%.cpp=%.o)
target = myp2p

INCLUDEPATH = -I../../../3rd/spdlog/include
LIBPATH = -L../../../3rd/spdlog/build
CFALG = -g -c

$(target): $(objects)
	g++ $(objects) $(LIBPATH) -lpthread -lspdlog -o $@
$(objects): %.o: %.cpp
	g++ -MMD  $(INCLUDEPATH) $(CFALG) $<

.PHONY: clean
clean:
	rm -f $(objects) $(target)

-include *.d