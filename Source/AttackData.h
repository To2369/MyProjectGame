#pragma once
#include "json.hpp"
#include <optional>
#include <string>

using json = nlohmann::json;
struct AttackData
{
	float hitStartTime;							// 当たり判定が始まる時間
	float hitEndTime;							// 当たり判定が終わる時間
	float inputAcceptStartTime;					// 次の攻撃に遷移するための入力受付開始
	float inputAcceptEndTime;					// 次の攻撃に遷移するための入力受付終了
	float progressLimit;						// 次の攻撃に移る際のアニメーション進行度リミット
	float hitRadius;							// 当たり判定の大きさ
	std::string hitBoneName;					// 当たり判定を付けるボーンの名前
	int damage;

	/*アニメーションに二つ以上攻撃判定があるときの情報*/
	float secondHitStartTime;		// 二回目の当たり判定が始まる時間
	float secondHitEndTime;			// 二回目の当たり判定が終わる時間
	float secondHitRadius;			// 二回目の当たり判定の範囲
	std::string secondHitBoneName;	// 二回目の当たり判定をつけるボーンの名前
};

inline void to_json(json& j, const AttackData& atkData) {
	j = json
	{
		{"hitStartTime", atkData.hitStartTime},
		{"hitEndTime", atkData.hitEndTime},
		{"inputAcceptStartTime", atkData.inputAcceptStartTime},
		{"inputAcceptEndTime", atkData.inputAcceptEndTime},
		{"progressLimit", atkData.progressLimit},
		{"hitRadius", atkData.hitRadius},
		{"hitBoneName", atkData.hitBoneName},
		{"damage", atkData.damage},
		{"secondHitStartTime", atkData.secondHitStartTime},
		{"secondHitEndTime", atkData.secondHitEndTime},
		{"secondHitRadius", atkData.secondHitRadius},
		{"secondHitBoneName", atkData.secondHitBoneName}
	};
}

// JSONからAttackDataに変換する関数
inline void from_json(const json& j, AttackData& atkData) 
{
	j.at("hitStartTime").get_to(atkData.hitStartTime);
	j.at("hitEndTime").get_to(atkData.hitEndTime);
	j.at("inputAcceptStartTime").get_to(atkData.inputAcceptStartTime);
	j.at("inputAcceptEndTime").get_to(atkData.inputAcceptEndTime);
	j.at("progressLimit").get_to(atkData.progressLimit);
	j.at("hitRadius").get_to(atkData.hitRadius);
	j.at("hitBoneName").get_to(atkData.hitBoneName);
	j.at("damage").get_to(atkData.damage);

	// optional
	if (j.contains("secondHitStartTime")) j.at("secondHitStartTime").get_to(atkData.secondHitStartTime);
	if (j.contains("secondHitEndTime")) j.at("secondHitEndTime").get_to(atkData.secondHitEndTime);
	if (j.contains("secondHitRadius")) j.at("secondHitRadius").get_to(atkData.secondHitRadius);
	if (j.contains("secondHitBoneName")) j.at("secondHitBoneName").get_to(atkData.secondHitBoneName);
}

std::unordered_map<std::string, AttackData> LoadAttackData(const std::string& filePath);