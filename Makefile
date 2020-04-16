CC = gcc
CXX = g++

source = src/
dist = dist/
includes = includes/
app = $(source)app/
scripts = $(source)scripts/
tests = $(source)scripts/

#Entry points
app: $(dist)app.o $(dist)sqlite3.o
	$(CXX) $(dist)app.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)app

scripts: $(dist)sqlite3.o $(dist)create_db.o $(dist)insert_data.o
	$(CXX) $(dist)create_db.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)create_db
	$(CXX) $(dist)insert_data.o $(dist)sqlite3.o -lpthread -ldl -o $(dist)insert_data

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