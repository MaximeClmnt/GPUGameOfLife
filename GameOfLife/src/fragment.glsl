
#version 430

layout(binding= 0, r32ui) coherent uniform uimage2D image;

in vec2 texcoord;
out vec4 fragment_color;

void main( )
{
    
    uint val= imageLoad(image, ivec2(texcoord)).r;
    if(val == 2) val=0;
    if(val == 3) val=1;
    imageStore(image, ivec2(texcoord), uvec4(val));
    float i = float(val);
    fragment_color= vec4(i, i, i, 1);
}