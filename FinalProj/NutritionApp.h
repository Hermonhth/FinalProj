#pragma once
#include <string>
#include <vector>


struct App
{
	std::string description;
};

void saveRequestsToFile(const std::vector<App>& requests, const std::string& fileName);
std::vector<App> loadRequestsFromFile(const std::string& fileName);