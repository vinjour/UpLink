CC = mips-openwrt-linux-gcc
CFLAGS = -g -Wall
LDFLAGS = 
LIBIFLAGS = -I/home/ensea/openwrt/openwrt-sdk-21.02.3-ath79-generic_gcc-8.4.0_musl.Linux-x86_64/staging_dir/target-mips_24kc_musl/usr/include
LIBDFLAGS = -L/home/ensea/openwrt/openwrt-sdk-21.02.3-ath79-generic_gcc-8.4.0_musl.Linux-x86_64/staging_dir/target-mips_24kc_musl/usr/lib
CFILES = client.c
TARGET = client

all: $(TARGET)

 $(TARGET): $(CFILES)
	$(CC) $(CFLAGS) $(CFILES) $(LIBIFLAGS) $(LIBDFLAGS) -lwebsockets -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
