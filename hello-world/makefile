src_files = $(wildcard *.cpp)
head_files = $(wildcard *.h)
objects = $(src_files:%.cpp=%.o)
target = myp2p

$(target): $(objects)
	g++ $(objects) -lpthread -o $@
$(objects): %.o: %.cpp
	g++ -MMD  -g -c $<

.PHONY: clean
clean:
	rm -f $(objects) $(target)

-include *.d