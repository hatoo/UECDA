TARGET	= client
SRCS	= client.cpp connection.c mydef.cpp cardChange.c cardSelect.cpp handGenerator.cpp bitCard.cpp myrandom.cpp mydebug.cpp montecarlo.cpp machineLearn.cpp fastWeakAI.cpp ml2.cpp mldefine.cpp mldefine2012.cpp ml3.cpp mldefine10.cpp mldefine5.cpp 
OBJS	= mydef.o client.o connection.o cardChange.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o machineLearn.o fastWeakAI.o ml3.o mldefine.o mldefine2012.o mldefine10.o mldefine5.o 
mlOBJS  = mlmain.o mydef.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o machineLearn.o fastWeakAI.o

mlOBJS2 = mlmain2.o mydef.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o machineLearn.o fastWeakAI.o ml2.o ml3.o mldefine.o mldefine2012.o

mlOBJS3 = mlmain3.o mydef.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o machineLearn.o fastWeakAI.o ml3.o mldefine.o mldefine2012.o mldefine10.o mldefine5.o

mergeOBJS = mydef.o cardSelect.o handGenerator.o bitCard.o myrandom.o mydebug.o montecarlo.o machineLearn.o fastWeakAI.o mldefine.o mldefine2012.o mldefine10.o mldefine5.o merge.o ml3.o


CC = gcc 
CPP= g++ -std=c++0x
CPPPG=g++ -std=c++0x -pg
LIBS = -lboost_serialization-mt
CFLAGS = -O3 -msse4 

all: $(TARGET)

$(TARGET): $(OBJS) $(SRCS)
	$(CPPPG) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

ml: $(mlOBJS)
	$(CPPPG) $(CFLAGS) $(mlOBJS) $(LIBS)
ml2:$(mlOBJS2)
	$(CPPPG) $(CFLAGS) $(mlOBJS2) $(LIBS) -o b.exe
ml3:$(mlOBJS3)
	$(CPPPG) $(CFLAGS) $(mlOBJS3) $(LIBS) -DNDEBUG -o c.exe
merge:$(mergeOBJS)
	$(CPPPG) $(CFLAGS) $(mergeOBJS) $(LIBS) -o merge.exe
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CPP) $(CFLAGS) -c -o $@ $< $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET) $(mlOBJS3)
