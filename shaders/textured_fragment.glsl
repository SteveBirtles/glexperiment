#version 330 core
out vec4 fragColor;

in vec2 texCoord;
in vec4 palColour;
in float light;
in float fogFactor;

uniform sampler2D textureNo;
uniform vec4 fogColour;

void main() {

	vec4 texel = texture(textureNo, texCoord) * palColour;
	fragColor = mix(fogColour, texel * light, fogFactor);

}