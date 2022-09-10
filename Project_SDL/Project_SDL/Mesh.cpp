#include <fstream>
#include <sstream>
#include <iostream>

#include "Mesh.h"
#include "Renderer.h"

void MeshHelper::Mesh::Load_Mesh(string file_name)
{
	// ���� ����� ���� ������
	string mtl_png_file;             
	string line;                     
	string header;                   
	stringstream word;               
	ifstream object_file(file_name); 
	char slash;                      

	// ���� ���� �ӽ� ������
	XMFLOAT3 temp_position;
	XMFLOAT2 temp_texcoord;
	XMFLOAT3 temp_normal;
	Face temp_face;

	unsigned int position_index;
	unsigned int texcoord_index;
	unsigned int normal_index;


	// ���� ����
	if (object_file.is_open())
	{
		// ������ ���� �Ǻ�
		while (!object_file.eof())
		{
			// �ܾ� �ʱ�ȭ
			word.clear();
			// ���Ͽ��� �� �پ� �о����
			getline(object_file, line);
			// line�� �ܾ�� �ɰ���
			word.str(line);
			// �� �տ� ���� �ܾ �б�
			word >> header;

			// line_header_map �� ���� header ���
			// continue �ؼ� ���� ���� �б�
			if (line_header_map.find(header) == line_header_map.end())
				continue;

			// line header ���� �������̽��� �����
			// �ش��ϴ� �Ľ� �۾��� ó��
			switch (line_header_map[header])
			{
			case LineHeader::POSITION: //������ǥ ó��
			{
				word >> temp_position.x >> temp_position.y >> temp_position.z;
				position_bundle.push_back(temp_position);
			}
				break;
			case LineHeader::TEXCOORD: //�ؽ�����ǥ ó��
			{
				word >> temp_texcoord.x >> temp_texcoord.y;
				texcoord_bundle.push_back(temp_texcoord);
			}
				break;
			case LineHeader::NORMAL: //�������� ó��
			{
				word >> temp_normal.x >> temp_normal.y >> temp_normal.z;
				normal_bundle.push_back(temp_normal);
			}
				break;
			case LineHeader::FACE: // �� ó��
			{
				// �ӽ� �� ��ü �ʱ�ȭ
				temp_face.vertex_num = 0;
				temp_face.position_indexes = vector<unsigned int>();
				temp_face.texcoord_indexes = vector<unsigned int>();
				temp_face.normal_indexes = vector<unsigned int>();

				// �鿡 �ش��ϴ� ������ǥ, �ؽ�����ǥ, ��������
				// ������ �������� �ӽ� �� ��ü�� ����.
				while (!word.eof())
				{
					word >> position_index >> slash >> texcoord_index >> slash >> normal_index;
					
					temp_face.position_indexes.push_back(position_index);
					temp_face.texcoord_indexes.push_back(texcoord_index);
					temp_face.normal_indexes.push_back(normal_index);
					temp_face.vertex_num++;
				}
				
				// �ӽ� �� ��ü�� �� ���� vector�� �߰�
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
	// ���� �⺻ ���� �ڿ��� ����
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(default_buffer.GetAddressOf())
	));

	// CPU �޸��� �ڷḦ �⺻ ���ۿ� �����Ϸ���
	// �ӽ� ���ε� ���� ������ ��
	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byte_size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(upload_buffer.GetAddressOf())
	));

	// �⺻ ���ۿ� ������ �ڷḦ ����
	D3D12_SUBRESOURCE_DATA sub_resource_data = {};
	sub_resource_data.pData = init_data;
	sub_resource_data.RowPitch = byte_size;
	sub_resource_data.SlicePitch = sub_resource_data.RowPitch;

	//�⺻ ���� �ڿ������� �ڷ� ���縦 ��û
	// ���������� �����ڸ�, ���� �Լ� UpdateSubresources�� CPU �޸𸮸�
	// �ӽ� ���ε� ���� �����ϰ�, ID3D12CommandList::CopySubresourceRegion��
	//�̿��ؼ� �ӽ� ���ε� ���� �ڷḦ mBuffer�� ����
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

	// ���� : ���� �Լ� ȣ�� ���Ŀ��� upload_buffer�� ���
	// �����ؾ� �Ѵ�. ������ ���縦 �����ϴ� ��� �����
	// ���� ������� �ʾұ� �����̴�.
	// ���簡 �Ϸ�Ǿ����� Ȯ������ �Ŀ� ȣ���ڰ� upload_buffer��
	// �����ϸ� �ȴ�.

	return default_buffer;
}