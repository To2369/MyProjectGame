#include"Misc.h"
#include"DeviceManager.h"

//����������
DeviceManager* DeviceManager::Initialize(HWND hwnd)
{
	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT screenWidth = rc.right - rc.left;
	UINT screenHeight = rc.bottom - rc.top;

	this->screenWidth = static_cast<float>(screenWidth);
	this->screenHeight = static_cast<float>(screenHeight);

	//�f�o�C�X�E�f�o�C�X�R���e�L�X�g�E�X���b�v�`�F�[���̍쐬
	HRESULT hr{ S_OK };

	UINT create_device_flags{ 0 };
#ifdef _DEBUG
	create_device_flags != D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
	//Device,Context�̔\�̓��x���̐ݒ�
	// DirectX11�̊�����S�ɖ�������GPU��œ��삷�郌�x���ɐݒ�
	D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };

	// IDXGISwapChain �̐ݒ���s�����߂̍\���́BSwapChain �쐬���邽�߂ɕK�v�Ȑݒ���s��
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.BufferCount = 1;								//�X���b�v�`�F�C���̃o�b�t�@��
	swap_chain_desc.BufferDesc.Width = screenWidth;					//�o�b�t�@�̕�
	swap_chain_desc.BufferDesc.Height = screenHeight;				//�o�b�t�@�̍���
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//�o�b�t�@�̃J���[�t�H�[�}�b�g�̎w��
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;			//���t���b�V�����[�g�̕���
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;			//���t���b�V�����[�g�̕��q
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//�o�b�t�@�̎g�p���@�̎w��
	swap_chain_desc.OutputWindow = hwnd;//�o�͐�E�B���h�E�o���h��
	// �o�b�N�o�b�t�@�̃}���`�T���v�����O�Ɋւ���ݒ�
	// �}���`�T���v�����O���g�p���Ȃ��ꍇ�� Count �ɂP�AQuality �ɂO���w��B
	swap_chain_desc.SampleDesc.Count = 1;							//1�s�N�Z���̐F�����߂�T���v�����O��(���g�p�͂P)
	swap_chain_desc.SampleDesc.Quality = 0;							//�T���v�����O�̕i��(���x)�i���g�p�͂O�j
	swap_chain_desc.Windowed = !FALSE;								//�E�B���h�E���[�h�@or�@�t���X�N���[�����[�h

	//�f�o�C�X�����ƃX���b�v�`�F�[���쐬���s��
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,						//�r�f�I�A�_�v�^�̎w��(null�͋K��)
		D3D_DRIVER_TYPE_HARDWARE,	//�h���C�o�^�C�v
		NULL,						//D3D_DRIVER_TYPE_SOFTWARE�w�莞�Ɏg�p
		create_device_flags,		//�t���O�w��
		&feature_levels,			//D3D_FEATURE_LEVEL�w��Ŏ����Œ�`�����z��
		1,							//���D3D_FEATURE_LEVEL�z��̗v�f��
		D3D11_SDK_VERSION,			//SDK�o�[�W����
		&swap_chain_desc,			//DXGI_SWAP_CHAIN_DESC
		&swap_chain,				//�֐���������SwapChain�̏o�͐�
		&device,					//�֐���������Device�̏o�͐�
		NULL,						//��������D3D_FEATURE_LEVEL�̏o�͐�
		&immediate_context);		//�֐���������Context�̏o�͐�
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�����_�[�^�[�Q�b�g�r���[�̍쐬
	ID3D11Texture2D* back_buffer{};
	hr = swap_chain->GetBuffer(
		0,											//�o�b�t�@�̃C���f�b�N�X(��{�͂O)
		__uuidof(ID3D11Texture2D),					//�擾����o�b�t�@�̃C���^�[�t�F�[�XID
		reinterpret_cast<LPVOID*>(&back_buffer));	//�o�b�t�@�̎擾��Avoid�|�C���^�ŃA�h���X�Ŏ擾����
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�o�b�N�o�b�t�@��ݒ肵�ă����_�[�^�[�Q�b�g�r���[���쐬
	hr = device->CreateRenderTargetView(
		back_buffer,							//�쐬����o�b�t�@�̃��\�[�X
		NULL,									//�쐬����View�̐ݒ���e�f�[�^�̎w��(nullptr�Ńf�t�H���g�ݒ�)
		&render_target_view);					//�쐬���ꂽRenderTargetView���i�[���邽�߂̃|�C���^�̃A�h���X
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	back_buffer->Release();

	//�[�x�X�e���V���r���[�̍쐬
	// ID3D11Texture2D �̐ݒ���s�����߂̍\���́BID3D11Texture2D �ɐ[�x�X�e���V���p�̐ݒ���s��
	ID3D11Texture2D* depth_stencil_buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = screenWidth;						//�e�N�X�`���̕�
	texture2d_desc.Height = screenHeight;					//�e�N�X�`���̍���
	texture2d_desc.MipLevels = 1;							//�~�b�v�}�b�v�̃��x���w��
	texture2d_desc.ArraySize = 1;							//�e�N�X�`���z��̃T�C�Y�w��
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//�e�N�X�`���̃t�H�[�}�b�g(DXGI_FORMAT_D24_UNORM_S8_UINT���g����Depth24bit,Stencil8bit�ƂȂ�)
	// �}���`�T���v�����O���g�p���Ȃ��ꍇ�� Count �ɂP�AQuality �ɂO���w��B
	texture2d_desc.SampleDesc.Count = 1;					//1�s�N�Z���̐F�����߂�T���v�����O��(���g�p�͂P)
	texture2d_desc.SampleDesc.Quality = 0;					//�T���v�����O�̕i��(���x)�i���g�p�͂O�j
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;				//�e�N�X�`���̎g�p���@
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	//Bind�ݒ�,D3D11_BIND_DEPTH_STENCIL���w��
	texture2d_desc.CPUAccessFlags = 0;						//CPU����̓ǂݏ�����s��
	texture2d_desc.MiscFlags = 0;							//���\�[�X�I�v�V�����̃t���O
	hr = device->CreateTexture2D(
		&texture2d_desc,			//�쐬����e�N�X�`���̏ڍ׏��
		NULL,						//�e�N�X�`���̏��ɕt�^����f�[�^���w�肷��A�ǂݍ��񂾉摜�f�[�^�����Ă������肷��
		&depth_stencil_buffer);		//�쐬���ꂽTexture���i�[���邽�߂̃|�C���^�̃A�h���X
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ID3D11DepthStencilView �̐ݒ���s�����߂̍\���́B�[�x�X�e���V���r���[���쐬���邽�߂ɕK�v�Ȑݒ���s��
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	//���\�[�X�e�[�^�̌`��
	depth_stencil_view_desc.Format = texture2d_desc.Format;					//View�̃t�H�[�}�b�g
	// �[�x�X�e���V���ւ̃A�N�Z�X���@
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	//�e�N�X�`���̎��(�����Őݒ肵���l�ɂ����D3D11_TEX2D_DSV�����߂�)
	depth_stencil_view_desc.Texture2D.MipSlice = 0;							// �ŏ��Ɏg�p����~�b�v�}�b�v�̃��x�����w�� 				
	// ���\�[�X�f�[�^�ɃA�N�Z�X���邽�߂̐[�x�X�e���V���r���[���쐬			
	hr = device->CreateDepthStencilView(
		depth_stencil_buffer,		//View�쐬�ɕK�v�ȃ��\�[�X
		&depth_stencil_view_desc,	//DSV�Ɋւ���ݒ���s�����f�[�^
		&depth_stencil_view);		//ID3D11DepthStencilView�̏o�͐�
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;									//�r���[�|�[�g������X�̈ʒu
	viewport.TopLeftY = 0;									//�r���[�|�[�g�㕔��Y�ʒu
	viewport.Width = static_cast<float>(screenWidth);		//�r���[�|�[�g�̕�
	viewport.Height = static_cast<float>(screenHeight);		//�r���[�|�[�g�̍���
	viewport.MinDepth = 0.0f;								//�r���[�|�[�g�̍ŏ��[�x�B0~1�͈̔�
	viewport.MaxDepth = 1.0f;								//�r���[�|�[�g�̍ő�[�x�B0~1�͈̔�
	immediate_context->RSSetViewports(1, &viewport);

	return this;
}