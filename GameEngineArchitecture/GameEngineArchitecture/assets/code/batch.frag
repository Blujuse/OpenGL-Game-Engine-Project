#version 410

// uv input
in vec2 uv;

// 20. Texture id input
flat in int textureId;

// output fragment colour
out vec4 fragmentColour;

// 21. Define a sampler2D array of 16 elements
// Have to give a number of elements and OpenGL
// indicates there should be 16 texture units
// at a minimum.  Could be more but after 16
// depends on hardware
uniform sampler2D textures[100];

void main()
{
	// 22. Get a colour from the texture associated with the vertex
	fragmentColour = texture(textures[textureId], uv).rgba;
}

