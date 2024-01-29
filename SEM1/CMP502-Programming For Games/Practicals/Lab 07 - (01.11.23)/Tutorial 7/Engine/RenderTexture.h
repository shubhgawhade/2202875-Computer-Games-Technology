/**
* \class Render Texture
*
* \brief Alternative render target, stored as a texture.
*
* Is a texture object that can be used as an alternative render target. Store what is rendered to it, instead of back buffer.
* Size can be speicified but traditionally this will match window size.
* Used in post processing and multi-render stages.
*
* \author Paul Robertson
*/

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class RenderTexture
{
public:
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	/** \brief Initialises render textures
	*	Required renderer device, specified width and height of texture/target, and near + far planes
	*/
	RenderTexture(ID3D11Device* device, int textureWidth, int textureHeight, float screenNear, float screenDepth);
	~RenderTexture();

	void setRenderTarget(ID3D11DeviceContext* deviceContext);		///< Set this render texture as the render target
	void clearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha);	///< Empties the render texture, provide device context and RGBA (background colour)
	ID3D11ShaderResourceView* getShaderResourceView();			///< Get the data from this render target as a texture resource.

	XMMATRIX getProjectionMatrix();		///< Get the projection matrix related to this render target (Could be different based on dimensions or near/far plane)
	XMMATRIX getOrthoMatrix();			///< Get the orthographics matrix stored within this render target (could be different based on dimension)

	int getTextureWidth();		///< Get width of this render texture
	int getTextureHeight();		///< Get height of this render texture

private:
	int textureWidth, textureHeight;
	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
	XMMATRIX projectionMatrix;
	XMMATRIX orthoMatrix;
};

#endif