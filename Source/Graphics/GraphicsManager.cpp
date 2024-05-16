#include"GraphicsManager.h"
#include"..\misc.h"
bool GraphicsManager::Initialize(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	//�T���v���[�X�e�[�g�𐶐����邽�߂̍\����
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//�O����P�͈̔͊O��au�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//�O����P�͈̔͊O��av�e�N�X�`�����W���������邽�߂Ɏg�p������@
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//�O����P�͈̔͊O��aw�e�N�X�`�����W���������邽�߂Ɏg�p������@
	sampler_desc.MipLODBias = 0;							//�v�Z���ꂽ�~�b�v�}�b�v���x������̃I�t�Z�b�g�B
	sampler_desc.MaxAnisotropy = 16;						//�N�����v�l
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	//�T���v�����O���ꂽ�f�[�^�������̃T���v�����O���ꂽ�f�[�^�Ɣ�r����֐�
	// AddressU,V,W ��D3D11_TEXTURE_ADDRESS_BORDER ���w�肳��Ă���ꍇ�Ɏg�p���鋫�E���̐F
	sampler_desc.BorderColor[0] = 0;
	sampler_desc.BorderColor[1] = 0;
	sampler_desc.BorderColor[2] = 0;
	sampler_desc.BorderColor[3] = 0;
	sampler_desc.MinLOD = 0;								//�A�N�Z�X���N�����v����~�b�v�}�b�v�͈̔͂̉���
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;				//�A�N�Z�X���N�����v����~�b�v�}�b�v�͈̔͂̏��

	//�|�C���g�T���v���[�X�e�[�g(�e��)
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
	//�|�C���g�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�|�C���g�T���v���[�X�e�[�g(���炩)
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
	//���j�A�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�A�j�\�g���s�b�N�T���v���[�X�e�[�g(���炩3D)
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;			//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
	//�A�j�\�g���s�b�N�Ƃ��ăT���v���[�X�e�[�g�𐶐�
	hr = device->CreateSamplerState(&sampler_desc, renderContext.sampler_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�[�x�e�X�g�� ON �[�x�������݂� ON
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, renderContext.depth_stencil_states[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�[�x�e�X�g�� ON �[�x�������݂� OFF
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, renderContext.depth_stencil_states[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�[�x�e�X�g�� OFF �[�x�������݂� ON
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, renderContext.depth_stencil_states[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�[�x�e�X�g�� OFF �[�x�������݂� OFF
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, renderContext.depth_stencil_states[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

void GraphicsManager::SettingRenderContext(std::function<void(RenderContext&)> SettingCallback)
{
	SettingCallback(renderContext);
}