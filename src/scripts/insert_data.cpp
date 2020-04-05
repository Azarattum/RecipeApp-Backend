#include "../../includes/sqlite3.h"
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>

using namespace std;

int on_ingredient(void* data, int argc, char** argv, char** azColName);
int on_recipe(void* data, int argc, char** argv, char** azColName);

int counter;
regex ingredient_regex;

/**
 * Script for converting and inserting raw data to the database
 * */
int main(int argc, char const* argv[])
{
	locale::global(locale("en_US.UTF-8"));
	//We have to declare ingredient regular expression after locale init
	const regex ex(
		R"~([>]([а-я ,%0-9]+?)\s*:\s*[^<])~",
		regex::ECMAScript | regex::icase | regex::collate | regex::optimize);
	ingredient_regex = ex;

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
	sqlite3_enable_load_extension(db, 1);

	//Fill ingredients
	counter = 0;
	printf("Generating ingredients from raw data...\n");
	char* query = (char*)"SELECT ingredients FROM recipe_part_1;";

	if (sqlite3_exec(raw, query, on_ingredient, db, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\n");

	//Fill recipes
	counter = 0;
	printf("Generating recipes from raw data...\n");
	query = (char*)"ATTACH DATABASE '../data/raw.db' as raw;"
				   "SELECT load_extension('../includes/glib_replace.so', 'sqlite3_extension_init');"
				   "INSERT OR IGNORE INTO Recipes"
				   "    SELECT"
				   "        rowid as id,"
				   "        title,"
				   "        REGEX_REPLACE('\\s+|\n+',"
				   "        REGEX_REPLACE('<[^>]*>',"
				   "            description, ''"
				   "        ), ' '"
				   "        ) as description,"
				   "        time,"
				   "        image as picture,"
				   "        REGEX_REPLACE('=[|]{3}=',"
				   "        REGEX_REPLACE('\\s+|\n+',"
				   "        REGEX_REPLACE('<[^>]*>',"
				   "            step_description, ''"
				   "        ), ' '), '\n'"
				   "        ) as text,"
				   "        '[\"' ||"
				   "        REGEX_REPLACE('=[|]{3}=',"
				   "            step_img,"
				   "        '\",\"') || '\"]' as steps"
				   "    FROM raw.recipe_part_1;";

	if (sqlite3_exec(db, query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	//Fill relations
	printf("Generating ingredient-recipe relationships from raw data...\n");
	query = (char*)"INSERT OR IGNORE INTO RecipeIngredients"
				   "	SELECT r.rowid as recipe_id, i.id as ingredient_id,"
				   "	REGEX_REPLACE('(^.*' || i.name || '[^:]*:\\s*|\\s*([А-ЯЁ]|<br/>).*$)',"
				   "		r.ingredients, '') as amount"
				   "	FROM raw.recipe_part_1 as r "
				   "	INNER JOIN Ingredients as i on"
				   "		r.ingredients LIKE ('%>' || i.name || ' :%');";

	if (sqlite3_exec(db, query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}

	printf("All done! Closing databases...\n");
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
	smatch matches;

	//Search for ingredients and create query
	string query = "INSERT OR IGNORE INTO Ingredients (name) VALUES ";
	int matched = 0;
	while (regex_search(ingredients, matches, ingredient_regex)) {
		string ingredient = matches[1];

		query.append("(\"");
		query.append(ingredient);
		query.append("\"),");

		ingredients = matches.suffix();
		matched = 1;
	}
	if (matched == 0) {
		return 0;
	}

	char* c_query = (char*)query.c_str();
	c_query[query.size() - 1] = ';';

	//Execute the query to insert items
	char* errorMessage = 0;
	if (sqlite3_exec((sqlite3*)data, c_query, NULL, 0, &errorMessage) != SQLITE_OK) {
		printf("Query: %s\n", c_query);
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\rProccessed %d recipes...      ", ++counter);

	return 0;
}