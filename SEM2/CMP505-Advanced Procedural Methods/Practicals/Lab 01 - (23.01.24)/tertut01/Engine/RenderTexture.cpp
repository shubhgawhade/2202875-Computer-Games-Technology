// render texture
// alternative render target
#include "pch.h"
#include "rendertexture.h"

// Initialise texture object based on provided dimensions. Usually to match window.
RenderTexture::RenderTexture(ID3D11Device* device, int ltextureWidth, int ltextureHeight, float screenNear, float screenFar)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	textureWidth = ltextureWidth;
	textureHeight = ltextureHeight;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	// Create the render target texture.
	result = device->CreateTexture2D(&textureDesc, NULL, &renderTargetTexture);
	
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	// Create the render target view.
	result = device->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
	
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	// Create the shader resource view.
	result = device->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &shaderResourceView);
	
	// Set up the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	
	// Set up the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	
	// Setup the viewport for rendering.
	viewport.Width = (float)textureWidth;
	viewport.Height = (float)textureHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	projectionMatrix = XMMatrixPerspectiveFovLH(((float)XM_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenFar);

	// Create an orthographic projection matrix for 2D rendering.
	orthoMatrix = XMMatrixOrthographicLH((float)textureWidth, (float)textureHeight, screenNear, screenFar);
}

// Release resources.
RenderTexture::~RenderTexture()
{
	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if (depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if (shaderResourceView)
	{
		shaderResourceView->Release();
		shaderResourceView = 0;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}

	if (renderTargetTexture)
	{
		renderTargetTexture->Release();
		renderTargetTexture = 0;
	}
}

// Set this renderTexture as the current render target.
// All rendering is now store here, rather than the back buffer.
void RenderTexture::setRenderTarget(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
//	deviceContext->RSSetViewports(1, &viewport);
}

// Clear render texture to specified colour. Similar to clearing the back buffer, ready for the next frame.
void RenderTexture::clearRenderTarget(ID3D11DeviceContext* deviceContext, float red, float green, float blue, float alpha)
{
	float color[4];
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer and depth buffer.
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTexture::getShaderResourceView()
{
	return shaderResourceView;
}

XMMATRIX RenderTexture::getProjectionMatrix()
{
	return projectionMatrix;
}

XMMATRIX RenderTexture::getOrthoMatrix()
{
	return orthoMatrix;
}

int RenderTexture::getTextureWidth()
{
	return textureWidth;
}

int RenderTexture::getTextureHeight()
{
	return textureHeight;
}