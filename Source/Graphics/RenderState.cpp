#include"RenderState.h"
#include"..\misc.h"
RenderState::RenderState(ID3D11Device* device)
{
	HRESULT hr{ S_OK };

	//�T���v���[�X�e�[�g�𐶐����邽�߂̍\����
	{
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
		{
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;	//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			//�|�C���g�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::POINT)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�|�C���g�T���v���[�X�e�[�g(���炩)
		{
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			//���j�A�t�B���^�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::LINEAR)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�A�j�\�g���s�b�N�T���v���[�X�e�[�g(���炩3D)
		{
			sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;			//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
			//�A�j�\�g���s�b�N�Ƃ��ăT���v���[�X�e�[�g�𐶐�
			hr = device->CreateSamplerState(&sampler_desc,
				sampler_states[static_cast<int>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	{
		//�[�x�e�X�g�� ON �[�x�������݂� ON
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		{
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::ON_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�[�x�e�X�g�� ON �[�x�������݂� OFF
		{
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::ON_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�[�x�e�X�g�� OFF �[�x�������݂� ON
		{
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::OFF_ON)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�[�x�e�X�g�� OFF �[�x�������݂� OFF
		{
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			hr = device->CreateDepthStencilState(&depth_stencil_desc,
				depth_stencil_states[static_cast<int>(DEPTH_STENCIL_STATE::OFF_OFF)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	//�u�����h�X�e�[�g
	{
		D3D11_BLEND_DESC blend_desc{};

		//��������
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = FALSE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::NONE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//�ʏ퍇��
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::ALPHABLENDING)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//���Z����
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::ADD)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//���Z����
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::SUB)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}

		//��Z����
		{
			blend_desc.AlphaToCoverageEnable = FALSE;
			blend_desc.IndependentBlendEnable = FALSE;
			blend_desc.RenderTarget[0].BlendEnable = TRUE;
			blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
			blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			hr = device->CreateBlendState(&blend_desc,
				blend_states[static_cast<int>(BLEND_STATE::MULTIPLE)].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		}
	}

	//���X�^���C�U�X�e�[�g���쐬���邽�߂̍\����
	D3D11_RASTERIZER_DESC rasterizer_desc{};

	//���X�^���C�U�X�e�[�g�\���̋��ʍ�
	rasterizer_desc.FrontCounterClockwise = FALSE;	//false...���v����\�ʂƂ���A�����v���𗠖�
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;

	//�u�\���b�h�v�Łu���ʔ�\���v
	{
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::SOLID_CULLBACK)].GetAddressOf());
	}

	//�u���C���[�t���[���v�Łu���ʔ�\���v
	{
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::WIRE_CULLBACK)].GetAddressOf());
	}

	//�u�\���b�h�v�Łu���ʕ\���v
	{
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::SOLID_CULLNONE)].GetAddressOf());
	}

	//�u���C���[�t���[���v�Łu���ʕ\���v
	{
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.AntialiasedLineEnable = TRUE;
		hr = device->CreateRasterizerState(&rasterizer_desc,
			rasterizer_states[static_cast<int>(RASTERIZER_STATE::WIRE_CULLNONE)].GetAddressOf());
	}
}