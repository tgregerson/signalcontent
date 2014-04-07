OBJDIR = build_files

VPATH = src/base:src/codec:src/parser

CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++0x
CXX = clang++

BASE_O = $(addprefix $(OBJDIR)/,huffman.o)

TARGET = signal_content_main

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)
base: $(BASE_O)

clean:
	rm -f $(OBJS) $(TARGET)

BASE_H = four_value_logic.h frame_fv.h macros.h queue_fv.h
CODEC_H = fixed_frame_huffman.h huffman.h
PARSER_H = parser_interface.h

$(OBJDIR)/huffman.o: huffman.cpp $(CODEC_H) $(BASE_H)
	$(CXX) -c $< $(CXXFLAGS) -o $@
