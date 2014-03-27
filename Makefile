CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		signal_content_main.o

LIBS =

TARGET =	signal_content_main

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
