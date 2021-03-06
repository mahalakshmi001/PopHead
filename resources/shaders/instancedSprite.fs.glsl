#version 330 core 

in DATA
{
    vec4 color;
    vec2 texCoords;
	vec2 texSize;
    flat int textureSlotRef; 
} fs_in;

out vec4 fragColor;

uniform sampler2D textures[32];

void main()
{
	vec2 locationWithinTexel = fract(fs_in.texCoords);
	float alpha = 0.1;
	vec2 interpolationAmount = clamp(locationWithinTexel / alpha, 0.0, 0.5) + clamp((locationWithinTexel - 1.0) / alpha + 0.5, 0.0, 0.5);
	vec2 finalTexCoords = (floor(fs_in.texCoords) + interpolationAmount) / fs_in.texSize; 
    fragColor = texture(textures[fs_in.textureSlotRef], finalTexCoords) * fs_in.color;
}

// TODO: Make alpha be set in the smart way
