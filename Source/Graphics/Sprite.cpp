#include "Sprite.h"
#include"../misc.h"
#include <sstream>
#include<WICTextureLoader.h>
Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr{ S_OK };
    //���_���̃Z�b�g
    vertex vertices[]
    {
        { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {0,0}},
        { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, {1,0}},
        { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {0,1}},
        { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, {1,1}},
    };

    //���_�o�b�t�@�I�u�W�F�N�g�̐���
    D3D11_BUFFER_DESC buffer_desc{};                                                //���_�o�b�t�@�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
    buffer_desc.ByteWidth = sizeof(vertices);                                       //���_���̃T�C�Y���w��
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                                        //�o�b�t�@�̓ǂݎ��Ə������݂̕��@�����BGPU ���炵���A�N�Z�X�ł��Ȃ��悤�ݒ�
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;                               //�o�b�t�@���p�C�v���C���Ƀo�C���h�������@�����,���̃o�b�t�@�I�u�W�F�N�g�𒸓_�o�b�t�@�Ƃ��Đݒ�
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                            //CPU �փA�N�Z�X����ۂ̃t���O
    buffer_desc.MiscFlags = 0;                                                      //���̑��̃t���O�B�s�v�ȏꍇ�͂O
    buffer_desc.StructureByteStride = 0;                                            //�o�b�t�@���\�����o�b�t�@��\���ꍇ�̃o�b�t�@�\�����̊e�v�f�̃T�C�Y�B

    //�T�u���\�[�X�f�[�^
    D3D11_SUBRESOURCE_DATA subresource_data{};
    // �������f�[�^�ւ̃|�C���^
    subresource_data.pSysMem = vertices;                                        //�e�N�X�`���̂P�s�̐擪���玟�̍s�܂ł̋���(�o�C�g�P��)�B
    subresource_data.SysMemPitch = 0;                                           //�e�N�X�`���̂P�s�̐擪���玟�̍s�܂ł̋���(�o�C�g�P��)�B
    subresource_data.SysMemSlicePitch = 0;                                      //����[�x���x���̊J�n���玟�̐[�x���x���܂ł̋���(�o�C�g�P��)
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf()); //�f�o�C�X���g���āA���_�o�b�t�@�̃T�u���\�[�X�Ƃ��Ē��_����ݒ肵�Ē��_�o�b�t�@�𐶐�
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    //���C�A�E�g�I�u�W�F�N�g�̐���
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]       //���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ���s�����߂̍\����
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    //���_�V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\Sprite_vs.cso",
            vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\Sprite_ps.cso",
            pixel_shader.GetAddressOf());
    }

    //�e�N�X�`���̓ǂݍ���
    {
        ShaderManager::Instance()->LoadTextureFromFile(device, filename, 
            shader_resource_view.GetAddressOf(), &texture2d_desc);
    }
}
Sprite::~Sprite()
{

}

void Sprite::Render(ID3D11DeviceContext* immediate_context, 
    float dx, float dy,
    float dw, float dh,
    float r, float g,float b, float a,
    float angle)
{
    //��������0.0���炪�摜�ő�ɂ��ăI�[�o�[���[�h�����ق���Render���Ăяo��
    Render(immediate_context,
        dx, dy,
        dw, dh,
        r, g, b, a,
        angle,
        0.0f, 0.0f,
        static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh
)
{
    //�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾
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

    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //��]�̒��S��Z�a�̒��S�_�ɂ����ꍇ
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);
    //�X�N���[�����W�n���� NDC ���W�n�ɕϊ�
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0,y0,0 };
        vertices[1].position = { x1,y1,0 };
        vertices[2].position = { x2,y2,0 };
        vertices[3].position = { x3,y3,0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    //���_�o�b�t�@�[�̃o�C���h
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(
        0,                              //���_�o�b�t�@�̊J�n�X���b�g
        1,                              //�z����̒��_�o�b�t�@�̐��B
        vertex_buffer.GetAddressOf(),   //���_�o�b�t�@
        &stride,                        //���̒��_�o�b�t�@�[����g�p�����v�f�̃T�C�Y
        &offset);                       //���_�o�b�t�@�̍ŏ��̗v�f�Ǝg�p�����ŏ��̗v�f�̊Ԃ̃o�C�g���ł�

    //�v���~�e�B�u�^�C�v����уf�[�^�̏���Ɋւ���o�C���h
    //�v���~�e�B�u�g�|���W�[�̐ݒ�
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
    //���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ�
    immediate_context->IASetInputLayout(input_layout.Get());

    //�V�F�[�_�[�̃o�C���h
    //���_�V�F�[�_�[�I�u�W�F�N�g�̐ݒ�
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    //�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐ݒ�
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    //�v���~�e�B�u�`��
    //�`�揈��
    immediate_context->Draw(4, 0);
}

//��ʂɕ������o��
void Sprite::Textout(ID3D11DeviceContext* immediate_context, std::string s,
    float x, float y, float w, float h,
    float r, float g, float b, float a)
{
    //�ꕶ�����̕��ƍ������v�Z
    float sw = static_cast<float>(texture2d_desc.Width / 16);
    float sh = static_cast<float>(texture2d_desc.Height / 16);
    //���݂̕����ʒu(���Έʒu)
    float carriage = 0;

    //������������render()���Ăяo��
    for (const char c : s)
    {
        LONG sx = c % 0x0F;
        //������\���A�A�X�L�[�R�[�h�̈ʒu�ɂ��镶���ʒu��؂蔲���ĕ\��
        Render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
        //�����ʒu�𕝕����炷
        carriage += w;
    }
}