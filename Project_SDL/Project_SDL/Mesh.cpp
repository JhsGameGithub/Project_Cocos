#include <fstream>
#include <sstream>
#include <iostream>

#include "Mesh.h"

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