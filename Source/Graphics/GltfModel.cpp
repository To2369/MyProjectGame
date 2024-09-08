#include "GltfModel.h"
#include "../misc.h"
#include <stack>
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tiny_gltf.h>
#include"Shader.h"
GltfModel::GltfModel(ID3D11Device* device, const std::string& filename) : filename(filename)
{
	tinygltf::Model gltf_model;
	tinygltf::TinyGLTF tiny_gltf;
	std::string error, warning;
	bool succeeded{ false };
	if (filename.find(".glb") != std::string::npos)
	{
		succeeded = tiny_gltf.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
	}
	else if (filename.find(".gltf") != std::string::npos)
	{
		succeeded = tiny_gltf.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
	}

	_ASSERT_EXPR_A(warning.empty(), warning.c_str());
	_ASSERT_EXPR_A(error.empty(), warning.c_str());
	_ASSERT_EXPR_A(succeeded, L"Failed to load glTF file");
	for (std::vector<tinygltf::Scene>::const_reference gltf_scene : gltf_model.scenes)
	{
		scene& scene_{ scenes.emplace_back() };
		scene_.name = gltf_model.scenes.at(0).name;
		scene_.nodes = gltf_model.scenes.at(0).nodes;
	}

	FetchNodes(gltf_model);
	FetchMeshes(device, gltf_model);

	// TODO: This is a force-brute programming, may cause bugs
	const std::map<std::string, buffer_view>& vertex_buffer_views{
		meshes.at(0).primitives.at(0).vertex_buffer_views
	};
	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
		{"POSITION",0,vertex_buffer_views.at("POSITION").format,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",0,vertex_buffer_views.at("NORMAL").format,1,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TANGENT",0,vertex_buffer_views.at("TANGENT").format,2,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD",0,vertex_buffer_views.at("TEXCOORD_0").format,3,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"JOINTS",0,vertex_buffer_views.at("JOINTS_0").format,4,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"WEIGHTS",0,vertex_buffer_views.at("WEIGHTS_0").format,5,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	// ���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	{
		ShaderManager::Instance()->CreateVsFromCso(device, ".\\Data\\Shader\\GltfModelVS.cso", vertex_shader.ReleaseAndGetAddressOf(),
			input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));
	}

	// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���
	{
		ShaderManager::Instance()->CreatePsFromCso(device, ".\\Data\\Shader\\GltfModelPS.cso",
			pixel_shader.ReleaseAndGetAddressOf());
	}

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(primitive_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_USAGE_DEFAULT;
	HRESULT hr;
	hr = device->CreateBuffer(&buffer_desc, nullptr, primitive_cbuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
}

void GltfModel::Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world)
{
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	std::function<void(int)> traverse{ [&](int node_index)->void {
		const node& node_{nodes.at(node_index)};
		if (node_.mesh > -1)
		{
			const mesh& mesh_{ meshes.at(node_.mesh) };
			for (std::vector<mesh::primitive>::const_reference primitive : mesh_.primitives)
			{
				ID3D11Buffer* vertex_buffers[]{
					primitive.vertex_buffer_views.at("POSITION").buffer.Get(),
					primitive.vertex_buffer_views.at("NORMAL").buffer.Get(),
					primitive.vertex_buffer_views.at("TANGENT").buffer.Get(),
					primitive.vertex_buffer_views.at("TEXCOORD_0").buffer.Get(),
					primitive.vertex_buffer_views.at("JOINTS_0").buffer.Get(),
					primitive.vertex_buffer_views.at("WEIGHTS_0").buffer.Get(),
				};
				UINT strides[]{
					static_cast<UINT>(primitive.vertex_buffer_views.at("POSITION").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("NORMAL").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TANGENT").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("TEXCOORD_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("JOINTS_0").stride_in_bytes),
					static_cast<UINT>(primitive.vertex_buffer_views.at("WEIGHTS_0").stride_in_bytes),
				};
				UINT offsets[_countof(vertex_buffers)]{ 0 };
				immediate_context->IASetVertexBuffers(0, _countof(vertex_buffers), vertex_buffers, strides, offsets);
				immediate_context->IASetIndexBuffer(primitive.index_buffer_view.buffer.Get(),
					primitive.index_buffer_view.format, 0);

				primitive_constants primitive_data{};
				primitive_data.material = primitive.material;
				primitive_data.has_tangent = primitive.vertex_buffer_views.at("TANGENT").buffer != NULL;
				primitive_data.skin = node_.skin;
				DirectX::XMStoreFloat4x4(&primitive_data.world,
					DirectX::XMLoadFloat4x4(&node_.global_transform) * DirectX::XMLoadFloat4x4(&world));
				immediate_context->UpdateSubresource(primitive_cbuffer.Get(), 0, 0, &primitive_data, 0, 0);
				immediate_context->VSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());
				immediate_context->PSSetConstantBuffers(0, 1, primitive_cbuffer.GetAddressOf());

				immediate_context->DrawIndexed(static_cast<UINT>(primitive.index_buffer_view.count()), 0, 0);
			}
		}
		for (std::vector<int>::value_type child_index : node_.children)
		{
			traverse(child_index);
		}
		} };
	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		traverse(node_index);
	}
}

void GltfModel::CumulateTransforms(std::vector<node>& nodes)
{
	std::stack<DirectX::XMFLOAT4X4> parent_global_transforms;
	std::function<void(int)> traverse{ [&](int node_index)->void
	{
			node& node_{nodes.at(node_index)};
			DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(node_.scale.x,node_.scale.y,node_.scale.z) };
			DirectX::XMMATRIX R{ DirectX::XMMatrixRotationQuaternion(
				DirectX::XMVectorSet(node_.rotation.x,node_.rotation.y,node_.rotation.z,node_.rotation.w)) };
			DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(node_.translation.x,node_.translation.y,node_.translation.z) };
			DirectX::XMStoreFloat4x4(&node_.global_transform, S * R * T * DirectX::XMLoadFloat4x4(&parent_global_transforms.top()));
			for (int child_index : node_.children)
			{
				parent_global_transforms.push(node_.global_transform);
				traverse(child_index);
				parent_global_transforms.pop();
			}
	} };
	for (std::vector<int>::value_type node_index : scenes.at(0).nodes)
	{
		parent_global_transforms.push({ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 });
		traverse(node_index);
		parent_global_transforms.pop();
	}
}

// �m�[�h���̎��o��
void GltfModel::FetchNodes(const tinygltf::Model& gltf_model)
{
	for (std::vector<tinygltf::Node>::const_reference gltf_node : gltf_model.nodes)
	{
		node& node_{ nodes.emplace_back() };
		node_.name = gltf_node.name;
		node_.skin = gltf_node.skin;
		node_.mesh = gltf_node.mesh;
		node_.children = gltf_node.children;
		if (!gltf_node.matrix.empty())
		{
			DirectX::XMFLOAT4X4 matrix;
			for (size_t row = 0; row < 4; row++)
			{
				for (size_t column = 0; column < 4; column++)
				{
					matrix(row, column) = static_cast<float>(gltf_node.matrix.at(4 * row + column));
				}
			}

			DirectX::XMVECTOR S, T, R;
			bool succeed = DirectX::XMMatrixDecompose(&S, &R, &T, DirectX::XMLoadFloat4x4(&matrix));
			_ASSERT_EXPR(succeed, L"Failed to decompose matrix.");

			DirectX::XMStoreFloat3(&node_.scale, S);
			DirectX::XMStoreFloat4(&node_.rotation, R);
			DirectX::XMStoreFloat3(&node_.translation, T);
		}
		else
		{
			if (gltf_node.scale.size() > 0)
			{
				node_.scale.x = static_cast<float>(gltf_node.scale.at(0));
				node_.scale.y = static_cast<float>(gltf_node.scale.at(1));
				node_.scale.z = static_cast<float>(gltf_node.scale.at(2));
			}
			if (gltf_node.translation.size() > 0)
			{
				node_.translation.x = static_cast<float>(gltf_node.translation.at(0));
				node_.translation.y = static_cast<float>(gltf_node.translation.at(1));
				node_.translation.z = static_cast<float>(gltf_node.translation.at(2));
			}
			if (gltf_node.rotation.size() > 0)
			{
				node_.rotation.x = static_cast<float>(gltf_node.rotation.at(0));
				node_.rotation.y = static_cast<float>(gltf_node.rotation.at(1));
				node_.rotation.z = static_cast<float>(gltf_node.rotation.at(2));
				node_.rotation.w = static_cast<float>(gltf_node.rotation.at(3));
			}
		}
	}
	CumulateTransforms(nodes);
}

// ���b�V�����̎��o��
void GltfModel::FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model)
{
	HRESULT hr;
	for (std::vector<tinygltf::Mesh>::const_reference gltf_mesh : gltf_model.meshes)
	{
		mesh& mesh_{ meshes.emplace_back() };
		mesh_.name = gltf_mesh.name;
		for (std::vector<tinygltf::Primitive>::const_reference gltf_primitive : gltf_mesh.primitives)
		{
			mesh::primitive& primitive{ mesh_.primitives.emplace_back() };
			primitive.material = gltf_primitive.material;

			// Create index buffer
			const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_primitive.indices) };
			const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

			primitive.index_buffer_view = MakeBufferView(gltf_accessor);
			
			D3D11_BUFFER_DESC buffer_desc{};
			buffer_desc.ByteWidth = static_cast<UINT>(primitive.index_buffer_view.size_in_bytes);
			buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA subresource_data{};
			subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data() +
				gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
			hr = device->CreateBuffer(&buffer_desc, &subresource_data,
				primitive.index_buffer_view.buffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

			//Create vertex buffers
			for (std::map<std::string, int>::const_reference gltf_attribute : gltf_primitive.attributes)
			{
				const tinygltf::Accessor& gltf_accessor{ gltf_model.accessors.at(gltf_attribute.second) };
				const tinygltf::BufferView& gltf_buffer_view{ gltf_model.bufferViews.at(gltf_accessor.bufferView) };

				buffer_view vertex_buffer_view{ MakeBufferView(gltf_accessor) };

				D3D11_BUFFER_DESC buffer_desc{};
				buffer_desc.ByteWidth = static_cast<UINT>(vertex_buffer_view.size_in_bytes);
				buffer_desc.Usage = D3D11_USAGE_DEFAULT;
				buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
				D3D11_SUBRESOURCE_DATA subresource_data{};
				subresource_data.pSysMem = gltf_model.buffers.at(gltf_buffer_view.buffer).data.data()
					+ gltf_buffer_view.byteOffset + gltf_accessor.byteOffset;
				hr = device->CreateBuffer(&buffer_desc, &subresource_data,
					vertex_buffer_view.buffer.ReleaseAndGetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				primitive.vertex_buffer_views.emplace(std::make_pair(gltf_attribute.first, vertex_buffer_view));
			}

			// Add dummy attributes if any are missing
			const std::unordered_map<std::string, buffer_view> attributes{
				{"TANGENT",{DXGI_FORMAT_R32G32B32A32_FLOAT}},
				{"TEXCOORD_0",{DXGI_FORMAT_R32G32_FLOAT}},
				{"JOINTS_0",{DXGI_FORMAT_R16G16B16A16_UINT}},
				{"WEIGHTS_0",{DXGI_FORMAT_R32G32B32A32_FLOAT}},
			};
			for (std::unordered_map<std::string, buffer_view>::const_reference attribute : attributes)
			{
				if (primitive.vertex_buffer_views.find(attribute.first) == primitive.vertex_buffer_views.end())
				{
					primitive.vertex_buffer_views.insert(std::make_pair(attribute.first, attribute.second));
				}
			}
		}
	}
}

GltfModel::buffer_view GltfModel::MakeBufferView(const tinygltf::Accessor& accessor)
{
	buffer_view buffer_view_;
	switch (accessor.type)
	{
	case TINYGLTF_TYPE_SCALAR:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			buffer_view_.format = DXGI_FORMAT_R16_UINT;
			buffer_view_.stride_in_bytes = sizeof(USHORT);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view_.format = DXGI_FORMAT_R32_UINT;
			buffer_view_.stride_in_bytes = sizeof(UINT);
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC2:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view_.format = DXGI_FORMAT_R32G32_FLOAT;
			buffer_view_.stride_in_bytes = sizeof(FLOAT) * 2;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC3:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view_.format = DXGI_FORMAT_R32G32B32_FLOAT;
			buffer_view_.stride_in_bytes = sizeof(FLOAT) * 3;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	case TINYGLTF_TYPE_VEC4:
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			buffer_view_.format = DXGI_FORMAT_R16G16B16A16_UINT;
			buffer_view_.stride_in_bytes = sizeof(USHORT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			buffer_view_.format = DXGI_FORMAT_R32G32B32A32_UINT;
			buffer_view_.stride_in_bytes = sizeof(UINT) * 4;
			break;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			buffer_view_.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			buffer_view_.stride_in_bytes = sizeof(FLOAT) * 4;
			break;
		default:
			_ASSERT_EXPR(FALSE, L"This accessor component type is not supported.");
			break;
		}
		break;
	default:
		_ASSERT_EXPR(FALSE, L"This accessor type is not supported.");
		break;
	}
	buffer_view_.size_in_bytes = static_cast<UINT>(accessor.count * buffer_view_.stride_in_bytes);
	return buffer_view_;
}