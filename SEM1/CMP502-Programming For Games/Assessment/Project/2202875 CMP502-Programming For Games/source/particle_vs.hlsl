////////////////////////////////////////////////////////////////////////////////
// Filename: particle.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b1)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION;
    // float3 rotation : ROTATION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

// matrix rotateX(float theta) {
// 	float c = cos(theta);
// 	float s = sin(theta);
// 	return matrix(
// 		float4(1, 0, 0, 0),
// 		float4(0, c, -s, 0),
// 		float4(0, s, c, 0),
// 		float4 (0, 0, 0, 1)
// 	);
// }
//
// matrix rotateY(float theta) {
// 	float c = cos(theta);
// 	float s = sin(theta);
// 	return matrix(
// 		float4(c, 0, s, 0),
// 		float4(0, 1, 0, 0),
// 		float4(-s, 0, c, 0),
// 		float4 (0, 0, 0, 1)
// 	);
// }
//
// matrix rotateZ(float theta) {
// 	float c = cos(theta);
// 	float s = sin(theta);
// 	return matrix(
// 		float4(c, -s, 0, 0),
// 		float4(s, c, 0, 0),
// 		float4(0, 0, 1, 0),
// 		float4 (0, 0, 0, 1)
// 	);
// }
//
// matrix Rotation(float3 radians)
// {
// 	matrix mymat[3];
// 	matrix mymatSum;
//
// 	mymat[0] = rotateX(radians.x);
// 	mymat[1] = rotateY(radians.y);
// 	mymat[2] = rotateZ(radians.z);
//
// 	mymatSum = mymat[2] * mymat[1] * mymat[0];
// 	
// 	return mymatSum;
// }

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ParticleVertexShader(VertexInputType input)
{
    PixelInputType output;
	
	output.position = float4(input.position,1);
	output.position.w = 1.0f;

	//i apply the transformation to the vertex input.position
	// output.position = mul(output.position, Rotation(float3(0,3.14f,0)));
	// output.position = mul(output.position, input.rotation);

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// Store the particle color for the pixel shader. 
	output.color = input.color;

    return output;
}