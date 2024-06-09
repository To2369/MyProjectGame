#include "Framework.h"
#include "SceneTitle.h"
#include "SceneTest.h"
#include "SceneManager.h"
Framework::Framework(HWND hwnd) : hWnd(hwnd)
{
	hDC = GetDC(hwnd);

	// デバイス管理の初期化
	graphics = Graphics::Instance();
	graphics->Initialize(hwnd);

	//シーン初期化
	SceneManager::Instance().ChangeScene(new SceneTest);
}

Framework::~Framework()
{
	//シーン終了化
	SceneManager::Instance().Clear();

	ReleaseDC(hWnd, hDC);
}

void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	ID3D11DeviceContext* dc = graphics->GetDeviceContext();

	graphics->Clear(0, 0, 0, 1);

	graphics->SetRenderTargets();

	// シーン描画処理
	SceneManager::Instance().Render();

	graphics->Present(syncInterval);
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
		SetWindowTextW(hWnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

// アプリケーションループ
int Framework::run()
{
	MSG msg{};

	ID3D11DeviceContext* dc = graphics->GetDeviceContext();
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(graphics->GetDevice(), graphics->GetDeviceContext());
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
	graphics->GetSwapChain()->GetFullscreenState(&fullscreen, 0);
	if (fullscreen)
	{
		graphics->GetSwapChain()->SetFullscreenState(FALSE, 0);
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