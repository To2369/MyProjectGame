#pragma once
struct ArtsData
{
	int useEnergy;
	float useEnergyTimer;
	int damage;
	float speed;
	float height;
	float radius;
	float lifeTimer;
};

namespace ArtsDatas
{
	static constexpr ArtsData skillSt =
	{
		100,0.0f,
		1,
		15.0f,
		1.0f,1.0f,
		3.0f
	};

	static constexpr ArtsData spiritEx =
	{
		300,0.0f,
		1,
		0.0f,
		1.0f,2.0f,
		3.0f
	};
}