#include "../app/database.cpp"
#include <cxxtest/TestSuite.h>

class IncorrectSymbolsTestSuite : public CxxTest::TestSuite {
public:
	void testPartialIncorrectSymbols(void)
	{
		//Allowed symbols
		//ёйцукенгшщзхъфывапролджэячсмитьбюЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮЁa-z,-.0-9
		vector<char*> ingredients = { (char*)"хлеб" };
		vector<recipe_result_t> results = search_recipe(ingredients, false);

		vector<char*> ingredients_2 = { (char*)"х%л+е!б" };
		vector<recipe_result_t> results_2 = search_recipe(ingredients, false);

		TS_ASSERT_EQUALS(results.size(), results_2.size());

		for (int i = 0; i < results.size(); i++) {
			recipe_result_t result = results[i];
			recipe_result_t result_2 = results_2[i];

			TS_ASSERT_EQUALS(result.id, result_2.id);
			TS_ASSERT_EQUALS(result.title, result_2.title);
			TS_ASSERT_EQUALS(result.description, result_2.description);
			TS_ASSERT_EQUALS(result.time, result_2.time);
			TS_ASSERT_EQUALS(result.picture, result_2.picture);
			TS_ASSERT_EQUALS(result.relevancy, result_2.relevancy);
		}
	}

	void testFullIncorrectSymbols(void)
	{
	}

	void testNoSymbols(void)
	{
	}
};