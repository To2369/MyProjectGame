#pragma once
struct AttackData
{
	float hitStartTime;							// 当たり判定が始まる時間
	float hitEndTime;							// 当たり判定が終わる時間
	float inputAcceptStartTime;					// 次の攻撃に遷移するための入力受付開始
	float inputAcceptEndTime;					// 次の攻撃に遷移するための入力受付終了
	float progressLimit;						// 次の攻撃に移る際のアニメーション進行度リミット
	float hitRadius;							// 当たり判定の大きさ
	const char* hitBoneName;					// 当たり判定を付けるボーンの名前
	int damage;

	/*アニメーションに二つ以上攻撃判定があるときの情報*/
	float secondHitStartTime = 0.0f;			// 二回目の当たり判定が始まる時間
	float secondHitEndTime = 0.0f;				// 二回目の当たり判定が終わる時間
	const char* secondHitBoneName = nullptr;	// 二回目の当たり判定をつけるボーンの名前
};

// 攻撃に使用しているデータ
namespace AttackDatas
{
	// 弱攻撃
	static constexpr AttackData weakAttack01 =
	{
		0.4f, 0.45f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData weakAttack02 = 
	{
		0.25f, 0.3f,
		0.1f, 0.8f,
		0.6f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack03 = 
	{ 
		0.38f, 0.45f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_l",
		1
	};
	static constexpr AttackData weakAttack04 = 
	{
		0.2f,  0.4f,
		0.1f, 0.9f,
		0.4f,
		0.7f, "calf_r",
		1
	};
	static constexpr AttackData weakAttack05 = 
	{
		0.27f, 0.31f,
		0.1f, 1.0f,
		0.7f,
		0.7f, "calf_r",
		1,
		0.55f, 0.73f,
		"hand_l" 
	};
	static constexpr AttackData weakAttack06 = 
	{ 
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};

	// 強攻撃
	static constexpr AttackData strongAttack01 =
	{
		0.4f, 0.9f,
		0.1f, 1.4f,
		0.5f,
		0.7f, "lowerarm_l",
		1
	};
	static constexpr AttackData strongAttack02 =
	{
		0.45f, 0.7f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData strongAttack03 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData strongAttack04 =
	{
		0.4f, 0.55f,
		0.1f, 0.8f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData strongAttack05 =
	{
		0.45f, 0.6f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData strongAttack06 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData strongAttack07 =
	{
		0.25f, 0.3f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_r",
		1,
		0.5f, 0.75f,
		"ball_r"
	};
	static constexpr AttackData strongAttack08 =
	{
		0.35f, 0.5f,
		0.1f, 0.8f,
		0.5f,
		1
	};
}