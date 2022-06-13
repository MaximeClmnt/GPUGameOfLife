#version 330

const float width = 1024;
const float height = 512;
const float win_width = 1080;
const float win_height = 720;

const vec2 positions[4]= vec2[4]( vec2(0, 0), vec2(width/win_width, 0), vec2(width/win_width, height/win_height), vec2(0, height/win_height));
const vec2 texcoords[4]= vec2[4]( vec2(0, 0), vec2(width, 0), vec2(width, height), vec2(0, height));
vec2 offset = vec2(-width/(win_width*2),-height/(win_height*2));
float zoom = 2;

out vec2 texcoord;

void main() {
    vec2 p= (positions[gl_VertexID] + offset)*zoom;
    texcoord= texcoords[gl_VertexID];
    gl_Position= vec4(p,0,1);
}