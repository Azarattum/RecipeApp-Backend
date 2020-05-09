#include "../../includes/argparse.hpp"
#include "database.cpp"
//#include "server.cpp"
#include <iostream>

int main(int argc, char const* argv[])
{
	std::locale(locale("ru_RU.UTF-8"));

	argparse::ArgumentParser par;
	par.addArgument("-i", "--ingredient", 1);
	par.addArgument("-r", "--recipe", 1);
	par.addArgument("-s", "--search", '+');
	par.addArgument("--serve", 1);
	par.parse(argc, argv);
	//Search any ingredient
	if (par.exists("serve")) {
	}
	if (par.exists("ingredient")) {
		std::cout << endl;
		std::cout << "================= RECIPE =================" << endl;
		string ing_result = par.retrieve<string>("i");
		if (ing_result.length() > 0) {
			vector<ingredient_result_t> ingredient_results = search_ingredient((char*)ing_result.c_str());
			//Ingredients
			for (auto&& result : ingredient_results) {
				std::cout << result.name << endl;
			}
		}
		std::cout << "================= RECIPE =================" << endl;
		std::cout << endl;
	}
	//Get any recipe
	if (par.exists("recipe")) {
		int id = par.retrieve<int>("recipe");
		if (id) {
			recipe_t recipe = get_recipe(id);
			//Recipe
			if (recipe.id > 0) {
				std::cout << endl;
				std::cout << "================= RECIPE =================" << endl;
				std::cout << recipe.title << endl
						  << endl;
				std::cout << "id рецепта:" << endl;
				std::cout << id << endl
						  << endl;
				std::cout << recipe.description << endl;
				std::cout << recipe.time << endl;
				std::cout << recipe.text << endl;
				std::cout << endl
						  << "Список ингредиентов" << endl;
				for (auto&& ingredient : recipe.ingredients) {
					std::cout << ingredient.name << ": " << ingredient.amount << endl;
				}
				std::cout << "================= RECIPE =================" << endl;
				std::cout << endl;
			} else {
				std::cout << "Recipe not found" << endl;
			}
		} else {
			std::cout << "Invalid recipe id" << endl;
		}
	}
	if (par.exists("s")) {
		std::cout << endl;
		std::cout << "================= RECIPE =================" << endl;
		vector<string> ingredients = par.retrieve<vector<string>>("s");
		vector<char*>* ing_string = new vector<char*>();
		for (auto&& result : ingredients) {
			ing_string->push_back((char*)result.c_str());
		}
		vector<recipe_result_t> results = search_recipe(*ing_string, false);
		for (auto&& result : results) {
			std::cout << result.title << endl
					  << endl;
			std::cout << "id рецепта" << endl;
			std::cout << result.id << endl
					  << endl;
			std::cout << result.description << endl;
			std::cout << result.time << endl;
		}
		std::cout << "================= RECIPE =================" << endl;
		std::cout << endl;
	}

	return 0;
}
