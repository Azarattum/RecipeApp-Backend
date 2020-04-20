#include "../../includes/crow.h"
#include "database.cpp"
#include <string>
#include <unistd.h>

using namespace std;
using namespace crow;

struct UTFMiddleware {
	struct context {
	};

	void before_handle(request&, response&, context&)
	{
	}

	void after_handle(request&, response& res, context&)
	{
		if (res.get_header_value("content-type") == "") {
			res.add_header("content-type", "application/json; charset=UTF-8");
		}
	}
};

using RecipeApp = Crow<UTFMiddleware>;

void serve_api(RecipeApp*);
void serve_static(RecipeApp*);
string url_decode(string&);

std::unordered_map<std::string, std::string> MIMETYPES = {
	{ "aac", "audio/aac" },
	{ "abw", "application/x-abiword" },
	{ "arc", "application/x-freearc" },
	{ "avi", "video/x-msvideo" },
	{ "azw", "application/vnd.amazon.ebook" },
	{ "bin", "application/octet-stream" },
	{ "bmp", "image/bmp" },
	{ "bz", "application/x-bzip" },
	{ "bz2", "application/x-bzip2" },
	{ "csh", "application/x-csh" },
	{ "css", "text/css" },
	{ "csv", "text/csv" },
	{ "doc", "application/msword" },
	{ "docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document" },
	{ "eot", "application/vnd.ms-fontobject" },
	{ "epub", "application/epub+zip" },
	{ "gz", "application/gzip" },
	{ "gif", "image/gif" },
	{ "htm", "text/html" },
	{ "html", "text/html" },
	{ "ico", "image/vnd.microsoft.icon" },
	{ "ics", "text/calendar" },
	{ "jar", "application/java-archive" },
	{ "jpeg ", "image/jpeg" },
	{ "jpg ", "image/jpeg" },
	{ "js", "text/javascript" },
	{ "json", "application/json" },
	{ "jsonld", "application/ld+json" },
	{ "mid ", " audio/midi" },
	{ "midi ", " audio/midi" },
	{ "mjs", "text/javascript" },
	{ "mp3", "audio/mpeg" },
	{ "mpeg", "video/mpeg" },
	{ "mpkg", "application/vnd.apple.installer+xml" },
	{ "odp", "application/vnd.oasis.opendocument.presentation" },
	{ "ods", "application/vnd.oasis.opendocument.spreadsheet" },
	{ "odt", "application/vnd.oasis.opendocument.text" },
	{ "oga", "audio/ogg" },
	{ "ogv", "video/ogg" },
	{ "ogx", "application/ogg" },
	{ "opus", "audio/opus" },
	{ "otf", "font/otf" },
	{ "png", "image/png" },
	{ "pdf", "application/pdf" },
	{ "php", "application/php" },
	{ "ppt", "application/vnd.ms-powerpoint" },
	{ "pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation" },
	{ "rar", "application/vnd.rar" },
	{ "rtf", "application/rtf" },
	{ "sh", "application/x-sh" },
	{ "svg", "image/svg+xml" },
	{ "swf", "application/x-shockwave-flash" },
	{ "tar", "application/x-tar" },
	{ "tif ", "image/tiff" },
	{ "tiff ", "image/tiff" },
	{ "ts", "video/mp2t" },
	{ "ttf", "font/ttf" },
	{ "txt", "text/plain" },
	{ "vsd", "application/vnd.visio" },
	{ "wav", "audio/wav" },
	{ "weba", "audio/webm" },
	{ "webm", "video/webm" },
	{ "webp", "image/webp" },
	{ "woff", "font/woff" },
	{ "woff2", "font/woff2" },
	{ "xhtml", "application/xhtml+xml" },
	{ "xls", "application/vnd.ms-excel" },
	{ "xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" },
	{ "xml", "application/xml" },
	{ "xul", "application/vnd.mozilla.xul+xml" },
	{ "zip", "application/zip" },
	{ "3gp", "video/3gpp" }
};

/**
 * Starts the HTTP server
 * */
void start(int port = 8000)
{
	RecipeApp app;

	serve_api(&app);
	serve_static(&app);

	app.port(port);
	if (access("fullchain.pem", F_OK) != -1 && access("privkey.pem", F_OK) != -1) {
		app.ssl_file("fullchain.pem", "privkey.pem");
	}
	app.multithreaded().run();
}

/**
 * Registers all application API routes
 * */
void serve_api(RecipeApp* app)
{
	CROW_ROUTE((*app), "/api/ingredients/search/<string>")
	([](string query) {
		char* name = (char*)url_decode(query).c_str();
		vector<ingredient_result_t> results = search_ingredient(name);

		crow::json::wvalue json;
		//Ingredients
		int i = 0;
		for (auto&& result : results) {
			json[i]["id"] = result.name;
			json[i]["relevancy"] = result.relevancy;
			i++;
		}

		return json::dump(json);
	});

	CROW_ROUTE((*app), "/api/recipe/search/<string>")
	([](string query) {
		vector<char*> ingredients;
		string items = url_decode(query);
		string delimiter = "&";

		//Parse ingredients input
		size_t pos = 0;
		string token;
		while ((pos = items.find(delimiter)) != std::string::npos) {
			token = items.substr(0, pos);
			items.erase(0, pos + delimiter.length());
			if (token.length() > 0)
				ingredients.push_back((char*)token.c_str());
		}
		ingredients.push_back((char*)items.c_str());

		//Look for recipe
		vector<recipe_result_t> results = search_recipe(ingredients, false);

		crow::json::wvalue json;
		//Ingredients
		int i = 0;
		//Recipe search
		for (auto&& result : results) {
			json[i]["id"] = result.id;
			json[i]["title"] = result.title;
			json[i]["description"] = result.description;
			json[i]["time"] = result.time;
			json[i]["picture"] = result.picture;
			json[i]["relevancy"] = result.relevancy;
			i++;
		}

		return json::dump(json);
	});

	CROW_ROUTE((*app), "/api/recipe/search/<string>/strict")
	([](string query) {
		vector<char*> ingredients;
		string items = url_decode(query);
		string delimiter = "&";

		//Parse ingredients input
		size_t pos = 0;
		string token;
		while ((pos = items.find(delimiter)) != std::string::npos) {
			token = items.substr(0, pos);
			items.erase(0, pos + delimiter.length());
			if (token.length() > 0)
				ingredients.push_back((char*)token.c_str());
		}
		ingredients.push_back((char*)items.c_str());

		//Look for recipe
		vector<recipe_result_t> results = search_recipe(ingredients, true);

		crow::json::wvalue json;
		//Ingredients
		int i = 0;
		//Recipe search
		for (auto&& result : results) {
			json[i]["id"] = result.id;
			json[i]["title"] = result.title;
			json[i]["description"] = result.description;
			json[i]["time"] = result.time;
			json[i]["picture"] = result.picture;
			json[i]["relevancy"] = result.relevancy;
			i++;
		}

		return json::dump(json);
	});

	CROW_ROUTE((*app), "/api/recipe/get/<int>")
	([](int id) {
		crow::json::wvalue json;
		recipe_t recipe = get_recipe(id);

		if (recipe.id == 0)
			return json::dump(json);

		//Recipe
		json["id"] = recipe.id;
		json["title"] = recipe.title;
		json["description"] = recipe.description;
		json["time"] = recipe.time;
		json["picture"] = recipe.picture;
		json["text"] = recipe.text;
		json["steps"] = recipe.steps;

		//Ingredients
		int i = 0;
		for (auto&& ingredient : recipe.ingredients) {
			json["ingredients"][i]["name"] = ingredient.name;
			json["ingredients"][i]["amount"] = ingredient.amount;
			i++;
		}

		return json::dump(json);
	});
}

/**
 * Serves any static content from ./public directory
 * */
void serve_static(RecipeApp* app)
{
	//Load static resource
	CROW_ROUTE((*app), "/<path>")
	([](const request& req, response& res, string path) {
		if (path.find("..") != string::npos) {
			res.code = 403;
		} else {
			ifstream in("public/" + path, ifstream::in);
			if (in) {
				ostringstream contents;
				contents << in.rdbuf();
				in.close();

				//Add content header depending on MIME type
				int index = path.find_last_of(".");
				string extension = path.substr(index + 1);
				res.add_header("content-type", MIMETYPES[extension] + "; charset=UTF-8");

				res.write(contents.str());
			} else {
				res.code = 404;
			}
		}
		res.end();
	});

	//Serve index.html on default route
	CROW_ROUTE((*app), "/")
	([](const request& req, response& res) {
		ifstream in("public/index.html", ifstream::in);
		if (in) {
			ostringstream contents;
			contents << in.rdbuf();
			in.close();
			res.add_header("content-type", "text/html; charset=UTF-8");
			res.write(contents.str());
		} else {
			res.code = 404;
		}
		res.end();
	});
}

/**
 * Decodes URL string
 * */
string url_decode(string& source)
{
	string result;
	char ch;
	int ii;
	for (int i = 0; i < (int)source.length(); i++) {
		if (int(source[i]) == 37) {
			sscanf(source.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			result += ch;
			i = i + 2;
		} else {
			result += source[i];
		}
	}
	return (result);
}
