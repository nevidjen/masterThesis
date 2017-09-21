#IDIR = just in case we have some include dir
#CC=g++
#CFLAGS= -Wall #-I$(IDIR)

#ODIR=obj
#LDIR =../lib

#LIBS=-lm

#_DEPS = Predictor.h
#DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

#_OBJ = Predictor.o Predictor.o
#OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


#$(ODIR)/%.o: %.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

#Predictor: $(OBJ)
#	gcc -o $@ $^ $(CFLAGS) $(LIBS)

#.PHONY: clean

#clean:
#	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~

PROJNAME=Predictor
CC=g++
IDIR=./include
SRCDIR=./src
OUTDIR=./bin
SRCFILES = Predictor.cpp
SOURCES = $(SRCFILES:%.cpp=$(SRCDIR)/%.cpp)
INCFILES = Predictor.h
INCLUDES = $(INCFILES:%.h=$(IDIR)/%.h)

CFLAGS= -std=c++11 -Wall -I$(IDIR) -I$(SRCDIR)

$(PROJNAME): main.cpp $(SOURCES) $(INCLUDES)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $(OUTDIR)/$(PROJNAME) main.cpp $(SOURCES)

.PHONY: clean

clean:
	rm -f *.o *~ core $(OUTDIR)/$(PROJNAME)
