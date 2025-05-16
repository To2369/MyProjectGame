#include"GeometricPrimitive.h"
#include"..\Misc.h"
#include<vector>
#define VertexPoints 24
#define VertexNumber 36
GeometricPrimitive::GeometricPrimitive(ID3D11Device* device)
{
    // �T�C�Y�� 1.0 �̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B
    // �������̂̃R���g���[���|�C���g���� 8 �A
    // 1 �̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌������Ⴄ���_�� 3 ����̂Œ��_���̑����� 8x3=24 �A
    // ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
    vertex vertices[VertexPoints]{};

    // �������̂� 6 �ʎ����A1 �̖ʂ� 2 �� 3 �p�`�|���S���ō\�������̂�
    // 3 �p�`�|���S���̑����� 6x2=12 �A
    // �������̂�`�悷�邽�߂� 12 ��� 3 �p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_���� 12x3=36 ��A
    // 3 �p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
    // ���v��肪�\�ʂɂȂ�悤�Ɋi�[���邱�ƁB
    uint32_t indices[VertexNumber]{};

    // ���
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    uint32_t face = 0;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // ����
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // �O��
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // ���
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // �E��
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // ����
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // ���_�f�[�^�ƃC���f�b�N�X�f�[�^���g���Ē��_�o�b�t�@�I�u�W�F�N�g���쐬
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ�
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̍쐬(���̓��C�A�E�g�����̒��ō쐬���Ă��܂�)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitiveVs.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̍쐬
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePs.cso",
            pixelShader.GetAddressOf());
    }
    // �萔(�R���X�^���g)�o�b�t�@�̍쐬
    {
        bufferDesc.ByteWidth = sizeof(constants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
}

// �`��
void GeometricPrimitive::Render(ID3D11DeviceContext* dc,
    const DirectX::XMFLOAT4X4& world,
    const DirectX::XMFLOAT4& materialColor)
{
    uint32_t stride{ sizeof(vertex) };
    uint32_t offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    // �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    dc->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    dc->IASetInputLayout(inputLayout.Get());

    dc->VSSetShader(vertexShader.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader.Get(), nullptr, 0);

    // �萔�o�b�t�@�Ƃ��āA���[���h�s��ƃ}�e���A���J���[��ݒ�
    constants data{ world,materialColor };
    dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
    // �萔(�R���X�^���g)�o�b�t�@�I�u�W�F�N�g�̐ݒ�
    dc->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    // �C���f�b�N�X���w�肵�ĕ`��
    D3D11_BUFFER_DESC buffer_desc{};
    indexBuffer->GetDesc(&buffer_desc);
    dc->DrawIndexed(buffer_desc.ByteWidth / sizeof(uint32_t), 0, 0);
}

// ���_�o�b�t�@�I�u�W�F�N�g�̍쐬
void GeometricPrimitive::CreateComBuffers(ID3D11Device* device, 
    vertex* vertices, size_t vertexCount,
    uint32_t* indices, size_t indexCount)
{
    HRESULT hr{ S_OK };

    // ���_���W���ݒ肳��Ă���z���ݒ�
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // ���_���W���ݒ肳��Ă���z���ݒ�
    subresourceData.pSysMem = vertices;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData,
        vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indexCount);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresourceData.pSysMem = indices;
    hr = device->CreateBuffer(&bufferDesc, &subresourceData, indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

//�����`
GeometricCube::GeometricCube(ID3D11Device* device) :GeometricPrimitive(device)
{
    // �T�C�Y�� 1.0 �̐������̃f�[�^���쐬����i�d�S�����_�ɂ���j�B
    // �������̂̃R���g���[���|�C���g���� 8 �A
    // 1 �̃R���g���[���|�C���g�̈ʒu�ɂ͖@���̌������Ⴄ���_�� 3 ����̂Œ��_���̑����� 8x3=24 �A
    // ���_���z��ivertices�j�ɂ��ׂĒ��_�̈ʒu�E�@�������i�[����B
    vertex vertices[VertexPoints]{};

    // �������̂� 6 �ʎ����A1 �̖ʂ� 2 �� 3 �p�`�|���S���ō\�������̂�
    // 3 �p�`�|���S���̑����� 6x2=12 �A
    // �������̂�`�悷�邽�߂� 12 ��� 3 �p�`�|���S���`�悪�K�v�A����ĎQ�Ƃ���钸�_���� 12x3=36 ��A
    // 3 �p�`�|���S�����Q�Ƃ��钸�_���̃C���f�b�N�X�i���_�ԍ��j��`�揇�ɔz��iindices�j�Ɋi�[����B
    // ���v��肪�\�ʂɂȂ�悤�Ɋi�[���邱�ƁB
    uint32_t indices[VertexNumber]{};

   // ���
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    uint32_t face = 0;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

   // ����
   // 0---------1
   // |         |
   // |   -Y    |
   // |         |
   // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +0.0f, -1.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +0.0f, -1.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // �O��
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, -1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, -1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // ���
    // 0---------1
    // |         |
    // |   +Z    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 1].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 2].normal = { +0.0f, +0.0f, +1.0f };
    vertices[face * 4 + 3].normal = { +0.0f, +0.0f, +1.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // �E��
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { +0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { +0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { +0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { +0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { +1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { +1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 1;
    indices[face * 6 + 2] = face * 4 + 2;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 3;
    indices[face * 6 + 5] = face * 4 + 2;

    // ����
    // 0---------1
    // |         |      
    // |   -X    |
    // |         |
    // 2---------3
    face += 1;
    vertices[face * 4 + 0].position = { -0.5f, +0.5f, -0.5f };
    vertices[face * 4 + 1].position = { -0.5f, +0.5f, +0.5f };
    vertices[face * 4 + 2].position = { -0.5f, -0.5f, -0.5f };
    vertices[face * 4 + 3].position = { -0.5f, -0.5f, +0.5f };
    vertices[face * 4 + 0].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 1].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 2].normal = { -1.0f, +0.0f, +0.0f };
    vertices[face * 4 + 3].normal = { -1.0f, +0.0f, +0.0f };
    indices[face * 6 + 0] = face * 4 + 0;
    indices[face * 6 + 1] = face * 4 + 2;
    indices[face * 6 + 2] = face * 4 + 1;
    indices[face * 6 + 3] = face * 4 + 1;
    indices[face * 6 + 4] = face * 4 + 2;
    indices[face * 6 + 5] = face * 4 + 3;

    // ���_�f�[�^�ƃC���f�b�N�X�f�[�^���g���Ē��_�o�b�t�@�I�u�W�F�N�g���쐬
    CreateComBuffers(device, vertices, VertexPoints, indices, VertexNumber);

    HRESULT hr{ S_OK };

    // ���̓��C�A�E�g�I�u�W�F�N�g�̐ݒ�
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
         {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
        D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
    };

    // ���_�V�F�[�_�[�I�u�W�F�N�g�̍쐬(���̓��C�A�E�g�����̒��ō쐬���Ă��܂�)
    {
        ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GeometricPrimitiveVS.cso", vertexShader.GetAddressOf(),
            inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
    }

    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̍쐬
    {
        ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GeometricPrimitivePS.cso",
            pixelShader.GetAddressOf());
    }

    // �萔(�R���X�^���g)�o�b�t�@�̍쐬
    {
        bufferDesc.ByteWidth = sizeof(constants);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
    }
}

// �~��
GeometricCylinder::GeometricCylinder(ID3D11Device* device, float radius, float height, uint32_t slices)
    :GeometricPrimitive(device)
{
    // ������(�~����邽�߂̎O�p�`�̐��A�U�̎O�p�`)
    // ���a
    float radius_ = radius;
    // ���_�f�[�^
    vertex* vertices = new vertex[(slices + 1) * 2 + slices * 2];
    // ���_�C���f�b�N�X
    uint32_t* indices = new uint32_t[(slices * 3) * 2 + (3 * 2 * slices)];

    // �O�p�`�̊p�x (�f�O���[�p)
    float d{ 2.0f * DirectX::XM_PI / slices };
    // ���a
    //float r{ 0.5f };

    // �^�񒆂̒��_���W�Ɩ@��
    vertices[0].position = { 0.0f, 1.0f, 0.0f };
    vertices[0].normal = { 0.0f, 1.0f, 0.0f };

    // �~�̎���̒��_���W�Ɩ@����ݒ�
    for (int i = 0; i < slices; ++i) {

        // �p�x���璸�_���W���v�Z
        vertices[1 + i].position.x = radius_ * cosf(i * d);
        vertices[1 + i].position.y = 1.0f;
        vertices[1 + i].position.z = radius_ * sinf(i * d);
        // �@����ݒ�
        vertices[1 + i].normal = { 0.0f, 1.0f, 0.0f };
    }

    for (int i = 0; i < slices - 1; ++i)
    {
        indices[(i * 3) + 0] = 0;
        indices[(i * 3) + 1] = i + 2;
        indices[(i * 3) + 2] = i + 1;
    }

    indices[slices * 3 - 3] = 0;
    indices[slices * 3 - 2] = 1;
    indices[slices * 3 - 1] = slices;

    // ���̉~
    int baseSlices = slices + 1;

    // ���̉~�̐^�񒆂̒��_���W�Ɩ@��
    vertices[baseSlices].position = { 0.0f, 0.0f, 0.0f };
    vertices[baseSlices].normal = { 0.0f, -1.0f, 0.0f };

    // �~�̎���̒��_���W�Ɩ@����ݒ�
    for (int i = 0; i < slices; ++i)
    {
        // �p�x���璸�_���W���v�Z
        vertices[slices + 1 + 1 + i].position.x = radius_ * cosf(i * d);
        vertices[slices + 1 + 1 + i].position.y = 0.0f;
        vertices[slices + 1 + 1 + i].position.z = radius_ * sinf(i * d);
        // �@����ݒ�
        vertices[slices + 1 + 1 + i].normal = { 0.0f, -1.0f, 0.0f };
    }

    // ���̉~�̃C���f�b�N�X�̐ݒ�
    int baseIndex = slices + 1;
    for (uint32_t i = 0; i < slices - 1; ++i)
    {
        indices[(slices * 3) + (i * 3) + 0] = baseIndex + 0;
        indices[(slices * 3) + (i * 3) + 1] = baseIndex + i + 1;
        indices[(slices * 3) + (i * 3) + 2] = baseIndex + i + 2;
    }

    indices[(slices * 3 * 2) - 1] = baseIndex + 1;
    indices[(slices * 3 * 2) - 2] = slices * 2 + 1;
    indices[(slices * 3 * 2) - 3] = baseIndex;


    // ����
    for (int i = 0; i < slices; ++i)
    {
        float x = radius_ * cosf(i * d);
        float z = radius_ * sinf(i * d);

        // ��̓_�̍��W�Ɩ@��
        vertices[slices + 1 + slices + 1 + i * 2 + 0].position = { x, 1.0f , z };
        vertices[slices + 1 + slices + 1 + i * 2 + 0].normal = { x, 0.0f , z };

        // ���̓_�̍��W�Ɩ@��
        vertices[slices + 1 + slices + 1 + i * 2 + 1].position = { x, 0.0f , z };
        vertices[slices + 1 + slices + 1 + i * 2 + 1].normal = { x, 0.0f , z };
    }

    for (int i = 0; i < slices - 1; ++i)
    {
        // i �Ԗڂ̎l�p�`�̂P�ڂ̎O�p�`
        indices[slices * 3 * 2 + i * 6 + 0] = slices + 1 + slices + 1 + i * 2 + 0;
        indices[slices * 3 * 2 + i * 6 + 1] = slices + 1 + slices + 1 + i * 2 + 2;
        indices[slices * 3 * 2 + i * 6 + 2] = slices + 1 + slices + 1 + i * 2 + 1;
        // i �Ԗڂ̎l�p�`�̂Q�ڂ̎O�p�`
        indices[slices * 3 * 2 + i * 6 + 3] = slices + 1 + slices + 1 + i * 2 + 1;
        indices[slices * 3 * 2 + i * 6 + 4] = slices + 1 + slices + 1 + i * 2 + 2;
        indices[slices * 3 * 2 + i * 6 + 5] = slices + 1 + slices + 1 + i * 2 + 3;
    }

    // �U�ڂ̎l�p�`�̂P�ڂ̎O�p�`
    indices[slices * 3 * 2 + (slices - 1) * 6 + 0] = slices + 1 + slices + 1 + slices - 1 + slices - 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 1] = slices + 1 + slices + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 2] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
    // �U�ڂ̎l�p�`�̂Q�ڂ̎O�p�`
    indices[slices * 3 * 2 + (slices - 1) * 6 + 3] = slices + 1 + slices + 1 + slices - 1 + slices - 1 + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 4] = slices + 1 + slices + 1;
    indices[slices * 3 * 2 + (slices - 1) * 6 + 5] = slices + 1 + slices + 1 + 1;

    //���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    CreateComBuffers(device, vertices, (slices + 1) * 2 + slices * 2, indices, (slices * 3) * 2 + (3 * 2 * slices));

    delete[] vertices;
    delete[] indices;
}

//��
GeometricSphere::GeometricSphere(
    ID3D11Device* device,
    float radius,
    uint32_t slices,
    uint32_t stacks):GeometricPrimitive(device)
{
    // ���̒��_�ƃC���f�b�N�X�̃��X�g��������
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;

    // ���̔��a
    float r{ radius };

    // �㕔�Ɖ����̒��_���쐬
    vertex topVertex{};
    topVertex.position = { 0.0f, +r, 0.0f };
    topVertex.normal = { 0.0f, +1.0f, 0.0f };

    vertex bottomVertex{};
    bottomVertex.position = { 0.0f, -r, 0.0f };
    bottomVertex.normal = { 0.0f, -1.0f, 0.0f };

    // ���_���X�g�ɏ㕔�̒��_��ǉ�
    vertices.emplace_back(topVertex);

    // �o�x�����̕������Ɋ�Â��ďc�����̃��[�v�����s
    float phiStep{ DirectX::XM_PI / stacks };
    float thetaStep{ 2.0f * DirectX::XM_PI / slices };

    for (uint32_t i = 1; i <= stacks - 1; ++i)
    {
        float phi{ i * phiStep };

        // �o�x�����̃��[�v�����s
        for (uint32_t j = 0; j <= slices; ++j)
        {
            float theta{ j * thetaStep };

            vertex v{};

            // ���̕\�ʏ�̓_�̍��W���v�Z
            v.position.x = r * sinf(phi) * cosf(theta);
            v.position.y = r * cosf(phi);
            v.position.z = r * sinf(phi) * sinf(theta);

            // �@���x�N�g�����v�Z���Đ��K��
            DirectX::XMVECTOR p{ XMLoadFloat3(&v.position) };
            DirectX::XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

            // ���_���X�g�ɒ��_��ǉ�
            vertices.emplace_back(v);
        }
    }

    // ���_���X�g�ɉ����̒��_��ǉ�
    vertices.emplace_back(bottomVertex);

    // �C���f�b�N�X�̐���
    // �O�p�`�X�g���b�v�̊e���_�C���f�b�N�X�𐶐�
    for (uint32_t i = 1; i <= slices; ++i)
    {
        indices.emplace_back(0);
        indices.emplace_back(i + 1);
        indices.emplace_back(i);
    }

    // �o�x�����̃��[�v�ŎO�p�`�X�g���b�v�𐶐�
    uint32_t baseIndex{ 1 };
    uint32_t ringVertexCount{ slices + 1 };
    for (uint32_t i = 0; i < stacks - 2; ++i)
    {
        for (uint32_t j = 0; j < slices; ++j)
        {
            indices.emplace_back(baseIndex + i * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
            indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
            indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
        }
    }

    // ��ɓ_�̃C���f�b�N�X���擾
    uint32_t southPoleIndex{ static_cast<uint32_t>(vertices.size() - 1) };

    // ��ɓ_�̒���̃����O�̃x�[�X�C���f�b�N�X���擾
    baseIndex = southPoleIndex - ringVertexCount;

    // ��ɓ_�𒆐S�Ƃ����O�p�`�X�g���b�v�𐶐�
    for (uint32_t i = 0; i < slices; ++i)
    {
        indices.emplace_back(southPoleIndex);
        indices.emplace_back(baseIndex + i);
        indices.emplace_back(baseIndex + i + 1);
    }

    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}

// �J�v�Z��
GeometricCapsule::GeometricCapsule(ID3D11Device* device,
    float mantleHeight,
    float radius,
    uint32_t slices,
    uint32_t ellipsoidStacks,
    uint32_t stacks) :GeometricPrimitive(device)
{
    std::vector<vertex> vertices;
    std::vector<uint32_t> indices;
    const int baseOffset = 0;

    slices = std::max<uint32_t>(3u, slices);
    stacks = std::max<uint32_t>(1u, stacks);
    ellipsoidStacks = std::max<uint32_t>(2u, ellipsoidStacks);

    const float invSlices = 1.0f / static_cast<float>(slices);
    const float invMantleStacks = 1.0f / static_cast<float>(stacks);
    const float invEllipsoidStacks = 1.0f / static_cast<float>(ellipsoidStacks);

    const float pi2{ 3.14159265358979f * 2.0f };
    const float pi05{ 3.14159265358979f * 0.5f };
    const float angleSteps = invSlices * pi2;
    const float halfHeight = mantleHeight * 0.5f;

    /* Generate mantle vertices */
    struct spherical {
        float radius, theta, phi;
    } point{ 1, 0, 0 };
    DirectX::XMFLOAT3 position, normal;
    DirectX::XMFLOAT2 texcoord;

    float angle = 0.0f;

    // �}���g�������̒��_�������[�v
    for (uint32_t u = 0; u <= slices; ++u)
    {
        /* X���W��Z���W�̌v�Z */
        texcoord.x = sinf(angle);
        texcoord.y = cosf(angle);

        position.x = texcoord.x * radius;
        position.z = texcoord.y * radius;

        /* �@���x�N�g���̌v�Z */
        normal.x = texcoord.x;
        normal.y = 0;
        normal.z = texcoord.y;

        float magnitude = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x = normal.x / magnitude;
        normal.y = normal.y / magnitude;
        normal.z = normal.z / magnitude;

        /* �㕔�Ɖ����̒��_��ǉ� */
        texcoord.x = static_cast<float>(slices - u) * invSlices;

        for (uint32_t v = 0; v <= stacks; ++v)
        {
            texcoord.y = static_cast<float>(v) * invMantleStacks;
#if _HAS_CXX20
            position.y = lerp(half_height, -half_height, texcoord.y);
#else
            position.y = halfHeight * (1 - texcoord.y) + -halfHeight * texcoord.y;
#endif

            vertices.push_back({ position, normal });
        }

        /* ���̃X���C�X�p�Ɋp�x�𑝉� */
        angle += angleSteps;
    }

    const float topCoverSide = { 1 };
    uint32_t topBaseOffsetEllipsoid = { 0 };

    topBaseOffsetEllipsoid = static_cast<uint32_t>(vertices.size());

    for (uint32_t v = 0; v <= ellipsoidStacks; ++v)
    {
        /* ���ʍ��W�n�̃Ƃ̌v�Z */
        texcoord.y = static_cast<float>(v) * invEllipsoidStacks;
        point.theta = texcoord.y * pi05;

        for (uint32_t u = 0; u <= slices; ++u)
        {
            /* ���ʍ��W�n�̃ӂ̌v�Z */
            texcoord.x = static_cast<float>(u) * invSlices;
            point.phi = texcoord.x * pi2 * topCoverSide + pi05;

            /* ���ʍ��W�n�𒼌����W�n�ɕϊ����āA�@�����Z�b�g */
            const float sin_theta = sinf(point.theta);
            position.x = point.radius * cosf(point.phi) * sin_theta;
            position.y = point.radius * sinf(point.phi) * sin_theta;
            position.z = point.radius * cosf(point.theta);

            std::swap(position.y, position.z);
            position.y *= topCoverSide;

            /* �@�������߂Ĕ����̂��ړ� */
            float magnitude = sqrtf(position.x * position.x + position.y * position.y + position.z * position.z);
            normal.x = position.x / magnitude;
            normal.y = position.y / magnitude;
            normal.z = position.z / magnitude;

            /* ���a�ƍ������g���č��W��ϊ� */
            position.x *= radius;
            position.y *= radius;
            position.z *= radius;
            position.y += halfHeight * topCoverSide;

            // ���_�̍��W�ɉ�]��K�p
            vertices.push_back({ position, normal });
        }
    }

    const float bottomCoverSide = { -1 };
    uint32_t bottomBaseOffsetEllipsoid = { 0 };

    bottomBaseOffsetEllipsoid = static_cast<uint32_t>(vertices.size());

    for (uint32_t v = 0; v <= ellipsoidStacks; ++v)
    {
        /* ���ʍ��W�n�̃Ƃ̌v�Z */
        texcoord.y = static_cast<float>(v) * invEllipsoidStacks;
        point.theta = texcoord.y * pi05;

        for (uint32_t u = 0; u <= slices; ++u)
        {
            /* ���ʍ��W�n�̃ӂ̌v�Z */
            texcoord.x = static_cast<float>(u) * invSlices;
            point.phi = texcoord.x * pi2 * bottomCoverSide + pi05;

            /* ���ʍ��W�n�𒼌����W�n�ɕϊ����āA�@�����Z�b�g */
            const float sin_theta = sinf(point.theta);
            position.x = point.radius * cosf(point.phi) * sin_theta;
            position.y = point.radius * sinf(point.phi) * sin_theta;
            position.z = point.radius * cosf(point.theta);

            std::swap(position.y, position.z);
            position.y *= bottomCoverSide;

            /* �@�������߂Ĕ����̂��ړ� */
            float magnitude = sqrtf(position.x * position.x + position.y * position.y + position.z * position.z);
            normal.x = position.x / magnitude;
            normal.y = position.y / magnitude;
            normal.z = position.z / magnitude;

            /* ���a�ƍ������g���č��W��ϊ� */
            position.x *= radius;
            position.y *= radius;
            position.z *= radius;
            position.y += halfHeight * bottomCoverSide;

            // ���_�̍��W�ɉ�]��K�p
            vertices.push_back({ position, normal });
        }
    }
    /* Generate indices for the mantle */
    int offset = baseOffset;
    for (uint32_t u = 0; u < slices; ++u)
    {
        for (uint32_t v = 0; v < stacks; ++v)
        {
            auto i0 = v + 1 + stacks;
            auto i1 = v;
            auto i2 = v + 1;
            auto i3 = v + 2 + stacks;

            indices.emplace_back(i0 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i3 + offset);
            indices.emplace_back(i1 + offset);
            indices.emplace_back(i2 + offset);
            indices.emplace_back(i3 + offset);
        }
        offset += (1 + stacks);
    }

    /* Generate indices for the top and bottom */
    for (uint32_t v = 0; v < ellipsoidStacks; ++v)
    {
        for (uint32_t u = 0; u < slices; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v * (slices + 1) + u;
            auto i1 = v * (slices + 1) + (u + 1);

            auto i2 = (v + 1) * (slices + 1) + (u + 1);
            auto i3 = (v + 1) * (slices + 1) + u;

            /* Add new indices */
            indices.emplace_back(i0 + topBaseOffsetEllipsoid);
            indices.emplace_back(i1 + topBaseOffsetEllipsoid);
            indices.emplace_back(i3 + topBaseOffsetEllipsoid);
            indices.emplace_back(i1 + topBaseOffsetEllipsoid);
            indices.emplace_back(i2 + topBaseOffsetEllipsoid);
            indices.emplace_back(i3 + topBaseOffsetEllipsoid);
        }
    }
    for (uint32_t v = 0; v < ellipsoidStacks; ++v)
    {
        for (uint32_t u = 0; u < slices; ++u)
        {
            /* Compute indices for current face */
            auto i0 = v * (slices + 1) + u;
            auto i1 = v * (slices + 1) + (u + 1);

            auto i2 = (v + 1) * (slices + 1) + (u + 1);
            auto i3 = (v + 1) * (slices + 1) + u;

            /* Add new indices */
            indices.emplace_back(i0 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i1 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i3 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i1 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i2 + bottomBaseOffsetEllipsoid);
            indices.emplace_back(i3 + bottomBaseOffsetEllipsoid);
        }
    }

    // ���_�o�b�t�@�̃I�u�W�F�N�g�̍쐬
    CreateComBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
}