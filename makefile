thereckoning:
	g++ -o thereckoning main.cpp `sdl2-config --cflags --libs` -lSDL2_ttf
clean:
	rm thereckoning
quick:
	make clean
	make
	./thereckoning
