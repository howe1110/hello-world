src_files = $(wildcard *.cpp)
head_files = $(wildcard *.h)
objects = $(src_files:%.cpp=%.o)
target = myp2p

$(target): $(objects)
	g++ $(objects) -lws2_32 -o $@
$(objects): %.o: %.cpp
	g++ -MMD -c $<

.PHONY: clean
clean:
	rm -f $(objects) $(target)

-include *.d