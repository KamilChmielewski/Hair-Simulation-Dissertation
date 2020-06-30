#include "ModelLoader.h"
#include <map>

using namespace std;
using namespace DirectX;

namespace ModelLoader
{
	void LoadVertices(string& buffer);
	void LoadFaces(string& buffer);
	void LoadTexCoords(string& buffer);
	void LoadNormals(string& buffer);
	void LoadVB(ID3D11Device* _pd3dDevice, Mesh* mesh);
	void LoadIB(ID3D11Device* _pd3dDevice, Mesh* mesh);
	//VertexToOutIndex is the data used to find a vertex check weather one already exists or not and then use that and put it into result which will be put into indices
	//check pre existing vertex then return result of that index
	bool GetSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned short>& VertexToOutIndex, unsigned short& result);
	//will get the data loadeded into the readData and index it and store it into the mesh which will be used to draw
	void IndexVBO(
		std::vector<XMFLOAT3>& out_vertices, std::vector<XMFLOAT3>& out_normals, std::vector<XMFLOAT2>& out_uv,
		std::vector<XMFLOAT3>& indexed_vertices, std::vector<XMFLOAT3>& indexed_normals, std::vector<XMFLOAT2>& indexed_uvs,
		Mesh& mesh);

	std::vector<unsigned short> indices;
	unsigned short vertexIndex[3], uvIndex[3], normalIndex[3];
	std::vector<unsigned short> vertexIndices, uvIndices, normalIndices; //These indices will be then pushed back into the out_temp

	std::vector<XMFLOAT3>temp_vertices;
	std::vector<XMFLOAT3>temp_normals;
	std::vector<XMFLOAT2>temp_uv;

	std::vector<XMFLOAT3>out_vertices;
	std::vector<XMFLOAT3>out_normals;
	std::vector<XMFLOAT2>out_uv;

	std::vector<XMFLOAT3> indexed_vertices;
	std::vector<XMFLOAT2> indexed_uvs;
	std::vector<XMFLOAT3> indexed_normals;


	//make this in the while loop

	void LoadVB(ID3D11Device* _pd3dDevice, Mesh* mesh)
	{
		HRESULT hr;

		// Create vertex buffer
		// Set vertex buffer
		ID3D11Buffer* _pVertexBuffer2;
		_pVertexBuffer2 = nullptr;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = mesh->indexed_vertices.size() * sizeof(Vertex);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &mesh->elements[0];

		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &mesh->_pVertexBuffer);
	}

	void LoadIB(ID3D11Device* _pd3dDevice, Mesh* mesh)
	{
		HRESULT hr;

		ID3D11Buffer* _pIndexBuffer2;
		_pIndexBuffer2 = nullptr;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short) * mesh->Indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &mesh->Indices[0];
		hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &mesh->_pIndexBuffer);
	}

	void LoadVertices(string& buffer)
	{
		if (buffer[0] == 'v' && buffer[1] == ' ')
		{
			XMFLOAT3 temp3;

			float tmpX, tmpY, tmpZ;
			sscanf_s(buffer.c_str(), "v %f %f %f", &tmpX, &tmpY, &tmpZ);

			temp3.x = tmpX;
			temp3.y = tmpY;
			temp3.z = tmpZ;
			temp_vertices.push_back(temp3);
		}
	}

	void LoadTexCoords(string& buffer)
	{
		if (buffer[0] == 'v' && buffer[1] == 't')
		{
			XMFLOAT2 temp2;

			float tmpU, tmpV;

			sscanf_s(buffer.c_str(), "vt %f %f", &tmpU, &tmpV);
			temp2.x = tmpU;
			temp2.y = tmpV;

			temp_uv.push_back(temp2);
		}
	}

	void LoadNormals(string& buffer)
	{
		if (buffer[0] == 'v' && buffer[1] == 'n')
		{
			XMFLOAT3 temp3;

			float tmpX, tmpY, tmpZ;
			sscanf_s(buffer.c_str(), "vn %f %f %f", &tmpX, &tmpY, &tmpZ);

			temp3.x = tmpX;
			temp3.y = tmpY;
			temp3.z = tmpZ;

			temp_normals.push_back(temp3);
		}
	}

	void LoadFaces(string& buffer)
	{
		if (buffer[0] == 'f' && buffer[1] == ' ')
		{
			if (sscanf_s(buffer.c_str(), "f %i//%i %i//%i %i//%i", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]) == 6) //Vertex and normal
			{

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}

			if (sscanf_s(buffer.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]) == 9) //vertex normal and texture
			{

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
			}

			if (sscanf_s(buffer.c_str(), "f %i/%i %i/%i %i/%i", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]) == 6) //vertex and texture
			{
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);

				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
			}
		}
	}

	bool GetSimilarVertexIndex(PackedVertex& packed, std::map<PackedVertex, unsigned short>& VertexToOutIndex, unsigned short& result)
	{
		std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
		if (it == VertexToOutIndex.end())
		{
			return false;
		}
		else
		{
			result = it->second;
			return true;
		}

	}

	void IndexVBO(
		std::vector<XMFLOAT3>& out_vertices, std::vector<XMFLOAT3>& out_normals, std::vector<XMFLOAT2>& out_uv,
		std::vector<XMFLOAT3>& indexed_vertices, std::vector<XMFLOAT3>& indexed_normals, std::vector<XMFLOAT2>& indexed_uvs,
		Mesh& mesh)
	{
		std::map<PackedVertex, unsigned short> VertexToOutIndex;

		for (unsigned int i = 0; i < out_vertices.size(); i++)
		{
			PackedVertex packed = { out_vertices[i], out_normals[i], out_uv[i] };

			//Try to find a similar index
			unsigned short index;
			bool found = GetSimilarVertexIndex(packed, VertexToOutIndex, index);

			if (found)
			{
				// A similar vertex is already in the VBO, use it instead !
				mesh.Indices.push_back(index);
			}
			else
			{
				indexed_vertices.push_back(out_vertices[i]);
				indexed_uvs.push_back(out_uv[i]);
				indexed_normals.push_back(out_normals[i]);
				unsigned short newindex = (unsigned short)indexed_vertices.size() - 1;
				mesh.Indices.push_back(newindex);
				VertexToOutIndex[packed] = newindex;
			}

		}
	}

	Mesh* ModelLoader::Load(char* path, ID3D11Device* _pd3dDevice)
	{
		Mesh* mesh = new Mesh();

		ifstream file;
		file.open(path);

		string buffer;

		if (!file.good())
		{
			cerr << "Can't open texture file " << path << endl;
			return nullptr;
		}

		while (!file.eof())
		{
			getline(file, buffer);

			if (buffer[0] == '#')
			{
				continue;
			}
			else
			{
				LoadVertices(buffer);

				LoadNormals(buffer);

				LoadTexCoords(buffer);

				LoadFaces(buffer); //* defeferncing
			}
		}

		file.close();

		for (unsigned short i = 0; i < vertexIndices.size(); i++)
		{

			unsigned int vertexIndex = vertexIndices[i];
			unsigned int normalIndex = normalIndices[i];
			unsigned int uvIndex = uvIndices[i];

			XMFLOAT2 faceUV;
			XMFLOAT3 faceVertices;
			XMFLOAT3 faceNormals;

			faceUV = temp_uv[uvIndex - 1];
			faceVertices = temp_vertices[vertexIndex - 1];
			faceNormals = temp_normals[normalIndex - 1];

			out_uv.push_back(faceUV);
			out_normals.push_back(faceNormals);
			out_vertices.push_back(faceVertices);
		}


		IndexVBO(out_vertices, out_normals, out_uv, indexed_vertices, indexed_normals, indexed_uvs, *mesh);

		for (unsigned int i = 0; i < indexed_vertices.size(); i++)
		{
			mesh->elements.push_back({ indexed_vertices[i], indexed_normals[i], indexed_uvs[i] });
			mesh->indexed_vertices.push_back({ indexed_vertices[i] });
			mesh->indexed_normals.push_back({ indexed_normals[i] });
			mesh->indexed_uvs.push_back({ indexed_uvs[i] });
		}

		LoadVB(_pd3dDevice, mesh);
		LoadIB(_pd3dDevice, mesh);

		temp_vertices.clear();
		temp_normals.clear();
		temp_uv.clear();

		out_vertices.clear();
		out_normals.clear();
		out_uv.clear();

		vertexIndices.clear();
		normalIndices.clear();
		uvIndices.clear();

		return mesh;
	}


}