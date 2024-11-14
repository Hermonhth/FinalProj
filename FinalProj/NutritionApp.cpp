#include "NutritionApp.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <algorithm>


void saveRequestsToFile(const std::vector<App>& requests, const std::string& fileName)
{
	std::ofstream ostream(fileName);
	ostream << requests.size();

	for (const App& request : requests) {
		std::string description = request.description;

		ostream << '\n' << description;
	}
}

std::vector<App> loadRequestsFromFile(const std::string& fileName)
{
	if (!std::filesystem::exists(fileName)) {
		return std::vector<App>();
	}

	std::vector<App> requests;
	std::ifstream istream(fileName);

	int n;
	istream >> n;

	for (int i = 0; i < n; i++) {
		std::string description;

		istream >> description;
		requests.push_back(App{ description });
	}

	return requests;
}
