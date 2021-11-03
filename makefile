thereckoning:
	g++ -o thereckoning main.cpp `sdl2-config --cflags --libs`
clean:
	rm thereckoning
quick:
	make clean
	make
	./thereckoning
