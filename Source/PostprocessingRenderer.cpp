#include "PostprocessingRenderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/RenderContext.h"
PostprocessingRenderer::PostprocessingRenderer()
{
    Graphics* graphics = Graphics::Instance();
    {
        UINT width = static_cast<UINT>(graphics->GetScreenWidth()) / 2;
        UINT height = static_cast<UINT>(graphics->GetScreenHeight()) / 2;
        framebuffers[0] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280, 720);
        framebuffers[1] = std::make_unique<FrameBuffer>(graphics->GetDevice(), 1280 / 2, 720 / 2);
    
        // オフスクリーン描画用のシェーダーリソースビュー描画用のスプライトの作成
        bitBlockTransfer = std::make_unique<FullScreenQuad>(graphics->GetDevice());
    }
     ShaderManager::Instance()->CreatePsFromCso(graphics->GetDevice(), ".\\Data\\Shader\\LuminanceExtractionPS.cso", pixel_shaders[0].GetAddressOf());
    ShaderManager::Instance()->CreatePsFromCso(graphics->GetDevice(), ".\\Data\\Shader\\BlurPS.cso", pixel_shaders[1].GetAddressOf());
}

PostprocessingRenderer::~PostprocessingRenderer()
{
}

void PostprocessingRenderer::Render(ID3D11DeviceContext* deviceContext)
{
    Graphics* graphics = Graphics::Instance();
    RenderState* renderState = graphics->GetRenderState();
    framebuffers[0]->Clear(deviceContext);
    framebuffers[0]->Activate(deviceContext);

    framebuffers[0]->Deactivate(deviceContext);
#if 1
    deviceContext->OMSetDepthStencilState(renderState->GetDepthStencilStates(DEPTH_STENCIL_STATE::OFF_OFF), 0);
    deviceContext->RSSetState(renderState->GetRasterizerStates(RASTERIZER_STATE::SOLID_CULLNONE));
    bitBlockTransfer->Blit(deviceContext, framebuffers[0]->shader_resource_views[static_cast<int>(SHADER_RESOURCE_VIEW::RenderTargetView)].GetAddressOf(), 0, 1);
#endif
    RenderContext rc;
    rc.deviceContext = deviceContext;
    {
        //ID3D11RenderTargetView*rtv=framebuffers[0]->
    }
#if 1
  /*  bit_block_transfer->Blit(dc, framebuffers[1]->shader_resource_views[0].GetAddressOf(),
        0, 1);*/
#endif
        //ID3D11ShaderResourceView* shaderResourceviews
        //{ framebuffers[0]->shader_resource_views[0].Get() };
        //bitBlockTransfer->Blit(deviceContext, shaderResourceviews, 0, 2, pixel_shaders[1].Get());
}