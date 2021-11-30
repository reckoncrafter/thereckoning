SRC=$(wildcard src/*.cpp)

OBJ=$(patsubst src/%.cpp,obj/%.o,$(wildcard src/*.cpp))

LIBS= -lSDL2_ttf -ljsoncpp
FLAGS = `sdl2-config --cflags --libs`



main: $(OBJ)
	g++ -o tr main.cpp $(OBJ) $(FLAGS) $(LIBS)

$(OBJ): obj/%.o: src/%.cpp
	g++ -c $< $(FLAGS) $(LIBS)
	mv $(subst obj/,,$@) obj

clean:
	rm obj/*