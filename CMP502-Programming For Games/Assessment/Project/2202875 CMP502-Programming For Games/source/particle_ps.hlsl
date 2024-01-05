////////////////////////////////////////////////////////////////////////////////
// Filename: particle.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t1);
SamplerState SampleType : register(s1);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	// matrix rot : ROT;
    float2 tex : TEXCOORD0;
	float4 color : COLOR;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ParticlePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;
	
	float2 rotatedTex;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

	// Rotate the texture coordinates by the input angle.
	// rotatedTex.x = input.tex.x * cos(0) - input.tex.y * sin(0);
	// rotatedTex.y = input.tex.x * sin(0) + input.tex.y * cos(0);
	//
	// // Sample the pixel color from the texture using the rotated texture coordinates.
	// textureColor = shaderTexture.Sample(SampleType, rotatedTex);

	if(textureColor.a < 1)
	{
		float averagecol = 1. - (textureColor.r+ textureColor.g + textureColor.b)/3.;
		float whitetoalpha = (averagecol) + 0;
		finalColor = averagecol;
		finalColor.a = whitetoalpha;
		// color.r = 1;
		return finalColor;
	}

	
	// Combine the texture color and the particle color to get the final color result.
    finalColor = textureColor * input.color;

    return finalColor;
}