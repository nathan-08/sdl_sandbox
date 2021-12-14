INCLUDE := -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers -I include
FW := -framework SDL2 -framework SDL2_ttf -F/Library/Frameworks
COMPILER := g++ -std=c++17 -Wall

main: main.o TextManager.o app.o initialization.o
	$(COMPILER) -o $@ $^ $(FW)

main.o: main.cpp rect.hpp
	$(COMPILER) -c $< -I include

%.o: %.cpp %.hpp
	$(COMPILER) -c $< $(INCLUDE)

vpath %.cpp src
vpath %.hpp include

.PHONY: run
run: main
	./main

.PHONY: clean
clean:
	-rm -rf *.o main
	-rm -rf main.dSYM

