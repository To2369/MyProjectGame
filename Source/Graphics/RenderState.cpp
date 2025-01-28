#include"RenderState.h"
#include"..\misc.h"
RenderState::RenderState(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	// �T���v���[�X�e�[�g�𐶐����邽�߂̍\����
	{
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		// �O����P�͈̔͊O��au�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		// �O����P�͈̔͊O��av�e�N�X�`�����W���������邽�߂Ɏg�p������@
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		// �O����P�͈̔͊O��aw�e�N�X�`�����W���������邽�߂Ɏg�p������@
		samplerDesc.MipLODBias = 0;							// �v�Z���ꂽ�~�b�v�}�b�v���x������̃I�t�Z�b�g�B
		samplerDesc.MaxAnisotropy = 16;						// �N�����v�l
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;	// �T���v�����O���ꂽ�f�[�^�������̃T���v�����O���ꂽ�f�[�^�Ɣ�r����֐�
		// AddressU,V,W ��D3D11_TEXTURE_ADDRESS_BORDER ���w�肳��Ă���ꍇ�Ɏg�p���鋫�E���̐F
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;								// �A�N�Z�X���N�����v����~�b�v�}�b�v�͈̔͂̉���
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;				// �A�N�Z�X���N�����v����~�b�v�}�b�v�͈̔͂̏��

		// �|�C���g�T���v���[�X�e�[�g(�e��)
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	// �e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			// �|�C���g�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::POINT)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �|�C���g�T���v���[�X�e�[�g(���炩)
		{
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// �e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			// ���j�A�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::LINEAR)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �A�j�\�g���s�b�N�T���v���[�X�e�[�g(���炩3D)
		{
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;			// �e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			// �A�j�\�g���s�b�N�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&samplerDesc,
				samplerStates[static_cast<int>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	{
		// �[�x�e�X�g�� ON �[�x�������݂� ON
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
		{
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::ON_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �[�x�e�X�g�� ON �[�x�������݂� OFF
		{
			depthStencilDesc.DepthEnable = TRUE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::ON_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �[�x�e�X�g�� OFF �[�x�������݂� ON
		{
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::OFF_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �[�x�e�X�g�� OFF �[�x�������݂� OFF
		{
			depthStencilDesc.DepthEnable = FALSE;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depthStencilDesc,
				depthStencilStates[static_cast<int>(DEPTH_STENCIL_STATE::OFF_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// �u�����h�X�e�[�g
	{
		D3D11_BLEND_DESC blendDesc{};

		// ��������
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = FALSE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::NONE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// �ʏ퍇��
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::ALPHABLENDING)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// ���Z����
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::ADD)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// ���Z����
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::SUB)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		// ��Z����
		{
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blendDesc,
				blendStates[static_cast<int>(BLEND_STATE::MULTIPLE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	// ���X�^���C�U�X�e�[�g���쐬���邽�߂̍\����
	D3D11_RASTERIZER_DESC rasterizerDesc{};

	// ���X�^���C�U�X�e�[�g�\���̋��ʍ�
	rasterizerDesc.FrontCounterClockwise = FALSE;	// false...���v����\�ʂƂ���A�����v���𗠖�
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;

	// �u�\���b�h�v�Łu���ʔ�\���v
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::SOLID_CULLBACK)].GetAddressOf());
	}

	// �u���C���[�t���[���v�Łu���ʔ�\���v
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIRE_CULLBACK)].GetAddressOf());
	}

	// �u�\���b�h�v�Łu���ʕ\���v
	{
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::SOLID_CULLNONE)].GetAddressOf());
	}

	// �u���C���[�t���[���v�Łu���ʕ\���v
	{
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizerDesc,
			rasterizerStates[static_cast<int>(RASTERIZER_STATE::WIRE_CULLNONE)].GetAddressOf());
	}
}