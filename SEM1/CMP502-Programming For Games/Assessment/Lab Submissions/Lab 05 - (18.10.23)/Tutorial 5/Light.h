/**
* Stores ambient, diffuse, specular colour, specular power. Also stores direction and position
//from Pauls framework. Modified by  Matt 2019
*/

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <directxmath.h>

using namespace DirectX;

class Light
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

	Light();
	~Light();

	// Setters
	void setAmbientColour(float red, float green, float blue, float alpha);		///< Set ambient colour RGBA
	void setDiffuseColour(float red, float green, float blue, float alpha);		///< Set diffuse colour RGBA
	void setDirection(float x, float y, float z);								///< Set light direction (for directional lights)
	void setSpecularColour(float red, float green, float blue, float alpha);	///< set specular colour RGBA
	void setSpecularPower(float power);											///< Set specular power
	void setPosition(float x, float y, float z);								///< Set light position (for point lights)
	void setLookAt(float x, float y, float z);									///< Set light lookAt (near deprecation)

	// Getters
	DirectX::SimpleMath::Vector4 getAmbientColour();		///< Get ambient colour, returns float4
	DirectX::SimpleMath::Vector4 getDiffuseColour();		///< Get diffuse colour, returns float4
	DirectX::SimpleMath::Vector3 getDirection();			///< Get light direction, returns float3
	DirectX::SimpleMath::Vector4 getSpecularColour();		///< Get specular colour, returns float4
	float getSpecularPower();								///< Get specular power, returns float
	DirectX::SimpleMath::Vector3 getPosition();				///< Get light position, returns float3


protected:
	DirectX::SimpleMath::Vector4 m_ambientColour;
	DirectX::SimpleMath::Vector4 m_diffuseColour;
	DirectX::SimpleMath::Vector3 m_direction;
	DirectX::SimpleMath::Vector4 m_specularColour;
	float m_specularPower;
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector4 m_lookAt;
};

#endif