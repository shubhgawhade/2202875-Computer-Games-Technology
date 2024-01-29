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
	float4 col = input.colour;

	float4 screenPos = input.screenPosition;
	
	// TASK 5
#pragma region COLOUR WRT POSITION OF THE PIXEL

	// Red if x > 0
	if (screenPos.x > 0.f)
	{
		col.r = 1.f;
		col.g = 0.f;
		col.b = 0.f;
		
		// Green if y > 0
		if (screenPos.y > 0.f)
		{
			col.r = 0.f;
			col.g = 1.f;
			col.b = 0.f;
		}
	}

	// Blue if x < 0
	if (screenPos.x < 0.f)
	{
		col.r = 0.f;
		col.g = 0.f;
		col.b = 1.f;
	
		// Yellow if y < 0
		if (screenPos.y < 0.f)
		{
			col.r = 1.f;
			col.g = 1.f;
			col.b = 0.f;
		}
	}

#pragma endregion
	
	//VISUALISE THE ROJECTION MATRIX SPACE GOING FROM -1 to +1
	//return float4(0.5 * (screenPos.r + 1), 0.0, 0.0, 1.0);
	
	return col;

	//return input.colour;
}