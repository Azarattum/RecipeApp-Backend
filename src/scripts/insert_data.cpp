#include "../../includes/sqlite3.h"
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>

using namespace std;

int on_ingredient(void* data, int argc, char** argv, char** azColName);

/**
 * Script for converting and inserting raw data to the database
 * */
int main(int argc, char const* argv[])
{
	locale::global(locale("en_US.UTF-8"));

	char* errorMessage = 0;
	sqlite3* db;
	sqlite3* raw;

	if (sqlite3_open("../data/database.db", &db)) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
	}
	if (sqlite3_open("../data/raw.db", &raw)) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(raw));
		return 1;
	}

	//Fill ingredients

	char* query = (char*)"SELECT ingredients FROM recipe_part_1 LIMIT 11;";

	if (sqlite3_exec(raw, query, on_ingredient, db, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	sqlite3_close(db);
	sqlite3_close(raw);

	return 0;
}

/**
 * Callback when ingredient is selected
 * */
int on_ingredient(void* data, int argc, char** argv, char** azColName)
{
	string ingredients(argv[0]);

	const regex ingredient_regex(
		R"~([>]([а-я ,%0-9]+?)\s*:\s*[^<])~",
		regex::ECMAScript | regex::icase | regex::collate);
	smatch matches;

	//Search for ingredients and create query
	string query = "INSERT OR IGNORE INTO Ingredients (name) VALUES ";
	while (regex_search(ingredients, matches, ingredient_regex)) {
		string ingredient = matches[1];

		query.append("(\"");
		query.append(ingredient);
		query.append("\"),");

		ingredients = matches.suffix();
	}
	char* c_query = (char*)query.c_str();
	c_query[query.size() - 1] = ';';

	//Execute the query to insert items
	char* errorMessage = 0;
	if (sqlite3_exec((sqlite3*)data, c_query, on_ingredient, NULL, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	return 0;
}
