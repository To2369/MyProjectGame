#pragma once
#include"Scene.h"
#include"Camera.h"
#include"CameraController.h"
#include"Graphics/FrameBuffer.h"
#include"Graphics/FullScreenQuad.h"
#include"Stage.h"
#include "Graphics/Sprite.h"
#include "Player.h"
#include "Graphics/Light.h"
#include "PostprocessingRenderer.h"
#include "Graphics/ColorGrading.h"
#include "Graphics/RenderContext.h"
#include "Graphics\Light.h"
//�^�C�g���V�[��
class SceneGame :public Scene
{
public:
    SceneGame() {}
    ~SceneGame()override {}

    //������
    void Initialize()override;

    //�I����
    void Finalize()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //�`��X�V
    void Render()override;

    void DrawGauge(ID3D11DeviceContext* dc, RenderContext* rc);

    void DrawUltArtsSelect(ID3D11DeviceContext* dc, RenderContext* rc);

    void DrawSkillArtsSelect(ID3D11DeviceContext* dc, RenderContext* rc);
public:
    // �V�[���萔
    struct Scene_constants
    {
        DirectX::XMFLOAT4X4		viewProjection;
        DirectX::XMFLOAT4		lightDirection;
        DirectX::XMFLOAT4		cameraPosition;
    };
    // �V�[���萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    struct point_lights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
        float range{ 0 };
        DirectX::XMFLOAT3 dummy;
    };
    struct spot_lights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };
        DirectX::XMFLOAT4 direction{ 0, 0, 1, 0 };
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };
        float range{ 0 };
        float innerCorn{ 0.99f };
        float outerCorn{ 0.9f };
        float dummy;
    };
    struct light_constants
    {
        DirectX::XMFLOAT4 ambient_color;
        DirectX::XMFLOAT4 directional_light_direction;
        DirectX::XMFLOAT4 directional_light_color;
        point_lights point_light[8];
        spot_lights spot_light[8];
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> light_constant_buffer;
DirectX::XMFLOAT4 ambient_color{ 0.2f, 0.2f, 0.2f, 0.2f };
DirectX::XMFLOAT4 directional_light_direction{ 0.0f, -1.0f, 1.0f, 1.0f };
DirectX::XMFLOAT4 directional_light_color{ 1.0f, 1.0f, 1.0f, 1.0f };
point_lights point_light[8];
spot_lights spot_light[8];


//	�K�E�X�t�B���^�[
#define	KERNEL_MAX 25
struct gaussian_filter_constants
{
    DirectX::XMFLOAT4	weights[KERNEL_MAX * KERNEL_MAX];
    float				kernelSize;
    DirectX::XMFLOAT2	texcel;
    float				dummy;
};
struct gaussian_filter_datas
{
    int					kernelSize{ 9 };
    float				sigma{ 10.0f };
    DirectX::XMFLOAT2	textureSize;
};
void calc_gaussian_filter_constant(gaussian_filter_constants& constant, const gaussian_filter_datas& data);
gaussian_filter_datas gaussian_filter_data;
gaussian_filter_constants gaussian_filter_constant;
Microsoft::WRL::ComPtr<ID3D11Buffer> gaussian_filter_constant_buffer;
Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_filter_pixel_shader;

private:
    std::unique_ptr<FrameBuffer> framebuffers[8];

    Camera* camera;
    std::unique_ptr<CameraController> cameraCtrl;
    //std::unique_ptr<ShadowMapCaster> shadowMapCaster;
    std::unique_ptr<Sprite> lifegauge;
    std::unique_ptr<Sprite> skillEnergyGauge;
    std::unique_ptr<Sprite> spritEnergyGauge;
    std::unique_ptr<Sprite> sprite;

    std::unique_ptr<Sprite> skillArtsSellect;
private:
    //���C�g�̕���
    DirectX::XMFLOAT4 light_direction{ 0.0f,-1.0f,0.0f,0.0f };

    std::unique_ptr<Stage> stage;
    std::unique_ptr<Player> player;
    // �|�X�g�v���Z�X
    std::unique_ptr<PostprocessingRenderer> postprocessingRenderer;

    // �J�����֌W
    DirectX::XMFLOAT3 cameraEye = { 0,10,-10 };
    DirectX::XMFLOAT3 cameraFocus = { 0,0,0 };
    DirectX::XMFLOAT3 cameraUp = { 0,1,0 };

    float cameraFovY = DirectX::XMConvertToRadians(45);
    float aspect = 0;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    bool mouseMoveFlag = true;
    bool play = false;

    D3D11_TEXTURE2D_DESC mask_texture2dDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
    std::unique_ptr<Sprite> dummy_sprite;
    float						dissolveThreshold;
    float						edgeThreshold;		// ����臒l
    DirectX::XMFLOAT4			edgeColor;			// ���̐F

};