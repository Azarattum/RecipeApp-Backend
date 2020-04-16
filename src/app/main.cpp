#include "database.cpp"
#include <iostream>

int main(int argc, char const* argv[])
{
	std::locale(locale("ru_RU.UTF-8"));

	///TEMP (Code examples)
	//                        ===API===

	//Search any ingredient
	vector<ingredient_result_t> ingredient_results = search_ingredient((char*)"масл");

	//Get any recipe
	recipe_t recipe = get_recipe(14880);

	//Search by ingredients                                        ↓ strict mode
	vector<char*> ingredients = { (char*)"хлеб" };
	vector<recipe_result_t> results = search_recipe(ingredients, false);

	//                         ===PRINTS===
	//Ingredients
	for (auto&& result : ingredient_results) {
		std::cout << result.name << endl;
		std::cout << result.relevancy << endl;
	}

	//Recipe
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

	//Recipe search
	for (auto&& result : results) {
		std::cout << result.id << endl;
		std::cout << result.title << endl;
		std::cout << result.description << endl;
		std::cout << result.time << endl;
		std::cout << result.picture << endl;
		std::cout << result.relevancy << endl;
	}
	return 0;
}
