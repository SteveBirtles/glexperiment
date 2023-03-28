#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoordIn;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 centre;
layout(location = 4) in vec3 palColourIn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 sun;
uniform bool illuminate;

out vec2 texCoord;
out float light;
out vec4 palColour;
out float fogFactor;

void main() {

	vec4 transformedPosition = view * model * vec4(position, 1);

	gl_Position = projection * transformedPosition;	
	float positionDistanceSquared = dot(transformedPosition, transformedPosition);

	texCoord = texCoordIn;
	palColour = vec4(palColourIn, 1);

	if(!illuminate) {
		light = 1.0;
	} else {
		light = 0.25 + max(0, abs(dot(normalize(centre - sun), normal))) * 0.75;
	}

	if(positionDistanceSquared < 14400) {
		fogFactor = 1.0;
	} else {
		fogFactor = clamp((128.0 - sqrt(positionDistanceSquared)) / 8.0, 0.0, 1.0);
	}

}
