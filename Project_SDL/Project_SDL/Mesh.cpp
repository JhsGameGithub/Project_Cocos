#include <fstream>
#include <sstream>
#include <iostream>

#include "Mesh.h"
#include "Renderer.h"

void MeshHelper::Mesh::Load_Mesh(string file_name)
{
	// 파일 입출력 관련 변수들
	string mtl_png_file;             
	string line;                     
	string header;                   
	stringstream word;               
	ifstream object_file(file_name); 
	char slash;                      

	// 정점 관련 임시 변수들
	XMFLOAT3 temp_position;
	XMFLOAT2 temp_texcoord;
	XMFLOAT3 temp_normal;
	Face temp_face;

	unsigned int position_index;
	unsigned int texcoord_index;
	unsigned int normal_index;


	// 파일 열기
	if (object_file.is_open())
	{
		// 파일의 끝을 판별
		while (!object_file.eof())
		{
			// 단어 초기화
			word.clear();
			// 파일에서 한 줄씩 읽어오기
			getline(object_file, line);
			// line을 단어별로 쪼개기
			word.str(line);
			// 맨 앞에 오는 단어를 읽기
			word >> header;

			// line_header_map 에 없는 header 라면
			// continue 해서 다음 줄을 읽기
			if (line_header_map.find(header) == line_header_map.end())
				continue;

			// line header 별로 점프케이스를 만들어
			// 해당하는 파싱 작업을 처리
			switch (line_header_map[header])
			{
			case LineHeader::POSITION: //정점좌표 처리
			{
				word >> temp_position.x >> temp_position.y >> temp_position.z;
				position_bundle.push_back(temp_position);
			}
				break;
			case LineHeader::TEXCOORD: //텍스쳐좌표 처리
			{
				word >> temp_texcoord.x >> temp_texcoord.y;
				texcoord_bundle.push_back(temp_texcoord);
			}
				break;
			case LineHeader::NORMAL: //법선벡터 처리
			{
				word >> temp_normal.x >> temp_normal.y >> temp_normal.z;
				normal_bundle.push_back(temp_normal);
			}
				break;
			case LineHeader::FACE: // 면 처리
			{
				// 임시 면 객체 초기화
				temp_face.vertex_num = 0;
				temp_face.position_indexes = vector<unsigned int>();
				temp_face.texcoord_indexes = vector<unsigned int>();
				temp_face.normal_indexes = vector<unsigned int>();

				// 면에 해당하는 정점좌표, 텍스쳐좌표, 법선벡터
				// 정점의 갯수까지 임시 면 객체에 저장.
				while (!word.eof())
				{
					word >> position_index >> slash >> texcoord_index >> slash >> normal_index;
					
					temp_face.position_indexes.push_back(position_index);
					temp_face.texcoord_indexes.push_back(texcoord_index);
					temp_face.normal_indexes.push_back(normal_index);
					temp_face.vertex_num++;
				}
				
				// 임시 면 객체를 면 모음 vector에 추가
				face_bundle.push_back(temp_face);
			}
				break;
			case LineHeader::MTLLIB:
				word >> mtl_png_file;
				break;
			}
		}
		object_file.close();
	}
}

static Microsoft::WRL::ComPtr<ID3D12Resource> MeshHelper::Create_Default_Buffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* cmd_list,
	const void* init_data,
	UINT64 byte_size,
	Microsoft::WRL::ComPtr<ID3D12Resource>& upload_buffer)
{

	using namespace Microsoft;
	using namespace WRL;

	ComPtr<ID3D12Resource> default_buffer;
	// 실제 기본 버퍼 자원을 생성
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(default_buffer.GetAddressOf())
	));

	// CPU 메모리의 자료를 기본 버퍼에 복사하려면
	// 임시 업로드 힙을 만들어야 함
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(upload_buffer.GetAddressOf())
	));

	// 기본 버퍼에 복사할 자료를 서술
	D3D12_SUBRESOURCE_DATA sub_resource_data = {};
	sub_resource_data.pData = init_data;
	sub_resource_data.RowPitch = byte_size;
	sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

	//기본 버퍼 자원으로의 자료 복사를 요청
	// 개략적으로 말하자면, 보조 함수 UpdateSubresources는 CPU 메모리를
	// 임시 업로드 힙에 복사하고, ID3D12CommandList::CopySubresourceRegion을
	//이용해서 임시 업로드 힙의 자료를 mBuffer에 복사
	cmd_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST
		));

	UpdateSubresources<1>(
		cmd_list,
		default_buffer.Get(),
		upload_buffer.Get(),
		0,
		0,
		1,
		&sub_resource_data);

	cmd_list->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ
		));

	// 주의 : 위의 함수 호출 이후에도 upload_buffer를 계속
	// 유지해야 한다. 실제로 복사를 수행하는 명령 목록이
	// 아직 실행되지 않았기 때문이다.
	// 복사가 완료되었음이 확실해진 후에 호출자가 upload_buffer를
	// 해제하면 된다.

	return default_buffer;
}