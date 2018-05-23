OBJ= ext4.o fat32.o common.o functionalities.o commands.o main.o
C_OBJ= ext4.o fat32.o common.o functionalities.o commands.o main.o
CC= gcc
CFLAGS= -std=gnu99 -Wall -Wextra -ggdb -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter -g
LDFLAGS= -lm
TARGET= ragnarok

all: clear
	 make $(TARGET)
	 rm -rf *.o

ext4.o: fs_utils/ext4.c fs_utils/ext4.h lib/exitCodes.h
	$(CC) -c fs_utils/ext4.c $(CFLAGS)

fat32.o: fs_utils/fat32.c fs_utils/fat32.h lib/exitCodes.h
	$(CC) -c fs_utils/fat32.c $(CFLAGS)

common.o: fs_utils/common.c fs_utils/common.h
	$(CC) -c fs_utils/common.c $(CFLAGS)

functionalities.o: functionalities/functionalities.c functionalities/functionalities.h
	$(CC) -c functionalities/functionalities.c $(CFLAGS)

commands.o: shell/commands.c shell/commands.h
	$(CC) -c shell/commands.c $(CFLAGS)

main.o: main.c shell/commands.h
	$(CC) -c main.c $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clear:
	rm -rf *.o
	rm -f $(TARGET)