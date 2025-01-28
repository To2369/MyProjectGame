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
	struct GltfModelScene
	{
		std::string name;
		std::vector<int> nodes; // Array of 'root' nodes
	};
	std::vector<GltfModelScene> scenes;

	struct Node
	{
		std::string name;
		int skin{ -1 }; //index of skin refereced by this node
		int mesh{ -1 }; //index of mesh refereced by this node

		std::vector<int> children; // An array of indices of child nodes of this node

		// Local transforms
		DirectX::XMFLOAT4 rotation{ 0,0,0,1 };
		DirectX::XMFLOAT3 scale{ 1,1,1 };
		DirectX::XMFLOAT3 translation{ 0,0,0 };

		DirectX::XMFLOAT4X4 globalTransform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};
	std::vector<Node> nodes;

	struct BufferView
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		size_t strideInBytes{ 0 };
		size_t sizeInBytes{ 0 };
		size_t count() const
		{
			return sizeInBytes / strideInBytes;
		}
	};
	struct Mesh
	{
		std::string name;
		struct  Primitive
		{
			int material;
			std::map<std::string, BufferView> vertexBufferViews;
			BufferView indexBufferView;
		};
		std::vector<Primitive> primitives;
	};
	std::vector<Mesh> meshes;

	struct TextureInfo
	{
		int index = -1;
		int texcoord = 0;
	};

	struct NormalTextureInfo
	{
		int index = -1;
		int texcoord = 0;
		float scale = 1;
	};

	struct OcclusionTextureInfo
	{
		int index = -1;
		int texcoord = 0;
		float strength = 1;
	};

	struct PbrMetalicRoughness
	{
		float baseColorFactor[4] = { 1,1,1,1 };
		TextureInfo baseColorTexture;
		float metallicFactor = 1;
		float roughnessFactor = 1;
		TextureInfo metalicRoughnessTexture;
	};

	struct Material {
		std::string name;
		struct Cbuffer
		{
			float emissiveFactor[3] = { 0,0,0 };
			int alphaMode = 0;	// "OPAQUE":0,"MASK":1,"BLEND":2
			float alphaCutoff = 0.5f;
			bool doubleSided = false;

			PbrMetalicRoughness pbrMetallicRougness;

			NormalTextureInfo normalTexture;
			OcclusionTextureInfo occlusionTexture;
			TextureInfo emissiveTexture;
		};
		Cbuffer data;
	};
	std::vector<Material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView;

	struct Texture
	{
		std::string name;
		int source{ -1 };
	};
	std::vector<Texture> textures;

	struct Image
	{
		std::string name;
		int width{ -1 };
		int height{ -1 };
		int component{ -1 };
		int bits{ -1 };
		int pixelType{ -1 };
		int bufferView;
		std::string mimeType;
		std::string uri;
		bool asIs{ false };
	};
	std::vector<Image> images;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews;
public:
	GltfModel(ID3D11Device* device, const std::string& filename);
	virtual ~GltfModel() = default;

	void Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& world);

	void CumulateTransforms(std::vector<Node>& nodes);

	// ノード情報の取り出し
	void FetchNodes(const tinygltf::Model& gltfModel);

	// メッシュ情報の取り出し
	void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel);

	// マテリアル情報の取り出し
	void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltfModel);

	//テクスチャ情報取り出し
	void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltfModel);

	BufferView MakeBufferView(const tinygltf::Accessor& accessor);
private:
	std::string filename;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	struct primitiveConstants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int hasTangent{ 0 };
		int skin{ -1 };
		int pad;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveCbuffer;
};