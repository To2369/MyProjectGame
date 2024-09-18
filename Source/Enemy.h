#pragma once
#include<d3d11.h>
#include "Character.h"

//“G
class Enemy :public Character
{
public:
	Enemy(){}
	~Enemy() {} ;

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* dc) = 0;

	virtual void DrawDebugPrimitive();
};