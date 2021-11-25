all: 
	gcc -std=c17 `pkg-config --cflags gtk4` -o flashcards gui.c data.c main.c `pkg-config --libs gtk4` && chmod +x flashcards && ./flashcards
	 
debug-gcc-analyzer:
	gcc -g -fanalyzer -std=c17 `pkg-config --cflags gtk4`  -o flashcards gui.c data.c main.c `pkg-config --libs gtk4`

debug-gcc-address-sanitizer:
	gcc -g -fsanitize=address -std=c17 `pkg-config --cflags gtk4`  -o flashcards gui.c data.c main.c `pkg-config --libs gtk4`
	 
clean:
#CFLAGS+="-DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED" gcc -std=c17 `pkg-config --cflags gtk4` -o flashcards gui.c data.c main.c `pkg-config --libs gtk4`
