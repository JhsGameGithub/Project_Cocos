#include <fstream>
#include <sstream>
#include <iostream>

#include "Mesh.h"

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