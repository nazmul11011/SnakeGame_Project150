all:
	g++ -I src/include -L src/lib -o SNAKEgame main.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	g++ -I src/include -L src/lib -o test1 test1.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer