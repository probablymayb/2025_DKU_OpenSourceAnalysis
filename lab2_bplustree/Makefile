CXX = g++
CXXFLAGS = -Wall -g

TARGET = lab2_bplustree
OBJS = src/bplustree_test.o src/zipf.o src/latest-generator.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

src/skiplist_test.o: src/bplustree_test.cc src/bplustree.h src/zipf.h src/latest-generator.h
	$(CXX) $(CXXFLAGS) -c src/bplustree_test.cc -o src/bplustree_test.o

src/zipf.o: src/zipf.cc src/zipf.h
	$(CXX) $(CXXFLAGS) -c src/zipf.cc -o src/zipf.o

src/latest-generator.o: src/latest-generator.cc src/latest-generator.h
	$(CXX) $(CXXFLAGS) -c src/latest-generator.cc -o src/latest-generator.o

clean:
	rm -f $(TARGET) $(OBJS)