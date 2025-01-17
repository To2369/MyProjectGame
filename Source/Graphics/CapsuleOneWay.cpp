#include "CapsuleOneWay.h"
#include"..\misc.h"
#include<vector>
CapsuleOneWay::CapsuleOneWay(ID3D11Device* device, float height, float radius, uint32_t slices, uint32_t ellipsoid_stacks, uint32_t stacks):
    currentHeight(height),
    radius(radius),
    slices(slices),
    stacks(stacks),
    device(device)
{
    RegenerateGeometry();

    HRESULT hr{ S_OK };

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ�
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̍쐬(���̓��C�A�E�g�����̒��ō쐬���Ă��܂�)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitiveVs.cso", vertex_shader.GetAddressOf(),
            input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̍쐬
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePs.cso",
            pixel_shader.GetAddressOf());
    }
    // �萔(�R���X�^���g)�o�b�t�@�̍쐬
    {
        buffer_desc.ByteWidth = sizeof(constants);
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
}
void CapsuleOneWay::RegenerateGeometry() {
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    float totalHeight = currentHeight; // ���݂̍������g�p
    // �㔼���̍쐬
    for (uint32_t stack = 0; stack <= stacks; ++stack) {
        float phi = DirectX::XM_PI / 2.0f * stack / stacks; // �ܓx�̊p�x
        float z = radius * std::sin(phi) + totalHeight; // ������ɐL�΂�
        float r = radius * std::cos(phi);

        for (uint32_t slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * DirectX::XM_PI * slice / slices; // �o�x�̊p�x
            float x = r * std::cos(theta);
            float y = r * std::sin(theta);

            vertices.push_back({ { x, y, z }, { x, y, z } });
        }
    }

    // �������̍쐬
    for (uint32_t stack = 0; stack <= stacks; ++stack) {
        float phi = DirectX::XM_PI / 2.0f * stack / stacks; // �ܓx�̊p�x
        float z = -radius * std::sin(phi); // �������ɂ͌Œ�
        float r = radius * std::cos(phi);

        for (uint32_t slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * DirectX::XM_PI * slice / slices; // �o�x�̊p�x
            float x = r * std::cos(theta);
            float y = r * std::sin(theta);

            vertices.push_back({ { x, y, z }, { x, y, z } });
        }
    }

    // �~���̍쐬
    for (uint32_t stack = 0; stack <= stacks; ++stack) {
        float t = static_cast<float>(stack) / stacks;
        float z = t * totalHeight; // ������ɂ̂݉���

        for (uint32_t slice = 0; slice <= slices; ++slice) {
            float theta = 2.0f * DirectX::XM_PI * slice / slices;
            float x = radius * std::cos(theta);
            float y = radius * std::sin(theta);

            vertices.push_back({ { x, y, z }, { x, y, 0 } });
        }
    }

    // �㔼���̃C���f�b�N�X
    uint32_t baseIndex = 0;
    for (uint32_t stack = 0; stack < stacks; ++stack) {
        for (uint32_t slice = 0; slice < slices; ++slice) {
            uint32_t first = baseIndex + stack * (slices + 1) + slice;
            uint32_t second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // �������̃C���f�b�N�X
    baseIndex = (stacks + 1) * (slices + 1);
    for (uint32_t stack = 0; stack < stacks; ++stack) {
        for (uint32_t slice = 0; slice < slices; ++slice) {
            uint32_t first = baseIndex + stack * (slices + 1) + slice;
            uint32_t second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // �~���̃C���f�b�N�X
    baseIndex = 2 * (stacks + 1) * (slices + 1);
    for (uint32_t stack = 0; stack < stacks; ++stack) {
        for (uint32_t slice = 0; slice < slices; ++slice) {
            uint32_t first = baseIndex + stack * (slices + 1) + slice;
            uint32_t second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

void CapsuleOneWay::Update(float elapsedTime) {
    // �����𑝉�
    currentHeight += growthRate * elapsedTime;

    // �K�v�ɉ����č��������Z�b�g
    if (currentHeight >= 30.0f) {
        currentHeight = 30.0f; // ���������ɖ߂�
    }

    // �V�������_�f�[�^�𐶐�
    RegenerateGeometry();
}

void  CapsuleOneWay::Render(ID3D11DeviceContext* immediate_context,
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& material_color)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
    // �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediate_context->IASetInputLayout(input_layout.Get());

    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    // �萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
    constants data{ world,material_color };
    immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
    // �萔(�R���X�^���g)�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

    // �C���f�b�N�X���w�肵�ĕ`��
    D3D11_BUFFER_DESC buffer_desc{};
    index_buffer->GetDesc(&buffer_desc);
    immediate_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}
void CapsuleOneWay::CreateComBuffers(ID3D11Device* device,
    vertex* vertices, size_t vertex_count,
    uint32_t* indices, size_t index_count)
{
    HRESULT hr{ S_OK };

    // ���_���W���ݒ肳��Ă���z���ݒ�
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;

    // ���_���W���ݒ肳��Ă���z���ݒ�
    subresource_data.pSysMem = vertices;
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        vertex_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}
