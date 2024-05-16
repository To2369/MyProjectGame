#include "Framework.h"
#include "SceneTitle.h"
#include "SceneManager.h"
Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
	// デバイス管理の初期化
	deviceMgr = DeviceManager::Instance()->Initialize(hwnd);

	//シーン初期化
	SceneManager::Instance().ChangeScene(new SceneTitle);
}

Framework::~Framework()
{
	//シーン終了化
	SceneManager::Instance().Clear();
}

void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}
void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	ID3D11DeviceContext* dc = deviceMgr->GetDeviceContext();

	// シーン描画処理
	SceneManager::Instance().Render();

	// バックバッファに描画した画を画面に表示する。
	deviceMgr->GetSwapChain()->Present(syncInterval, 0);

}

// フレームレート計算
void Framework::Calculate_frame_stats()
{
	if (++frames, (timer.time_stamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
		SetWindowTextW(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

// アプリケーションループ
int Framework::run()
{
	MSG msg{};

	ID3D11DeviceContext* dc = deviceMgr->GetDeviceContext();
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(deviceMgr->GetDevice(), deviceMgr->GetDeviceContext());
	ImGui::StyleColorsDark();
#endif

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.Tick();
			Calculate_frame_stats();
			Update(timer.time_interval());
			Render(timer.time_interval());
		}
	}

#ifdef USE_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#if 1
	BOOL fullscreen = 0;
	deviceMgr->GetSwapChain()->GetFullscreenState(&fullscreen, 0);
	if (fullscreen)
	{
		deviceMgr->GetSwapChain()->SetFullscreenState(FALSE, 0);
	}
#endif

	return static_cast<int>(msg.wParam);
}

// メッセージハンドラ
LRESULT CALLBACK Framework::Handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			timer.Stop();
			break;
		case WM_EXITSIZEMOVE:
			timer.Start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}
}