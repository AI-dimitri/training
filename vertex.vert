#version 330 core

layout (location = 0) in vec3 aPos; // This is required
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

// translate * rotate * scale * vec
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// Remember about "swizzling"!
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
