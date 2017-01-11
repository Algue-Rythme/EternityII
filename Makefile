PREFIX=.
SRCDIR=$(PREFIX)/src
INCDIR=$(PREFIX)/include
BINDIR=$(PREFIX)/bin
OBJDIR=$(PREFIX)/obj
DEPDIR=$(PREFIX)/depend

SFML=D:/libs/SFML-2.3

CC=g++
CPPFLAGS=-I$(INCDIR) -I$(SFML)/include -DSFML_STATIC
CFLAGS=-c -Wall -Werror -pedantic -std=c++11 -O2 -s
LDFLAGS=-static-libgcc -static-libstdc++ -L$(SFML)/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lfreetype -ljpeg -lgdi32 -lopengl32 -lwinmm

OUTFILE=$(BINDIR)/EternityII.exe
SRC_FILES=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(SRC_FILES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(DEPDIR)/%.d: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MT $(OBJDIR)/$(notdir $(^:.cpp=.o)) $^> $@
	
include $(SRC_FILES:$(SRCDIR)/%.cpp=$(DEPDIR)/%.d)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $< $(CFLAGS) $(CPPFLAGS) -o $@

$(OUTFILE): $(OBJS)
	$(CC) -mwindows $^ -o $@ $(LDFLAGS)

all: $(OUTFILE)

.PHONY: clean mrproper
	
clean: 
	del obj\*.o depend\*.d

mrproper: clean
	del bin\*.exe