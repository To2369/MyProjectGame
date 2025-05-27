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
	float secondHitRadius = 0.0f;
	const char* secondHitBoneName = nullptr;	// 二回目の当たり判定をつけるボーンの名前
};

// 攻撃に使用しているデータ
namespace PlAttackDatas
{
	// 弱攻撃
	static constexpr AttackData strongAttack01 =
	{
		0.4f, 0.45f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData strongAttack02 =
	{
		0.25f, 0.3f,
		0.1f, 0.8f,
		0.6f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData strongAttack03 =
	{
		0.38f, 0.45f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_l",
		1
	};
	static constexpr AttackData strongAttack04 =
	{
		0.2f,  0.4f,
		0.1f, 0.9f,
		0.4f,
		0.7f, "calf_r",
		1
	};
	static constexpr AttackData strongAttack05 =
	{
		0.27f, 0.31f,
		0.1f, 1.0f,
		0.7f,
		0.9f, "calf_r",
		1,
		0.55f, 0.73f,
		0.9f,"hand_l"
	};
	static constexpr AttackData  strongAttack06 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};

	// 強攻撃
	static constexpr AttackData weakAttack01_1 =
	{
		0.4f, 0.9f,
		0.1f, 1.4f,
		0.5f,
		0.7f, "lowerarm_l",
		1
	};
	static constexpr AttackData weakAttack01_2 =
	{
		0.45f, 0.7f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack01_3 =
	{
		0.3f, 0.4f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1,
		0.5f, 0.65f,
		1.0f, "calf_r"
	};
	static constexpr AttackData weakAttack01_4 =
	{
		0.4f, 0.55f,
		0.1f, 0.8f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack01_5 =
	{
		0.45f, 0.6f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_l",
		1
	};
	static constexpr AttackData weakAttack01_6 =
	{
		0.4f, 0.5f,
		0.1f, 0.9f,
		0.5f,
		0.7f, "ball_r",
		1
	};
	static constexpr AttackData weakAttack01_7 =
	{
		0.25f, 0.3f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "hand_r",
		1,
		0.5f, 0.75f,
		0.7f, "ball_r"
	};
	static constexpr AttackData weakAttack01_8 =
	{
		0.3f, 0.5f,
		0.1f, 0.8f,
		0.5f,
		0.7f,"calf_r",
		1
	};

	static constexpr AttackData weakAttack02_1 =
	{
		0.4f, 0.9f,
		0.1f, 1.4f,
		0.5f,
		0.7f, "lowerarm_l",
		1
	};
	static constexpr AttackData weakAttack02_2 =
	{
		0.45f, 0.7f,
		0.1f, 1.0f,
		0.5f,
		0.7f, "ball_r",
		1
	};
}