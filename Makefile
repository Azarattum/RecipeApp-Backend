#Compilers
CC = gcc
CXX = g++
python = python3

#Paths
source = src/
dist = dist/
includes = includes/
app = $(source)app/
scripts = $(source)scripts/
tests = $(source)tests/

#Libraries
static = -lssl -lcrypto -lboost_system -lstdc++
dynamic = -lpthread -ldl


#Entry points
app: $(dist)app.o $(dist)sqlite3.o
	$(CXX) $(dist)app.o $(dist)sqlite3.o -Wl,-Bstatic $(static) -Wl,-Bdynamic $(dynamic) -Wl,--as-needed -o $(dist)app

scripts: $(dist)sqlite3.o $(dist)create_db.o $(dist)insert_data.o
	$(CXX) $(dist)create_db.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)create_db
	$(CXX) $(dist)insert_data.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)insert_data

tests: $(dist)test_app.o $(dist)sqlite3.o
	$(CXX) $(dist)test_app.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)test_app

#Test App
$(tests)test_app.cpp: $(tests)test_app.h
	$(python) $(tests)cxxtestgen --error-printer -o $(tests)test_app.cpp $(tests)test_app.h

$(dist)test_app.o: $(tests)test_app.cpp
	$(CXX) -c -I$(includes) -o $(dist)test_app.o $(tests)test_app.cpp

#SQLite 3
$(dist)sqlite3.o: $(includes)sqlite3.c
	$(CC) -c $< -o $(dist)sqlite3.o

#App
$(dist)app.o: $(app)main.cpp $(app)database.cpp $(app)server.cpp
	$(CXX) -Wall -c $(app)main.cpp -o $(dist)app.o -g

#Scripts
$(dist)create_db.o: $(scripts)create_db.cpp
	$(CXX) -Wall -c $(scripts)create_db.cpp -o $(dist)create_db.o

$(dist)insert_data.o: $(scripts)insert_data.cpp
	$(CXX) -Wall -c $(scripts)insert_data.cpp -o $(dist)insert_data.o

clean:
	rm -rf $(dist)*.o