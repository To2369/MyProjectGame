#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "Misc.h"
#include "HighResolutionTimer.h"

#include <d3d11.h>
#include"Graphics/Graphics.h"
#include"Graphics/Sprite.h"
#include "Input/InputManager.h"
#ifdef USE_IMGUI
#include <imgui.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LPCWSTR APPLICATION_NAME{ L"����" };

class Framework
{
public:
	Framework(HWND hwnd);
	~Framework();

	/*Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;*/

	int Run();

	LRESULT CALLBACK HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	void Update(float elapsed_Time/*Elapsed seconds from last frame*/);
	void Render(float elapsed_Time/*Elapsed seconds from last frame*/);

	void CalculateFrameStats();

private:
	const HWND				hWnd;
	HDC						hDC;
	HighResolutionTimer timer;
	uint32_t frames{ 0 };
	float elapsedTime{ 0.0f };
	Graphics* graphics;
	const int	syncInterval = 1;		// ���������Ԋu�ݒ�

	InputManager* inputMgr;
};

