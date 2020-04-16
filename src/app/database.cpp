#include "../../includes/sqlite3.h"
#include <regex>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

typedef struct {
	string name;
	int relevancy;
} ingredient_t;

string sanitize_string(char* string);

/**
 * Connects to the database and returns sqlite3 pointer
 * */
sqlite3* connect_db()
{
	sqlite3* db;
	char* filename = new char[80];
	//Try to find the database file
	if (access("../data/database.db", F_OK) != -1) {
		strcpy(filename, (char*)"../data/database.db");
	} else if (access("./data/database.db", F_OK) != -1) {
		strcpy(filename, (char*)"./data/database.db");
	} else if (access("./database.db", F_OK) != -1) {
		strcpy(filename, (char*)"./database.db");
	} else {
		fprintf(stderr, "Database file not found!\n");
		exit(EXIT_FAILURE);
	}
	if (sqlite3_open(filename, &db)) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	sqlite3_enable_load_extension(db, 1);

	return db;
}

/**
 * Callback function for ingredient search
 * */
int ingredient_search_callback(void* data, int argc, char** argv, char** azColName)
{
	ingredient_t* ingredient = new ingredient_t();
	ingredient->name = argv[0];
	ingredient->relevancy = atoi(argv[1]);
	((vector<ingredient_t>*)data)->push_back(*ingredient);

	return 0;
}

/**
 * Performs ingredient search by its name
 * */
vector<ingredient_t> ingredient_search(char* name)
{
	//Defining things
	char* errorMessage = 0;
	sqlite3* db = connect_db();
	//Sanitizing input
	std::string sanitized = sanitize_string(name);

	//Building query
	std::string query = "SELECT name, (CASE WHEN name LIKE '"
		+ sanitized + "' THEN 3"
		+ " WHEN name LIKE '" + sanitized + "%' THEN 2"
		+ " WHEN name LIKE '% " + sanitized + "%' THEN 1";

	int length = sanitized.size();
	for (int i = 0; i < length; i += 2) {
		query += " WHEN name LIKE '";

		sanitized.insert(max(length - 2 - i, 0), "%");
		query += sanitized + "%' THEN " + to_string(-(i / 2));
	}

	query += "  END) as relevancy"
			 "  FROM Ingredients"
			 "  WHERE relevancy NOT NULL"
			 "  ORDER BY relevancy DESC"
			 "  LIMIT 10";

	//Creating resulting vector
	vector<ingredient_t>* ingredients = new vector<ingredient_t>();

	//Executing the query
	if (sqlite3_exec(
			db, query.c_str(), ingredient_search_callback, ingredients, &errorMessage)
		!= SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}

	return *ingredients;
}

/**
 * Sanitizes the input string from special characters
 * */
string sanitize_string(char* string)
{
	const regex sanitize(
		R"([^ёйцукенгшщзхъфывапролджэячсмитьбюЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁa-z,-.0-9 ]+)",
		regex::icase);
	std::string sanitized = regex_replace(string, sanitize, "");
	return sanitized;
}