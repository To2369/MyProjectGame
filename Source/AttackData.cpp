#include <fstream>
#include <unordered_map>
#include "AttackData.h"
std::unordered_map<std::string, AttackData> LoadAttackData(const std::string& filePath) {
	std::ifstream inFile(filePath);
	json j;
	inFile >> j;

	std::unordered_map<std::string, AttackData> attackMap;

	for (auto& [name, data] : j.items()) {
		attackMap[name] = data.get<AttackData>();
	}

	return attackMap;
}