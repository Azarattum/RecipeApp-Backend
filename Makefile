CC = gcc
CXX = g++

source = src/
dist = dist/
includes = includes/
app = $(source)app/
scripts = $(source)scripts/
tests = $(source)scripts/

sqlite: $(includes)sqlite3.c
	$(CC) -c $< -o $(dist)sqlite3.o

scripts: sqlite scripts_create_db scripts_insert_data

scripts_create_db: $(scripts)create_db.cpp
	$(CXX) -Wall -c $(scripts)create_db.cpp -o $(dist)create_db.o
	$(CXX) $(dist)create_db.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)create_db

scripts_insert_data: $(scripts)insert_data.cpp
	$(CXX) -Wall -c $(scripts)insert_data.cpp -o $(dist)insert_data.o
	$(CXX) $(dist)insert_data.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)insert_data

clean:
	rm -rf $(dist)*.o