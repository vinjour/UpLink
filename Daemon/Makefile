CC = mips-openwrt-linux-gcc
CFLAGS = -g -Wall
LDFLAGS =
OBJFILES = functions.o main.o
TARGET = daemon

all: $(TARGET)

 $(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(TARGET) $(LDFLAGS)

main.o: main.c functions.h
	$(CC) $(CFLAGS) -c $< -o  $@ $(LDFLAGS)

functions.o: functions.c functions.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET)
