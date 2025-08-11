bin_PROGRAMS=video test thimble


video_CFLAGS=$(shell pkg-config --cflags gtk+-3.0) -I../../include
video_LDFLAGS=-Wl,--no-as-needed $(shell pkg-config --libs gtk+-3.0)

video_SOURCES=   \
main.c          \
shmem.c         \
shmem.h         \
hw_c24p.h

test_CFLAGS=-I../../include
test_SOURCES=   \
test.c          \
hw_c24p.h 

thimble_CFLAGS=$(shell pkg-config --cflags gtk+-3.0)
thimble_LDFLAGS=-Wl,--no-as-needed $(shell pkg-config --libs gtk+-3.0)
thimble_SOURCES=tmain.c

