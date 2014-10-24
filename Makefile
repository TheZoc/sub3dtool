LIBS = 
CFLAGS = -g -std=c99 -Wall -pedantic
LDFLAGS = -g
OUTPUT = sub3dtool

HEADERS = src/global.h src/utils.h src/subsrt.h src/subass.h src/subsrt_ass.h src/subass3d.h
SRC = src/main.c src/utils.c src/subsrt.c src/subass.c src/subsrt_ass.c src/subass3d.c
OBJ = $(SRC:.c=.o)

all: $(OBJ) $(OUTPUT)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUTPUT) $(LDFLAGS) $(LIBS)

clean:
	rm -f src/*.o
