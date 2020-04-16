#include "../../includes/sqlite3.h"
#include <algorithm>
#include <regex>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

typedef struct {
	string name;
	int relevancy;
} ingredient_result_t;

typedef struct {
	int id;
	string title;
	string description;
	string time;
	string picture;
	int relevancy;
} recipe_result_t;

typedef struct {
	string name;
	string amount;
} ingredient_t;

typedef struct {
	int id;
	string title;
	string description;
	string time;
	string picture;
	string text;
	string steps;
	vector<ingredient_t> ingredients;
} recipe_t;

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
 * Closes the database connection
 * */
void close_db(sqlite3* db)
{
	sqlite3_close(db);
}

/**
 * Callback function for ingredient search
 * */
int search_ingredient_callback(void* data, int argc, char** argv, char** azColName)
{
	ingredient_result_t* ingredient = new ingredient_result_t();
	ingredient->name = argv[0];
	ingredient->relevancy = atoi(argv[1]);
	((vector<ingredient_result_t>*)data)->push_back(*ingredient);

	return 0;
}

/**
 * Performs ingredient search by its name
 * */
vector<ingredient_result_t> search_ingredient(char* name)
{
	//Defining things
	char* errorMessage = 0;
	sqlite3* db = connect_db();
	//Sanitizing input
	string sanitized = sanitize_string(name);

	//Building query
	string query = "SELECT name, (CASE WHEN name LIKE '"
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
			 "  LIMIT 10;";

	//Creating resulting vector
	vector<ingredient_result_t>* ingredients = new vector<ingredient_result_t>();

	//Executing the query
	if (sqlite3_exec(
			db, query.c_str(), search_ingredient_callback, ingredients, &errorMessage)
		!= SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}
	close_db(db);

	return *ingredients;
}

/**
 * Callback function for recipe get
 * */
int get_recipe_callback(void* data, int argc, char** argv, char** azColName)
{
	recipe_t* recipe = (recipe_t*)data;
	recipe->id = atoi(argv[0]);
	recipe->title = argv[1];
	recipe->description = argv[2];
	recipe->time = argv[3];
	recipe->picture = argv[4];
	recipe->text = argv[5];
	recipe->steps = argv[6];

	return 0;
}

/**
 * Callback function for recipe ingredients get
 * */
int get_recipe_ingredients_callback(void* data, int argc, char** argv, char** azColName)
{
	recipe_t* recipe = (recipe_t*)data;
	ingredient_t* ingredient = new ingredient_t();
	ingredient->name = argv[0];
	ingredient->amount = argv[1];

	recipe->ingredients.push_back(*ingredient);

	return 0;
}

/**
 * Performs ingredient search by its name
 * */
recipe_t get_recipe(int id)
{
	//Defining things
	char* errorMessage = 0;
	sqlite3* db = connect_db();
	//Creating resulting recipe
	recipe_t* recipe = new recipe_t();

	//Building query
	string query = "SELECT name, amount FROM Ingredients"
				   "	INNER JOIN RecipeIngredients ON ingredient_id = id"
				   "	WHERE recipe_id = "
		+ to_string(id) + ";";

	//Executing the query
	if (sqlite3_exec(
			db, query.c_str(), get_recipe_ingredients_callback, recipe, &errorMessage)
		!= SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}

	//Building the second query
	query = "SELECT * FROM Recipes"
			"	WHERE id = "
		+ to_string(id) + ";";

	//Executing the second query
	if (sqlite3_exec(
			db, query.c_str(), get_recipe_callback, recipe, &errorMessage)
		!= SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}

	close_db(db);

	return *recipe;
}

/**
 * Callback function for recipe search
 * */
int search_recipe_callback(void* data, int argc, char** argv, char** azColName)
{
	recipe_result_t* recipe = new recipe_result_t();
	recipe->id = atoi(argv[0]);
	recipe->title = argv[1];
	recipe->description = argv[2];
	recipe->time = argv[3];
	recipe->picture = argv[4];
	recipe->relevancy = atoi(argv[5]);

	((vector<recipe_result_t>*)data)->push_back(*recipe);

	return 0;
}

/**
 * Performs ingredient search by its name
 * */
vector<recipe_result_t> search_recipe(vector<char*> ingredients, bool strict = false)
{
	//Defining things
	char* errorMessage = 0;
	sqlite3* db = connect_db();
	//Creating resulting recipe
	vector<recipe_result_t>* recipes = new vector<recipe_result_t>();

	//Building query
	string query = "SELECT id, title, description, time, picture,"
				   "(	SELECT COUNT(name) FROM Ingredients"
				   "	INNER JOIN RecipeIngredients ON ingredient_id = id"
				   "	WHERE recipe_id = Recipes.id AND (";

	for (auto&& ingredient : ingredients) {
		string name = sanitize_string(ingredient);
		name.erase(name.begin(), find_if(name.begin(), name.end(), [](int ch) {
			return !isspace(ch);
		}));

		query += "	name LIKE '" + name + "%' OR name LIKE '% " + name + "%' OR";
	}

	//Cut out last OR
	query = query.substr(0, query.size() - 3);

	if (strict) {
		query += ")) as relevancy "
				 "FROM Recipes WHERE "
				 "(relevancy - "
				 "("
				 "    SELECT COUNT(name) FROM Ingredients"
				 "    INNER JOIN RecipeIngredients ON ingredient_id = id"
				 "    WHERE recipe_id = Recipes.id"
				 ") >= 0) ORDER BY relevancy DESC LIMIT 20;";
	} else {
		query += ")) * 2 - "
				 "("
				 "    SELECT COUNT(name) FROM Ingredients"
				 "    INNER JOIN RecipeIngredients ON ingredient_id = id"
				 "    WHERE recipe_id = Recipes.id"
				 ") as relevancy "
				 "FROM Recipes ORDER BY relevancy DESC LIMIT 20;";
	}

	//Executing the query
	if (sqlite3_exec(
			db, query.c_str(), search_recipe_callback, recipes, &errorMessage)
		!= SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", errorMessage);
		sqlite3_free(errorMessage);
	}

	close_db(db);

	return *recipes;
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