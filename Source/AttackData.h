#pragma once
struct AttackData
{
	float hitStartTime;
	float hitEndTime;
	float inputAcceptStartTime;
	float inputAcceptEndTime;
	float progressLimit;
	float hitRadius;
	const char* hitBoneName;

	/*アニメーションに二つ以上攻撃判定があるときの情報*/
	float secondHitStartTime = 0.0f;
	float secondHitEndTime = 0.0f;
	const char* secondHitBoneName = nullptr;
};
namespace AttackDatas
{
	static constexpr AttackData weakAttack01 =
	{
		0.4f, 0.45f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_l"
	};
	static constexpr AttackData weakAttack02 = 
	{
		0.25f, 0.3f,
		0.1f, 0.8f,
		0.6f,
		0.7f, "ball_r" 
	};
	static constexpr AttackData weakAttack03 = 
	{ 
		0.38f, 0.45f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_l"
	};
	static constexpr AttackData weakAttack04 = 
	{
		0.2f,  0.4f,
		0.1f, 0.9f,
		0.4f,
		0.7f, "calf_r" 
	};
	static constexpr AttackData weakAttack05 = 
	{
		0.27f, 0.31f,
		0.1f, 1.0f,
		0.7f,
		0.7f, "calf_r",
		0.55f, 0.73f,
		"hand_l" 
	};
	static constexpr AttackData weakAttack06 = 
	{ 
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r"
	};
}