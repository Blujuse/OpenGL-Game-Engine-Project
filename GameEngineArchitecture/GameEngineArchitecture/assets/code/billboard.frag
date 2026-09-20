#version 410

// Input to fragment shader
in vec2 uv;

// Texture
uniform sampler2D sampler;

// From vertex shader
in float fog_amount;

// Fog colour from cpp
uniform vec3 fog_Colour;

// Final fragment colour
out vec4 fragmentColour;

// Shader body
void main()
{
	// Determine fragment colour from texture based on uv co-oords
	vec4 texColour = texture(sampler, uv);

	// Instead of blending, check the aplpha value and anything
	// less than 0.9 results in the fragment being discarded
	if (texColour.a < 0.9)
		discard;
		
	fragmentColour = mix(texColour.rgba, vec4(fog_Colour, 1.0), fog_amount);
}

