#include "SpriteBatch.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites) :max_vertices(max_sprites * 6)
{
    HRESULT hr{ S_OK };

    // ���_���̃Z�b�g
    std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };
    // ���_�o�b�t�@�I�u�W�F�N�g�̐���
    D3D11_BUFFER_DESC buffer_desc{};                                                // ���_�o�b�t�@�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
    buffer_desc.ByteWidth = sizeof(vertex) * max_vertices;                          // ���_���̃T�C�Y���w��
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                                        // �o�b�t�@�̓ǂݎ��Ə������݂̕��@�����BGPU ���炵���A�N�Z�X�ł��Ȃ��悤�ݒ�
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               // �o�b�t�@���p�C�v���C���Ƀo�C���h�������@�����,���̃o�b�t�@�I�u�W�F�N�g�𒸓_�o�b�t�@�Ƃ��Đݒ�
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            // CPU �փA�N�Z�X����ۂ̃t���O
    buffer_desc.MiscFlags = 0;                                                      // ���̑��̃t���O�B�s�v�ȏꍇ�͂O
    buffer_desc.StructureByteStride = 0;                                            // �o�b�t�@���\�����o�b�t�@��\���ꍇ�̃o�b�t�@�\�����̊e�v�f�̃T�C�Y�B

    // �T�u���\�[�X�f�[�^
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // �������f�[�^�ւ̃|�C���^
    subresource_data.pSysMem = vertices.get();                                                  // �e�N�X�`���̂P�s�̐擪���玟�̍s�܂ł̋���(�o�C�g�P��)�B
    subresource_data.SysMemPitch = 0;                                                           // �e�N�X�`���̂P�s�̐擪���玟�̍s�܂ł̋���(�o�C�g�P��)�B
    subresource_data.SysMemSlicePitch = 0;                                                      // ����[�x���x���̊J�n���玟�̐[�x���x���܂ł̋���(�o�C�g�P��)
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());   // �f�o�C�X���g���āA���_�o�b�t�@�̃T�u���\�[�X�Ƃ��Ē��_����ݒ肵�Ē��_�o�b�t�@�𐶐�
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    // ���C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]       // ���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\SpriteVS.cso",
            vertex_shader.GetAddressOf(), input_layout.GetAddressOf(),
            input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\SpritePS.cso",
            pixel_shader.GetAddressOf());
    }
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\EffectPS.cso",
            replaced_pixel_shader.GetAddressOf());
    }
    // �e�N�X�`���̓ǂݍ���
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, filename,
            shader_resource_view.GetAddressOf(),
            &texture2d_desc);
    }
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, filename,
            replaced_shader_resource_view.GetAddressOf(),
            &replaced_texture2d_desc);
    }
}

SpriteBatch::~SpriteBatch()
{

}

// �V�F�[�_�[�ƃe�N�X�`���̐ݒ�
void SpriteBatch::Begin(ID3D11DeviceContext* immediate_context,
    ID3D11PixelShader* replaced_pixel_shader,
    ID3D11ShaderResourceView* replaced_shader_resource_view)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : 
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    if (replaced_shader_resource_view)
    {
        // texture2d_desc�ɂ����ς�����e�N�X�`������ݒ�
        HRESULT hr{ S_OK };
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        replaced_shader_resource_view->GetResource(resource.GetAddressOf());
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
        hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
        texture2d->GetDesc(&texture2d_desc);

        // �����ς�����e�N�X�`����ݒ�
        immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
    }
    else
    {
        // ���܂ł̃e�N�X�`����ݒ�
        immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    }
}

void SpriteBatch::End(ID3D11DeviceContext* immediate_context)
{
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(),
        0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    size_t vertex_count = vertices.size();
    _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = vertices.data();
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }
    immediate_context->Unmap(vertex_buffer.Get(), 0);

    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->Draw(static_cast<UINT>(vertex_count), 0);
}


void SpriteBatch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh
)
{
    // ��������0.0���炪�摜�ő�ɂ��ăI�[�o�[���[�h�����ق���Render���Ăяo��
    Render(immediate_context,
        dx, dy,
        dw, dh,
        1, 1, 1, 1,
        0.0f,
        0.0f, 0.0f,
        static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteBatch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle)
{
    // ��������0.0���炪�摜�ő�ɂ��ăI�[�o�[���[�h�����ق���Render���Ăяo��
    Render(immediate_context,
        dx, dy,
        dw, dh,
        r, g, b, a,
        angle,
        0.0f, 0.0f,
        static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteBatch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh
)
{
    // �X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //  (x0, y0) *----* (x1, y1)             
    //           |   /|           
    //           |  / |
    //           | /  |
    //           |/   |
    //  (x2, y2) *----* (x3, y3)

    // left-top
    float x0{ dx };
    float y0{ dy };
    // right-top
    float x1{ dx + dw };
    float y1{ dy };
    // left-bottom
    float x2{ dx };
    float y2{ dy + dh };
    //right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };

    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

    // ��]�̒��S��Z�a�̒��S�_�ɂ����ꍇ
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, sin, cos);
    rotate(x1, y1, cx, cy, sin, cos);
    rotate(x2, y2, cx, cy, sin, cos);
    rotate(x3, y3, cx, cy, sin, cos);

    // �X�N���[�����W�n���� NDC ���W�n�ɕϊ�
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // �e�N�X�`�����W�̐ݒ�
    float u0{ sx / texture2d_desc.Width };
    float v0{ sy / texture2d_desc.Height };
    float u1{ (sx + sw) / texture2d_desc.Width };
    float v1{ (sy + sh) / texture2d_desc.Height };

    // ���_�f�[�^�̐ݒ�
    vertices.push_back({ {x0,y0,0},{r,g,b,a},{u0,v0} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x2,y2,0},{r,g,b,a},{u0,v1} });
    vertices.push_back({ {x1,y1,0},{r,g,b,a},{u1,v0} });
    vertices.push_back({ {x3,y3,0},{r,g,b,a},{u1,v1} });
}