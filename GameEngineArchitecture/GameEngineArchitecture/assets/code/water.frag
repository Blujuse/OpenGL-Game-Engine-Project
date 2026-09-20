#version 410

// Input to fragment shader
in vec2 uv;

// Fog amount from vertex shader
in float fog_amount;

// Fog colour from C++
uniform vec3 fog_colour;

// Texture
uniform sampler2D sampler;

// Final fragment colour
out vec4 fragmentColour;

// Shader body
void main()
{
	// Determine fragment colour from texture based on uv co-oords
	vec4 texColour = texture(sampler, uv);

	fragmentColour = mix(texColour, vec4(fog_colour, 1.0f), fog_amount);
}

