// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

struct InputType
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
	float4 screenPosition : SCREEN_POSITION;
};


float4 main(InputType input) : SV_TARGET
{

	return input.colour;
}