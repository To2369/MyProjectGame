#include "Framework.h"
#include "SceneTitle.h"
#include "SceneTest.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Effect/EffectManager.h"
Framework::Framework(HWND hwnd) : hWnd(hwnd)
{
	hDC = GetDC(hwnd);

	// �f�o�C�X�Ǘ��̏�����
	graphics = Graphics::Instance();
	graphics->Initialize(hwnd);

	inputMgr = InputManager::Instance()->initialize(hwnd);

	EffectManager::Instance().Initialize();

	//�V�[��������
	SceneManager::Instance().ChangeScene(new SceneGame);
}

Framework::~Framework()
{
	//�V�[���I����
	SceneManager::Instance().Clear();

	EffectManager::Instance().Finalize();

	ReleaseDC(hWnd, hDC);
}

void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	inputMgr->update();

	// �V�[���X�V����
	SceneManager::Instance().Update(elapsedTime);
}

void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{

	std::lock_guard<std::mutex> lock(Graphics::Instance()->GetMutex());

	ID3D11DeviceContext* dc = graphics->GetDeviceContext();

	graphics->Clear(0, 0, 1, 1);

	graphics->SetRenderTargets();

	// �V�[���`�揈��
	SceneManager::Instance().Render();

	graphics->Present(syncInterval);
}

// �t���[�����[�g�v�Z
void Framework::CalculateFrameStats()
{
	if (++frames, (timer.TimeStamp() - elapsedTime) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
		SetWindowTextW(hWnd, outs.str().c_str());

		frames = 0;
		elapsedTime += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::Run()
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
			CalculateFrameStats();
			Update(timer.TimeInterval());
			Render(timer.TimeInterval());
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

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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