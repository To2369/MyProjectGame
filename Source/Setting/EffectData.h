#pragma once
#include "json.hpp"
#include <optional>
#include <string>
using json = nlohmann::json;

struct EffectData
{
    std::string filename;
    float scale;
};

inline void to_json(json& j, const EffectData& effData) {
	j = json
	{
		{"filename", effData.filename},
		{"scale", effData.scale}
	};
}

// JSONÇ©ÇÁAttackDataÇ…ïœä∑Ç∑ÇÈä÷êî
inline void from_json(const json& j, EffectData& effData)
{
	j.at("filename").get_to(effData.filename);

	// optional
	if (j.contains("scale")) j.at("scale").get_to(effData.scale);
}