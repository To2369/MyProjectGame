#include"FrameBuffer.h"
#include"..\misc.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr{ S_OK };
	// �I�t�X�N���[�������_�����O�p�̕`���̃e�N�X�`�����쐬
	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	{
		texture2d_desc.Width = width;
		texture2d_desc.Height = height;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �I�t�X�N���[�������_�����O�p�̕`���̃����_�[�^�[�Q�b�g�r���[�𐶐�
	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desx{};
	{
		// �`���̃e�N�X�`���Ƃ��ď�L�ō쐬�����e�N�X�`����ݒ�
		render_target_view_desx.Format = texture2d_desc.Format;
		render_target_view_desx.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desx,
			render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ��L�ō쐬���������_�[�^�[�Q�b�g�r���[�p�̃e�N�X�`���̃V�F�[�_�[���\�[�X�r���[���쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	{
		shader_resource_view_desc.Format = texture2d_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc,
			shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �I�t�X�N���[�������_�����O�p�̕`���̐[�x�X�e���V���p�e�N�X�`�����쐬
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
	{
		texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �I�t�X�N���[�������_�����O�p�̕`���̐[�x�X�e���V���r���[���쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	{
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc,
			depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// ��L�ō쐬�����[�x�X�e���V���r���[�p�̃e�N�X�`���̃V�F�[�_�[���\�[�X�r���[���쐬
	{
		shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc,
			shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::ShaderResourceView)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
	}

	// �I�t�X�N���[�������_�����O�p�̃r���[�|�[�g�̃T�C�Y��ݒ�
	{
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
	}
}

// �N���A����
void FrameBuffer::Clear(ID3D11DeviceContext* immediate_context,
	float r, float g, float b, float a, float depth)
{
	float color[4]{ r,g,b,a };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

// �I�t�X�N���[�������_�����O�̐ݒ�ɘA��
// ���܂ł̕`������L���b�V���ɑޔ������I�t�X�N���[���������ݗp�Ƀ^�[�Q�b�g��ݒ�
void FrameBuffer::Activate(ID3D11DeviceContext* immediate_context)
{
	// ���ݎw�蒆�̃r���[�|�[�g�ƃ����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���̏���ޔ�
	viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediate_context->RSGetViewports(&viewport_count, cached_viewports);
	immediate_context->OMGetRenderTargets(1, cached_render_target_view.ReleaseAndGetAddressOf(),
		cached_depth_stencil_view.ReleaseAndGetAddressOf());

	// �I�t�X�N���[���p�ɂ��̃N���X�ō쐬�����r���[�|�[�g�ƃ����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V����ݒ�
	immediate_context->RSSetViewports(1, &viewport);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(),
		depth_stencil_view.Get());
}

// �I�t�X�N���[���̐ݒ肩��A��������
void FrameBuffer::Deactivate(ID3D11DeviceContext* immediate_context)
{
	// �ޔ������Ă����r���[�|�[�g�ƃ����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���̏����Đݒ�
	immediate_context->RSSetViewports(viewport_count, cached_viewports);
	immediate_context->OMSetRenderTargets(1, cached_render_target_view.GetAddressOf(),
		cached_depth_stencil_view.Get());
}