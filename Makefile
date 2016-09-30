CXXFLAGS += -std=c++11
CXXFLAGS += -I./
CXXFLAGS += -std=c++11 -Wall -Werror -g -c -o

LIB_FILES :=-lglog -lgflags -levent  -lpthread -lssl -lcrypto -lz  \
	-lgrpc++ -lgrpc -lgrpc++_reflection -lprotobuf -lpthread -ldl

TEST_LIB_FILES :=  -L/usr/local/lib -lgtest -lgtest_main -lpthread

PROTOC = protoc
GRPC_CPP_PLUGIN=grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
PROTOS_PATH = ./protos

CPP_SOURCES := \
	./core/base/arena.cc \
	./core/platform/mem.cc \
	./protos/helloworld.pb.cc \
	./protos/helloworld.grpc.pb.cc \

CPP_OBJECTS := $(CPP_SOURCES:.cc=.o)


TESTS := \
	./core/base/arena_unittest \
	./helloworld/greeter_client \
	./helloworld/greeter_server \

########
vpath %.proto $(PROTOS_PATH)

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=$(PROTOS_PATH) \
		--plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=$(PROTOS_PATH) $<

all: $(CPP_OBJECTS) $(TESTS)
.cc.o:
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

######
./core/base/arena_unittest: ./core/base/arena_unittest.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES) $(TEST_LIB_FILES)
./core/base/arena_unittest.o: ./core/base/arena_unittest.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./helloworld/greeter_client: \
	./helloworld/greeter_client.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./helloworld/greeter_client.o: \
	./helloworld/greeter_client.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

./helloworld/greeter_server: \
	./helloworld/greeter_server.o
	@echo "  [LINK] $@"
	@$(CXX) -o $@ $< $(CPP_OBJECTS) $(LIB_FILES)
./helloworld/greeter_server.o: \
	./helloworld/greeter_server.cc
	@echo "  [CXX]  $@"
	@$(CXX) $(CXXFLAGS) $@ $<

## /////////////////////////////

clean:
	@rm -fr $(TESTS)
	@echo "rm *_unittest"
	@rm -fr $(CPP_OBJECTS)
	@echo "rm *.o"
	@rm -fr ./protos/*.pb.h
	@rm -fr ./protos/*.pb.cc
