#include "../../includes/sqlite3.h"
#include <stdio.h>

/**
 * Script for creating empty database tables
 * */
int main(int argc, char const* argv[])
{
	sqlite3* db;
	if (sqlite3_open("../data/database.db", &db)) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	printf("Database opened!\n");

	//Recipes table
	printf("Creating Recipes table...\n");
	char* query = (char*)"CREATE TABLE IF NOT EXISTS Recipes ("
						 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
						 "title TEXT NOT NULL,"
						 "description TEXT NOT NULL,"
						 "time TEXT NOT NULL,"
						 "picture TEXT NOT NULL,"
						 "text TEXT NOT NULL,"
						 "steps TEXT NOT NULL );";

	char* errorMessage = 0;
	if (sqlite3_exec(db, query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	//Ingredients table
	printf("Creating Ingredients table...\n");
	query = (char*)"CREATE TABLE IF NOT EXISTS Ingredients ("
				   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
				   "name TEXT NOT NULL,"
				   "UNIQUE(name) );";

	if (sqlite3_exec(db, query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	//RecipeIngredients table
	printf("Creating RecipeIngredients table...\n");
	query = (char*)"CREATE TABLE IF NOT EXISTS RecipeIngredients ("
				   "recipe_id INTEGER NOT NULL,"
				   "ingredient_id INTEGER NOT NULL,"
				   "count INTEGER NOT NULL,"
				   "	PRIMARY KEY (recipe_id, ingredient_id) );";

	if (sqlite3_exec(db, query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	printf("All done! Closing the database...\n");
	sqlite3_close(db);
	return 0;
}