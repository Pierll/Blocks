#gcc tetris.c -o tetris -lSDL2-2.0 -g
make
if [ $? -eq 0 ] #si la compilation réussi
then
	./tetris
fi
