#version 410 core

uniform vec2 screenSize;
uniform float scale;

out vec4 colorFrag;

void main() {
	colorFrag = vec4(1.0);
	float x = scale * (gl_FragCoord.x / screenSize.x - 1);
    float y = scale * (gl_FragCoord.y / screenSize.y - 1);
}