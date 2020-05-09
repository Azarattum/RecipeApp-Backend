#include "../../includes/argparse.hpp"
#include "database.cpp"
#include "server.cpp"
#include <codecvt>
#include <iostream>

using namespace std;

int main(int argc, char const* argv[])
{
	locale(locale("ru_RU.UTF-8"));

	argparse::ArgumentParser parser;
	parser.addArgument("-h", "--help", 0);
	parser.addArgument("-i", "--ingredient", 1);
	parser.addArgument("-s", "--search", '+');
	parser.addArgument("--strict", 0);
	parser.addArgument("-r", "--recipe", 1);
	parser.addArgument("--serve", 1);
	parser.parse(argc, argv);

	if (parser.exists("help") || parser.empty() || argc <= 1) {
		cout << parser.usage() << endl;
		return 0;
	}

	//Start the server
	if (parser.exists("serve")) {
		int port = parser.retrieve<int>("serve");
		if (port) {
			cout << "================= RECIPE =================" << endl;
			cout << endl;

			start_server(port);
		} else {
			cout << "Invalid recipe id" << endl;
			return 1;
		}
	}

	//Search any ingredient
	if (parser.exists("ingredient")) {
		cout << "================= RECIPE =================" << endl;
		cout << endl;
		cout << "Доступные ингредиенты:" << endl;
		string ing_result = parser.retrieve<string>("i");
		if (ing_result.length() > 0) {
			vector<ingredient_result_t> ingredient_results = search_ingredient((char*)ing_result.c_str());
			//Ingredients
			for (auto&& result : ingredient_results) {
				cout << "  " << result.name << endl;
			}
		}
		cout << endl;
		cout << "================= RECIPE =================" << endl;
	}

	//Get any recipe
	if (parser.exists("recipe")) {
		int id = parser.retrieve<int>("recipe");
		if (id) {
			recipe_t recipe = get_recipe(id);
			//Recipe
			if (recipe.id > 0) {
				cout << "================= RECIPE =================" << endl;
				cout << endl;
				cout << "№ " << id << ") " << recipe.title << endl;
				cout << "[" << recipe.time << "]" << endl
					 << endl;
				cout << "Ингредиенты:" << endl;
				for (auto&& ingredient : recipe.ingredients) {
					cout << "  " << ingredient.name << ": " << ingredient.amount << endl;
				}
				cout << endl;
				cout << recipe.description << endl
					 << endl;
				cout << recipe.text << endl;
				cout << endl;
				cout << "================= RECIPE =================" << endl;
			} else {
				cout << "Recipe not found" << endl;
			}
		} else {
			cout << "Invalid recipe id" << endl;
			return 1;
		}
	}

	//Search for recipes
	if (parser.exists("search")) {
		cout << "================= RECIPE =================" << endl;
		cout << endl;

		vector<string> ingredients = parser.retrieve<vector<string>>("search");
		vector<char*>* ing_string = new vector<char*>();
		for (auto&& result : ingredients) {
			ing_string->push_back((char*)result.c_str());
		}

		vector<recipe_result_t> results = search_recipe(*ing_string, parser.exists("strict"));
		for (auto&& result : results) {
			cout << "№ " << result.id << ") " << result.title << endl;
			cout << "    [" << result.time << "]" << endl;

			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			wstring description = converter.from_bytes(result.description);

			cout << "    " << converter.to_bytes(description.substr(0, 80));
			if (result.description.length() > 100) {
				cout << "...";
			}
			cout << endl
				 << endl;
		}

		cout << "================= RECIPE =================" << endl;
	}

	return 0;
}
