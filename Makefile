INCLUDE := -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers -I/Library/Frameworks/SDL2_mixer.framework/Headers -I include
IJSON := -I/Users/nathanklundt/code/cpp/jsoncpp
LJSON := -L/Users/nathanklundt/code/cpp/jsoncpp
FW := -framework SDL2 -framework SDL2_ttf -framework SDL2_mixer -F/Library/Frameworks
COMPILER := g++ -std=c++11 -Wall

main: main.o TextManager.o app.o clock.o http.o initialization.o geometry.o
	$(COMPILER) -o main main.o TextManager.o app.o clock.o http.o initialization.o geometry.o $(FW) -lcurl -ljsoncpp $(LJSON)

main.o: main.cpp
	$(COMPILER) -c $< -I include

app.o: app.cpp app.hpp AppErr.hpp constants.hpp
	$(COMPILER) -c $< $(INCLUDE)

TextManager.o: TextManager.cpp TextManager.hpp
	$(COMPILER) -c $< $(INCLUDE)

clock.o: clock.cpp Clock.hpp
	$(COMPILER) -c $< -I include

http.o: http.cpp http.hpp env.hpp
	$(COMPILER) -c $< -I include $(IJSON)

initialization.o: initialization.cpp initialization.hpp constants.hpp
	$(COMPILER) -c $< $(INCLUDE)

geometry.o: geometry.cpp geometry.hpp
	$(COMPILER) -c $< $(INCLUDE)

vpath %.cpp src
vpath %.hpp include

