#pragma once
#include<d3d11.h>
#include "Character.h"
#include "StateMachine.h"
//“G
class Enemy :public Character
{
public:
	Enemy(){}
	~Enemy()override {}

	virtual void Update(float elapsedTime) = 0;

	virtual void Render(ID3D11DeviceContext* dc) = 0;

	virtual void DrawDebugGUI() = 0;

	virtual void DrawDebugPrimitive();

	//”jŠü
	void Destroy();
private:
	std::unique_ptr<StateMachine<Enemy>> stateMachine;
};