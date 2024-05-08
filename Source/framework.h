#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "Misc.h"
#include "High_resolution_timer.h"

#include <d3d11.h>
#include"DeviceManager.h"
#include"Graphics/Sprite.h"
#ifdef USE_IMGUI

#include "../Imgui/imgui.h"
#include "../Imgui/imgui_internal.h"
#include "../Imgui/imgui_impl_dx11.h"
#include "../Imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

CONST LPCWSTR APPLICATION_NAME{ L"MyProjectGame" };

class Framework
{
public:
	Framework(HWND hwnd);
	~Framework();

	/*Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;*/

	int run();

	LRESULT CALLBACK Handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
	void Update(float elapsed_Time/*Elapsed seconds from last frame*/);
	void Render(float elapsed_Time/*Elapsed seconds from last frame*/);

	void Calculate_frame_stats();

private:
	const HWND				hwnd;

	High_resolution_timer timer;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	DeviceManager* deviceMgr;
	const int	syncInterval = 1;		// êÇíºìØä˙ä‘äuê›íË
};

