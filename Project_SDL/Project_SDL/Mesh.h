#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "Component.h"

namespace MeshHelper
{
	using namespace std;
	using namespace DirectX;
	using namespace Microsoft;
	using namespace WRL;

	struct Face
	{
		vector<unsigned int> position_indexes;
		vector<unsigned int> texcoord_indexes;
		vector<unsigned int> normal_indexes;
		unsigned int vertex_num;
	};

	enum LineHeader
	{
		INVALID = -1,
		POSITION,
		TEXCOORD,
		NORMAL,
		FACE,
		MTLLIB,
		SIZE
	};

	static unordered_map<string, LineHeader> line_header_map = {
		{ "v",      LineHeader::POSITION },
		{ "vt",     LineHeader::TEXCOORD },
		{ "vn",     LineHeader::NORMAL   },
		{ "f",      LineHeader::FACE     },
		{ "mtllib", LineHeader::MTLLIB   }
	};

	class Mesh : public Component
	{
	private:
		vector<XMFLOAT3> position_bundle;
		vector<XMFLOAT2> texcoord_bundle;
		vector<XMFLOAT3> normal_bundle;
		vector<Face>     face_bundle;

	public:
		void Load_Mesh(std::string file_name);
	};
	
	static ComPtr<ID3D12Resource> Create_Default_Buffer(
		ID3D12Device* device, 
		ID3D12GraphicsCommandList* cmd_list, 
		const void* init_data, 
		UINT64 byte_size, 
		ComPtr<ID3D12Resource>& upload_buffer);
}