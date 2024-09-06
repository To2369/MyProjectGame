#include "GltfModel.h"
#include "../misc.h"
#include <stack>
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tiny_gltf.h>

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

// ノード情報の取り出し
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

// メッシュ情報の取り出し
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