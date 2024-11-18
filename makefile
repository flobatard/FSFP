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


SOURCES=LMDB_wrapper.cpp http_server.cpp fsfp_utils.cpp
OBJS=$(SOURCES:%.cpp=%.o)
PATHOBJS=$(OBJS:%=$(OBJ)%)
WARNINGS=-Wall -Wextra -pedantic

TEST_SOURCES=concurrency_test.cpp simple_test.cpp
TEST_BIN=$(TEST)bin$(SEP)
TEST_OBJ=$(TEST)obj$(SEP)
TEST_OBJS=$(TEST_SOURCES:$(TEST)$(SEP)%.cpp=$(TEST)$(SEP)%.o)
TEST_EXES=$(TEST_SOURCES:%.cpp=test_%.exe)

DEPENDENCIES=liblmdb.a

LIBS=$(DEPENDENCIES:%.a=$(LIB)%.a)

FLAGS= -std=c++23 $(WARNINGS) -I$(HEADERDIR) -I$(INCLUDEDIR) -L$(LIB)  

all: $(OBJS) main.o
	$(CC) $(FLAGS) -o $(BIN)$(NAME).exe -static $(OBJ)main.o $(OBJS:%=$(OBJ)%) $(LIBS)

tests: $(TEST_EXES)
	@echo Test generated

test_%.exe: test_%.o
	$(CC) $(FLAGS) -o $(TEST_BIN)$(@:test_%.exe=%.exe) -static $(TEST_OBJ)$(@:test_%.exe=%.o) $(OBJS:%=$(OBJ)%) $(LIBS)

test_%.o:
	$(CC) -g $(FLAGS) -c $(TEST)$(@:test_%.o=%.cpp) -o $(TEST)$(SEP)$(OBJ)$(@:test_%.o=%.o) -static

main.o:
	$(CC) -g $(FLAGS) -c main.cpp -o $(OBJ)main.o -static

%.o:
	$(CC) -g $(FLAGS) -c $(SOURCEDIR)$(@:%.o=%.cpp) -o $(OBJ)$@ -static

share: $(OBJS)
	ar -rcs $(DISTR)$(NAME).a $(OBJS:%=$(OBJ)%)