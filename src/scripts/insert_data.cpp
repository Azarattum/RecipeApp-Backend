#include "../../includes/sqlite3.h"
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>

using namespace std;

int on_ingredient(void* data, int argc, char** argv, char** azColName);
int on_recipe(void* data, int argc, char** argv, char** azColName);
int counter;

const regex html_regex(
	R"~(<[^>]*>)~",
	regex::ECMAScript | regex::icase | regex::collate | regex::optimize);
const regex spaces_regex(
	R"~(\s+|\n+)~",
	regex::ECMAScript | regex::icase | regex::collate | regex::optimize);
const regex separator_regex(
	R"~(=[|]{3}=)~",
	regex::ECMAScript | regex::icase | regex::collate | regex::optimize);
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

	//Fill ingredients
	counter = 0;
	printf("Generating ingredients from raw data...\n");
	char* query = (char*)"SELECT ingredients FROM recipe_part_1 LIMIT 1;";

	if (sqlite3_exec(raw, query, on_ingredient, db, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\n");

	//Fill recipes
	counter = 0;
	printf("Generating recipes from raw data...\n");
	query = (char*)"SELECT"
				   "    title, description, time, image, step_description, step_img "
				   "FROM recipe_part_1 LIMIT 1;";

	if (sqlite3_exec(raw, query, on_recipe, db, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\n");

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
	if (sqlite3_exec((sqlite3*)data, c_query, NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\rProccessed %d recipes...      ", ++counter);

	return 0;
}

/**
 * Callback when recipe is selected
 * */
int on_recipe(void* data, int argc, char** argv, char** azColName)
{
	//Define values
	char* title = argv[0];
	string description(argv[1]);
	char* time = argv[2];
	char* picture = argv[3];
	string text(argv[4]);
	string steps(argv[5]);

	//Clear description
	description = regex_replace(description, html_regex, "");
	description = regex_replace(description, spaces_regex, " ");

	//Format text
	text = regex_replace(text, html_regex, "");
	text = regex_replace(text, spaces_regex, " ");
	text = regex_replace(text, separator_regex, "\n");

	//Format steps
	steps = regex_replace(steps, separator_regex, "\",\"");

	//Prepare query
	string query = "INSERT OR IGNORE INTO Recipes"
				   "    (title,description,time,picture,text,steps) "
				   "VALUES (\"";
	query.append(title);
	query.append("\",\"");
	query.append(description);
	query.append("\",\"");
	query.append(time);
	query.append("\",\"");
	query.append(picture);
	query.append("\",\"");
	query.append(text);
	query.append("\",'[\"");
	query.append(steps);
	query.append("\"]');");

	//Execute the query to insert items
	char* errorMessage = 0;
	if (sqlite3_exec((sqlite3*)data, query.c_str(), NULL, 0, &errorMessage) != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
		return 1;
	}
	printf("\rProccessed %d recipes...      ", ++counter);

	return 0;
}