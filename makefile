SRC=$(wildcard src/*.cpp)

OBJ=$(patsubst src/%.cpp,obj/%.o,$(wildcard src/*.cpp))

LIBS= -L".\SDL2\lib" -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
FLAGS = -std=c++17 -I"."


main: $(OBJ) jsoncpp.o
	g++ -o tr main.cpp $(OBJ) jsoncpp.o $(FLAGS) $(LIBS)

$(OBJ): obj/%.o: src/%.cpp
	g++ -c $< $(FLAGS) $(LIBS)
	move $(subst obj/,,$@) .\obj\

jsoncpp.o:
	g++ -c jsoncpp.cpp

clean:
	del .\obj\*