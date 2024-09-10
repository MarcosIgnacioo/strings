
te: ./src/main.c
	$(CC) -o ./out/\main ./src/main.c 
	./out/\main

run:
	./out/\main

clear:
	rm -r ./out/\main
