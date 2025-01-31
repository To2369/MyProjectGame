#pragma once
#include "RenderState.h"
#include "..\Camera.h"
struct LightData
{
	DirectX::XMFLOAT4 direction;	// ����
	DirectX::XMFLOAT4 color;	// �F
};

// �}�X�N�f�[�^
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float						dissolveThreshold;
	float						edgeThreshold;		// ����臒l
	DirectX::XMFLOAT4			edgeColor;			// ���̐F
};

// �F���␳���
struct ColorGradingData
{
	float	hueShift = 0;	// �F������
	float	saturation = 1;	// �ʓx����
	float	brightness = 1;	// ���x����
	float	dummy;
};

struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState;
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 viewProjection;
	//�F���␳���
	ColorGradingData colorGradingData;
	LightData lightData;
	MaskData maskData;
	//GlitchNoiseData glitchnoiseData;
};

struct GlitchNoiseData
{
	float intensity = 0.25f;//���x
	float time = 0;

	//�u���b�N��m�C�Y
	float baseIntensity = 0.2f;
	float baseFps = 7;
	DirectX::XMFLOAT2 baseAmount = { 4.0f, 32.0f };
	//�F�����炷
	float caIntensity = 0.5f;//�F�V�t�g�̋��x
	float caFps = 2;	//�F�V�t�g��FPS
	DirectX::XMFLOAT3 caColorFactor = { 0.5f,0.5f,0.5f };//�F���̂��炵��
};