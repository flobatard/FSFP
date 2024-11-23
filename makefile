CC=g++
NAME=main

SEP=/

SOURCEDIR=src$(SEP)
HEADERDIR=headers$(SEP)
INCLUDEDIR=include$(SEP)

OBJ=obj$(SEP)
BIN=bin$(SEP)
LIB=lib$(SEP)
TEST=test$(SEP)
DISTR=distr$(SEP)

OBJSUBDIRS=databases routes/middlewares
OBJDIRS=$(OBJSUBDIRS:%=$(OBJ)%)

SOURCES=LMDB_wrapper.cpp http_server.cpp fsfp_utils.cpp routes/admin.cpp routes/guest.cpp routes/owner.cpp databases/registry.cpp \
	databases/file.cpp databases/owners.cpp routes/middlewares/auth_middlewares.cpp


OBJS=$(SOURCES:%.cpp=%.o)
PATHOBJS=$(OBJS:%=$(OBJ)%)
WARNINGS=-Wall -Wextra -pedantic -pedantic-errors

TEST_SOURCES=concurrency_test.cpp simple_test.cpp search_test.cpp
TEST_BIN=$(TEST)bin$(SEP)
TEST_OBJ=$(TEST)obj$(SEP)
TEST_OBJS=$(TEST_SOURCES:$(TEST)$(SEP)%.cpp=$(TEST)$(SEP)%.o)
TEST_EXES=$(TEST_SOURCES:%.cpp=test_%.exe)
TEST_MEM_EXES=$(TEST_SOURCES:%.cpp=test_mem%.exe)

DEPENDENCIES=liblmdb.a

LIBS=$(DEPENDENCIES:%.a=$(LIB)%.a)
FSANITIZER_FLAG=-fsanitize=address

FLAGS= -std=c++23 $(WARNINGS) -I$(HEADERDIR) -I$(INCLUDEDIR) -L$(LIB)

all: $(OBJS) main.o $(BIN)
	$(CC) $(FLAGS) -o $(BIN)$(NAME).exe -static $(OBJ)main.o $(OBJS:%=$(OBJ)%) $(LIBS)

# For the weird linux line, note that this command doesn't work on other platform than linux
# https://stackoverflow.com/questions/78293129/c-programs-fail-with-asan-addresssanitizerdeadlysignal/78302537
mem_main: $(OBJS) main.o $(BIN)
	$(CC) $(FSANITIZER_FLAG) $(FLAGS) -o $(BIN)mem_$(NAME).exe $(OBJ)main.o $(OBJS:%=$(OBJ)%) $(LIBS)
	/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2 ./bin/mem_$(NAME).exe

tests: $(TEST_EXES)
	@echo Test generated

mem_tests: $(TEST_MEM_EXES)

clean:
	rm -rf obj/*
	rm -rf bin/*
	rm -rf test/bin/*
	rm -rf test/obj/*

share: $(OBJS)
	ar -rcs $(DISTR)$(NAME).a $(OBJS:%=$(OBJ)%)

test_%.exe: test_%.o $(TEST_BIN)
	$(CC) $(FLAGS) -o $(TEST_BIN)$(@:test_%.exe=%.exe) -static $(TEST_OBJ)$(@:test_%.exe=%.o) $(OBJS:%=$(OBJ)%) $(LIBS)

test_mem%.exe: test_%.o $(TEST_BIN)
	$(CC) $(FSANITIZER_FLAG) $(FLAGS) -o $(TEST_BIN)$(@:test_mem%.exe=mem_%.exe) $(TEST_OBJ)$(@:test_mem%.exe=%.o) $(OBJS:%=$(OBJ)%) $(LIBS)

test_%.o: $(TEST_OBJ)
	$(CC) -g $(FLAGS) -c $(TEST)$(@:test_%.o=%.cpp) -o $(TEST)$(SEP)$(OBJ)$(@:test_%.o=%.o)

main.o: $(OBJ)
	$(CC) -g $(FLAGS) -c main.cpp -o $(OBJ)main.o

%.o: $(OBJDIRS)
	$(CC) -g $(FLAGS) -c $(SOURCEDIR)$(@:%.o=%.cpp) -o $(OBJ)$@

$(BIN):
	mkdir -p $@

$(OBJ):
	mkdir -p $@

$(TEST_BIN):
	mkdir -p $@

$(TEST_OBJ):
	mkdir -p $@

$(OBJ)%:
	mkdir -p $@

%:
	@echo "Unknown command $@"