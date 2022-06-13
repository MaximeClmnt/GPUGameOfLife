#version 430

layout(binding= 0, r32ui) coherent uniform uimage2D image;

const ivec2[8] offsets = ivec2[8](ivec2(-1,-1),	ivec2(0,-1),ivec2(1,-1),
						  ivec2(-1, 0)			   ,ivec2(1, 0),
						  ivec2(-1, 1),	ivec2(0, 1),ivec2(1, 1));

layout(local_size_x= 8, local_size_y= 8) in;
void main( )
{
	ivec2 cellcoord = ivec2(gl_GlobalInvocationID.xy);
	uint n = 0;
	for(int i = 0; i<8; i++){
		uint val = imageLoad(image,cellcoord+offsets[i]).r;
		if(val==1 || val == 2){
		n++;
		}
	}


	
	uint val = imageLoad(image,cellcoord).r;
	if(n==3){
		if(val==0)
			imageStore(image,cellcoord, uvec4(3)); // 3 : la cellule est morte mais elle doit prendre vie
	}
	else if(n!=2){
		if(val==1)
			imageStore(image,cellcoord, uvec4(2)); // 2 : la cellule est vivante mais elle doit mourrir
	}
}