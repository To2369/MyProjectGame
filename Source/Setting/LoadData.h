#pragma once
#include "json.hpp"
#include <fstream>
#include <unordered_map>
using json = nlohmann::json;
template<typename Data>

inline std::unordered_map<std::string, Data> LoadData(const std::string& filePath)
{
    std::ifstream inFile(filePath);
    	json j;
    	inFile >> j;
    
    	std::unordered_map<std::string, Data> Map;
    
    	for (auto& [name, data] : j.items()) {
    		Map[name] = data.get<Data>();
    	}
    
    	return Map;
}
//std::unordered_map<std::string, Data> LoadData(const std::string& filePath) 
//{
//	std::ifstream inFile(filePath);
//	json j;
//	inFile >> j;
//
//	std::unordered_map<std::string, Data> Map;
//
//	for (auto& [name, data] : j.items()) {
//		Map[name] = data.get<Data>();
//	}
//
//	return Map;
//}