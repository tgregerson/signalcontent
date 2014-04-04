OBJDIR = build_files

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x
CXX = clang++

BASE_O = $(addprefix $(OBJDIR)/,.o)

TARGET = signal_content_main

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

BASE_H = macros.h four_value_logic.h frame_queue.h frame_vector.h queue_fv.h
PARSER_H = parser_interface.h

