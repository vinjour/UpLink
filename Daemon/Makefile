CC = mips-openwrt-linux-gcc
CFLAGS = -g -Wall
LDFLAGS =
LIBIFLAGS = -I/home/ensea/openwrt/openwrt-sdk-21.02.3-ath79-generic_gcc-8.4.0_musl.Linux-x86_64/staging_dir/target-mips_24kc_musl/usr/include
LIBDFLAGS = -L/home/ensea/openwrt/openwrt-sdk-21.02.3-ath79-generic_gcc-8.4.0_musl.Linux-x86_64/staging_dir/target-mips_24kc_musl/usr/lib
OBJFILES = client.o functions.o main.o
TARGET = daemon

all: $(TARGET)

 $(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $(TARGET) $(LIBIFLAGS) $(LIBDFLAGS) -lwebsockets $(LDFLAGS)

main.o: main.c main.h
	$(CC) $(CFLAGS) -c $< -o  $@ $(LDFLAGS)

functions.o: functions.c main.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

client.o: client.c main.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBIFLAGS) $(LIBDFLAGS) -lwebsockets $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET)
