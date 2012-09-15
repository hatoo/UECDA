TARGET	= client
SRCS	= client.cpp connection.c daihinmin.c mydef.cpp cardChange.c cardSelect.cpp handGenerator.cpp bitCard.cpp myrandom.cpp mydebug.cpp montecarlo.cpp
OBJS	= mydef.o client.o connection.o daihinmin.o cardChange.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o

CC = gcc 
CPP= g++ -std=c++0x
CPPPG=g++ -std=c++0x -pg
LIBS =   
CFLAGS = -O3 -msse4

all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	$(CPPPG) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
