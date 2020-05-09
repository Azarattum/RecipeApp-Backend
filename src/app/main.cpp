#include "../../includes/argparse.hpp"
#include "database.cpp"
#include <iostream>

int main(int argc, char const* argv[])
{
	std::locale(locale("ru_RU.UTF-8"));

	argparse::ArgumentParser par;
	//par.addArgument("-i",'+');
	par.addArgument("-r", "--recipe", 1);
	//par.addFinalArgument("output");
	par.parse(argc, argv);
	//Search any ingredient
	//char ing_result=par.retrieve<char>("i");
	/*if(ing_result)
	{
		vector<ingredient_result_t> ingredient_results = search_ingredient((char*)ing_result);
		//Ingredients
		for (auto&& result : ingredient_results) {
			std::cout << result.name << endl;
			std::cout << result.relevancy << endl;
		}
	}*/
	//Get any recipe
	//vector<float> i=par.retrieve<vector<float>>("recipe");

	int id = par.retrieve<int>("recipe"); //par.retrieve<int>("r");
	if (id) {
		recipe_t recipe = get_recipe(id);
		//Recipe
		if (recipe.id > 0) {
			std::cout << recipe.id << endl;
			std::cout << recipe.title << endl;
			std::cout << recipe.description << endl;
			std::cout << recipe.time << endl;
			std::cout << recipe.picture << endl;
			std::cout << recipe.text << endl;
			std::cout << recipe.steps << endl;
			for (auto&& ingredient : recipe.ingredients) {
				std::cout << ingredient.name << endl;
				std::cout << ingredient.amount << endl;
			}
		} else {
			std::cout << "Recipe not found" << endl;
		}
	} else {
		std::cout << "Invalid recipe id" << endl;
	}
	///TEMP (Code examples)
	//                        ===API===

	//Search by ingredients                                        ↓ strict mode
	/*vector<char*> ingredients = { (char*)"хлеб" };
	vector<recipe_result_t> results = search_recipe(ingredients, false);

	//                         ===PRINTS===
	//Recipe search
	for (auto&& result : results) {
		std::cout << result.id << endl;
		std::cout << result.title << endl;
		std::cout << result.description << endl;
		std::cout << result.time << endl;
		std::cout << result.picture << endl;
		std::cout << result.relevancy << endl;
	}*/
	return 0;
}
