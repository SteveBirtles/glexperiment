#version 310 es
out mediump vec4 fragColor;

in mediump vec2 texCoord;
in mediump vec4 palColour;
in mediump float light;
in mediump float fogFactor;

uniform sampler2D textureNo;
uniform mediump vec4 fogColour;

void main() {

	mediump vec4 texel = texture(textureNo, texCoord) * palColour;
	fragColor = mix(fogColour, texel * light, fogFactor);

}