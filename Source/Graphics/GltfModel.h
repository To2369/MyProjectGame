#pragma once
#define NOMINMAX
#include<d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include<stack>
#include <unordered_map>
#include<tiny_gltf.h>

class GltfModel
{
public:
	struct scene
	{
		std::string name;
		std::vector<int> nodes; // Array of 'root' nodes
	};
	std::vector<scene> scenes;

	struct node
	{
		std::string name;
		int skin{ -1 }; //index of skin refereced by this node
		int mesh{ -1 }; //index of mesh refereced by this node

		std::vector<int> children; // An array of indices of child nodes of this node

		// Local transforms
		DirectX::XMFLOAT4 rotation{ 0,0,0,1 };
		DirectX::XMFLOAT3 scale{ 1,1,1 };
		DirectX::XMFLOAT3 translation{ 0,0,0 };

		DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};
	std::vector<node> nodes;

	struct buffer_view
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		size_t stride_in_bytes{ 0 };
		size_t size_in_bytes{ 0 };
		size_t count() const
		{
			return size_in_bytes / stride_in_bytes;
		}
	};
	struct mesh
	{
		std::string name;
		struct  primitive
		{
			int material;
			std::map<std::string, buffer_view> vertex_buffer_views;
			buffer_view index_buffer_view;
		};
		std::vector<primitive> primitives;
	};
	std::vector<mesh> meshes;
public:
	GltfModel(ID3D11Device* device, const std::string& filename);
	virtual ~GltfModel() = default;

	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world);

	void CumulateTransforms(std::vector<node>& nodes);

	// ノード情報の取り出し
	void FetchNodes(const tinygltf::Model& gltf_model);

	// メッシュ情報の取り出し
	void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltf_model);
	buffer_view MakeBufferView(const tinygltf::Accessor& accessor);
private:
	std::string filename;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;
};