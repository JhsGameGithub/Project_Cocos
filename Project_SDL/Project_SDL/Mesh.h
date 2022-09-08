#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <DirectXMath.h>

#include "Component.h"

namespace MeshHelper
{
	using namespace std;
	using namespace DirectX;

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
}