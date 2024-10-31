#include "../misc.h"
#include "RenderState.h"
#include "Graphics.h"
#include "ShadowMapCaster.h"
#include "Buffer.h"
CONST LONG SHADOWMAP_WIDTH{ 1024 };
CONST LONG SHADOWMAP_HEIGHT{ 1024 };
CONST float SHADOWMAP_DRAWRECT{ 30 };

ShadowMapCaster::ShadowMapCaster(ID3D11Device* device)
{

	HRESULT hr{ S_OK };
	// ���C�g���猩���V�[���̐[�x�`��p�̃o�b�t�@����
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SHADOWMAP_WIDTH;
		texture2d_desc.Height = SHADOWMAP_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		for (int index = 0; index < shadowBufferSize; ++index)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer{};
			hr = device->CreateTexture2D(&texture2d_desc, NULL, depthBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

			//	�[�x�X�e���V���r���[����
			D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
			depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
			depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depth_stencil_view_desc.Texture2D.MipSlice = 0;
			hr = device->CreateDepthStencilView(depthBuffer.Get(),
				&depth_stencil_view_desc,
				shadowMapDepthStencilView[index].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

			//	�V�F�[�_�[���\�[�X�r���[����
			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
			shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
			shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
			shader_resource_view_desc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(depthBuffer.Get(),
				&shader_resource_view_desc,
				shadowMapShaderResourceView[index].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

		}

	}

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// �V���h�E�}�b�v�����p�V�F�[�_�[
	ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\ShadowMapCasterVS.cso",
		shadowMapCasterVertexShader.GetAddressOf(),
		shadowMapCasterInputLayout.GetAddressOf(),
		input_element_desc, ARRAYSIZE(input_element_desc));


}

void ShadowMapCaster::Render(ID3D11DeviceContext* immediateContext)
{
	HRESULT hr{ S_OK };
	RenderState* renderState = Graphics::Instance()->GetRenderState();
	shadowMapCasterConstants shadowMapConstants{};
	// �V���h�E�}�b�v��������
	{
		// �V���h�E�}�b�v�p�̐[�x�o�b�t�@�ɐݒ�
		immediateContext->ClearDepthStencilView(shadowMapDepthStencilView->Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		immediateContext->OMSetRenderTargets(0, nullptr, shadowMapDepthStencilView->Get());
		// �r���[�|�[�g�̐ݒ�
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(SHADOWMAP_WIDTH);
		viewport.Height = static_cast<float>(SHADOWMAP_HEIGHT);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediateContext->RSSetViewports(1, &viewport);
		// �u�����h�X�e�[�g�̐ݒ�
		immediateContext->OMSetBlendState(renderState->GetBlendStates(BLEND_STATE::ALPHABLENDING), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�
		immediateContext->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::ON_ON), 0);
		// ���X�^���C�U�[�X�e�[�g�̐ݒ�
		immediateContext->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLBACK));
		// �V�F�[�_�[�ݒ�
		immediateContext->IASetInputLayout(shadowMapCasterInputLayout.Get());
		immediateContext->VSSetShader(shadowMapCasterVertexShader.Get(), nullptr, 0);
		immediateContext->PSSetShader(nullptr, nullptr, 0);

		//BindBuffer(immediateContext, 1, &buffer, &shadowMapConstants);
		DirectX::XMMATRIX S, R, T;
		DirectX::XMFLOAT4X4 world;
		{
			// ���C�g�̈ʒu���猩�������s��𐶐�
			DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat4(&directional_light_direction);
			LightPosition = DirectX::XMVectorScale(LightPosition, -50);
			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
			DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(SHADOWMAP_DRAWRECT, SHADOWMAP_DRAWRECT,
				0.1f, 200.0f);
			// �萔�o�b�t�@�̍X�V
			{
				// 0�Ԃ̓��b�V�����ōX�V���Ă���
				shadowMapCasterConstants scene{};
				scene.options.x = cursor_position.x;
				scene.options.y = cursor_position.y;
				scene.options.z = timer;
				scene.options.w = flag;
				DirectX::XMStoreFloat4x4(&scene.view_projection, V * P);
				immediateContext->UpdateSubresource(buffer.Get(), 0, 0, &scene, 0, 0);
				immediateContext->VSSetConstantBuffers(1, 1, buffer.GetAddressOf());
				immediateContext->PSSetConstantBuffers(1, 1, buffer.GetAddressOf());
			}
		}
	}
}