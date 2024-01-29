////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

public:
	ModelClass();
	~ModelClass();

	bool InitializeModel(ID3D11Device *device, char* filename);
	bool InitializeTeapot(ID3D11Device*);
	bool InitializeSphere(ID3D11Device*);
	bool InitializeBox(ID3D11Device*, float xwidth, float yheight, float zdepth);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	
	int GetIndexCount();


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);

	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;

};

#endif