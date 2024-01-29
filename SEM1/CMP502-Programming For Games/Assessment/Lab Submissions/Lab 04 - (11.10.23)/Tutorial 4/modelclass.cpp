////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelclass.h"

using namespace DirectX;

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

}
ModelClass::~ModelClass()
{
}


bool ModelClass::InitializeModel(ID3D11Device *device, char* filename)
{
	LoadModel(filename);
	InitializeBuffers(device);
	return false;
}

bool ModelClass::InitializeTeapot(ID3D11Device* device)
{
	GeometricPrimitive::CreateTeapot(preFabVertices, preFabIndices, 1, 8, false);
	m_vertexCount	= preFabVertices.size();
	m_indexCount	= preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::InitializeSphere(ID3D11Device *device)
{
	GeometricPrimitive::CreateSphere(preFabVertices, preFabIndices, 1, 8, false);
	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::InitializeBox(ID3D11Device * device, float xwidth, float yheight, float zdepth)
{
	GeometricPrimitive::CreateBox(preFabVertices, preFabIndices,
		DirectX::SimpleMath::Vector3(xwidth, yheight, zdepth),false);
	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	return true;
}


void ModelClass::Shutdown()
{

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}
	
	// Load the vertex array and index array with data from the pre-fab
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position	= DirectX::SimpleMath::Vector3(preFabVertices[i].position.x, preFabVertices[i].position.y, preFabVertices[i].position.z);
		vertices[i].texture		= DirectX::SimpleMath::Vector2(preFabVertices[i].textureCoordinate.x, preFabVertices[i].textureCoordinate.y);
		vertices[i].normal		= DirectX::SimpleMath::Vector3(preFabVertices[i].normal.x, preFabVertices[i].normal.y, preFabVertices[i].normal.z);
	}
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = preFabIndices[i];
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ModelClass::LoadModel(char* filename)
{
	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> norms;
	std::vector<XMFLOAT2> texCs;
	std::vector<unsigned int> faces;

	FILE* file;// = fopen(filename, "r");
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if (err != 0)
		//if (file == NULL)
	{
		return false;
	}

	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
					&face[3], &face[4], &face[5],
					&face[6], &face[7], &face[8]);
				if (matches != 9)
				{
					// Parser error, or not triangle faces
					return false;
				}

				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}


			}
		}
	}

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	//// Create the model using the vertex count that was read in.
	m_vertexCount = numFaces * 3;
//	model = new ModelType[vertexCount];

	// "Unroll" the loaded obj information into a list of triangles.
	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		VertexPositionNormalTexture tempVertex;
		tempVertex.position.x = verts[(faces[f + 0] - 1)].x;
		tempVertex.position.y = verts[(faces[f + 0] - 1)].y;
		tempVertex.position.z = verts[(faces[f + 0] - 1)].z;

		tempVertex.textureCoordinate.x = texCs[(faces[f + 1] - 1)].x;
		tempVertex.textureCoordinate.y = texCs[(faces[f + 1] - 1)].y;
			
		tempVertex.normal.x = norms[(faces[f + 2] - 1)].x;
		tempVertex.normal.y = norms[(faces[f + 2] - 1)].y;
		tempVertex.normal.z = norms[(faces[f + 2] - 1)].z;

		//increase index count
		preFabVertices.push_back(tempVertex);
		
		int tempIndex;
		tempIndex = vIndex;
		preFabIndices.push_back(tempIndex);
		vIndex++;
	}

	m_indexCount = vIndex;

	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();
	return true;
}


void ModelClass::ReleaseModel()
{
	return;
}