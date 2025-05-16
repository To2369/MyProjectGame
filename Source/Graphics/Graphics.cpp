#include "../Misc.h"
#include"Graphics.h"
#include "Buffer.h"

#include "MaskShader.h"
// ����������
void Graphics::Initialize(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	// �f�o�C�X�E�f�o�C�X�R���e�L�X�g�E�X���b�v�`�F�[���̍쐬
	HRESULT hr{ S_OK };
	{
		UINT createDeviceFlags{ 0 };
#ifdef _DEBUG
		createDeviceFlags != D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
		// Device,Context�̔\�̓��x���̐ݒ�
		// DirectX11�̊�����S�ɖ�������GPU��œ��삷�郌�x���ɐݒ�
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			{D3D_FEATURE_LEVEL_11_0}
		};

		// IDXGISwapChain �̐ݒ���s�����߂̍\���́BSwapChain �쐬���邽�߂ɕK�v�Ȑݒ���s��
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		{
			swapChainDesc.BufferCount = 1;								// �X���b�v�`�F�C���̃o�b�t�@��
			swapChainDesc.BufferDesc.Width = screenWidth;					// �o�b�t�@�̕�
			swapChainDesc.BufferDesc.Height = screenHeight;				// �o�b�t�@�̍���
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �o�b�t�@�̃J���[�t�H�[�}�b�g�̎w��
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// ���t���b�V�����[�g�̕���
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// ���t���b�V�����[�g�̕��q
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �o�b�t�@�̎g�p���@�̎w��
			swapChainDesc.OutputWindow = hwnd;//�o�͐�E�B���h�E�o���h��
			// �o�b�N�o�b�t�@�̃}���`�T���v�����O�Ɋւ���ݒ�
			// �}���`�T���v�����O���g�p���Ȃ��ꍇ�� Count �ɂP�AQuality �ɂO���w��B
			swapChainDesc.SampleDesc.Count = 1;							// 1�s�N�Z���̐F�����߂�T���v�����O��(���g�p�͂P)
			swapChainDesc.SampleDesc.Quality = 0;							// �T���v�����O�̕i��(���x)�i���g�p�͂O�j
			swapChainDesc.Windowed = !FALSE;								// �E�B���h�E���[�h�@or�@�t���X�N���[�����[�h
		}

		D3D_FEATURE_LEVEL featureLevel;

		// �f�o�C�X�����ƃX���b�v�`�F�[���쐬���s��
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,									// �r�f�I�A�_�v�^�̎w��(null�͋K��)
			D3D_DRIVER_TYPE_HARDWARE,				// �h���C�o�^�C�v
			NULL,									// D3D_DRIVER_TYPE_SOFTWARE�w�莞�Ɏg�p
			createDeviceFlags,					// �t���O�w��
			featureLevels,							// D3D_FEATURE_LEVEL�w��Ŏ����Œ�`�����z��
			1,										// ���D3D_FEATURE_LEVEL�z��̗v�f��
			D3D11_SDK_VERSION,						// SDK�o�[�W����
			&swapChainDesc,						// DXGI_SWAP_CHAIN_DESC
			swapChain.GetAddressOf(),				// �֐���������SwapChain�̏o�͐�
			device.GetAddressOf(),					// �֐���������Device�̏o�͐�
			&featureLevel,							// ��������D3D_FEATURE_LEVEL�̏o�͐�
			dc.GetAddressOf());		// �֐���������Context�̏o�͐�
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapChain->GetBuffer(
			0,											// �o�b�t�@�̃C���f�b�N�X(��{�͂O)
			__uuidof(ID3D11Texture2D),					// �擾����o�b�t�@�̃C���^�[�t�F�[�XID
			reinterpret_cast<LPVOID*>(backBuffer.GetAddressOf()));	// �o�b�t�@�̎擾��Avoid�|�C���^�ŃA�h���X�Ŏ擾����
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		// �o�b�N�o�b�t�@��ݒ肵�ă����_�[�^�[�Q�b�g�r���[���쐬
		hr = device->CreateRenderTargetView(
			backBuffer.Get(),						// �쐬����o�b�t�@�̃��\�[�X
			NULL,									// �쐬����View�̐ݒ���e�f�[�^�̎w��(nullptr�Ńf�t�H���g�ݒ�)
			&renderTargetView);					// �쐬���ꂽRenderTargetView���i�[���邽�߂̃|�C���^�̃A�h���X
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �[�x�X�e���V���r���[�̍쐬
	{
		// ID3D11Texture2D �̐ݒ���s�����߂̍\���́BID3D11Texture2D �ɐ[�x�X�e���V���p�̐ݒ���s��
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = screenWidth;						// �e�N�X�`���̕�
		texture2dDesc.Height = screenHeight;					// �e�N�X�`���̍���
		texture2dDesc.MipLevels = 1;							// �~�b�v�}�b�v�̃��x���w��
		texture2dDesc.ArraySize = 1;							// �e�N�X�`���z��̃T�C�Y�w��
		texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// �e�N�X�`���̃t�H�[�}�b�g(DXGI_FORMAT_D24_UNORM_S8_UINT���g����Depth24bit,Stencil8bit�ƂȂ�)
		// �}���`�T���v�����O���g�p���Ȃ��ꍇ�� Count �ɂP�AQuality �ɂO���w��B
		texture2dDesc.SampleDesc.Count = 1;					// 1�s�N�Z���̐F�����߂�T���v�����O��(���g�p�͂P)
		texture2dDesc.SampleDesc.Quality = 0;					// �T���v�����O�̕i��(���x)�i���g�p�͂O�j
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;				// �e�N�X�`���̎g�p���@
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// Bind�ݒ�,D3D11_BIND_DEPTH_STENCIL���w��
		texture2dDesc.CPUAccessFlags = 0;						// CPU����̓ǂݏ�����s��
		texture2dDesc.MiscFlags = 0;							// ���\�[�X�I�v�V�����̃t���O
		hr = device->CreateTexture2D(
			&texture2dDesc,							// �쐬����e�N�X�`���̏ڍ׏��
			NULL,										// �e�N�X�`���̏��ɕt�^����f�[�^���w�肷��A�ǂݍ��񂾉摜�f�[�^�����Ă������肷��
			depthStencilBuffer.GetAddressOf());		// �쐬���ꂽTexture���i�[���邽�߂̃|�C���^�̃A�h���X
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		// ID3D11DepthStencilView �̐ݒ���s�����߂̍\���́B�[�x�X�e���V���r���[���쐬���邽�߂ɕK�v�Ȑݒ���s��
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		// ���\�[�X�e�[�^�̌`��
		depthStencilViewDesc.Format = texture2dDesc.Format;					// View�̃t�H�[�}�b�g
		// �[�x�X�e���V���ւ̃A�N�Z�X���@
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// �e�N�X�`���̎��(�����Őݒ肵���l�ɂ����D3D11_TEX2D_DSV�����߂�)
		depthStencilViewDesc.Texture2D.MipSlice = 0;							// �ŏ��Ɏg�p����~�b�v�}�b�v�̃��x�����w�� 				
		// ���\�[�X�f�[�^�ɃA�N�Z�X���邽�߂̐[�x�X�e���V���r���[���쐬			
		hr = device->CreateDepthStencilView(
			depthStencilBuffer.Get(),					// View�쐬�ɕK�v�ȃ��\�[�X
			&depthStencilViewDesc,					// DSV�Ɋւ���ݒ���s�����f�[�^
			depthStencilView.GetAddressOf());			// ID3D11DepthStencilView�̏o�͐�
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �r���[�|�[�g�̐ݒ�
	{
		viewport.TopLeftX = 0;									// �r���[�|�[�g������X�̈ʒu
		viewport.TopLeftY = 0;									// �r���[�|�[�g�㕔��Y�ʒu
		viewport.Width = static_cast<float>(screenWidth);		// �r���[�|�[�g�̕�
		viewport.Height = static_cast<float>(screenHeight);		// �r���[�|�[�g�̍���
		viewport.MinDepth = 0.0f;								// �r���[�|�[�g�̍ŏ��[�x�B0~1�͈̔�
		viewport.MaxDepth = 1.0f;								// �r���[�|�[�g�̍ő�[�x�B0~1�͈̔�
		dc->RSSetViewports(1, &viewport);
	}

	// �����_�[�X�e�[�g����
	renderState = std::make_unique<RenderState>(device.Get());

	{
		light = std::make_unique<Light>(device.Get());
	}

	{
		
	}
	// �����_��
	{
		debugPrimitive = std::make_unique<DebugPrimitive>(device.Get());
		lineRenderer= std::make_unique<LineRenderer>(device.Get(), 1024);
	}
}

// �N���A
void Graphics::Clear(float r, float g, float b, float a)
{
	float color[4]{ r, g, b, a };
	dc->ClearRenderTargetView(renderTargetView.Get(), color);
	dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

// �����_�[�^�[�Q�b�g�ݒ�
void Graphics::SetRenderTargets()
{
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

// ��ʕ\��
void Graphics::Present(UINT syncInterval)
{
	swapChain->Present(syncInterval, 0);
}
