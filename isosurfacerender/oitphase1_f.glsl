#version 430 core


//layout(early_fragment_tests) in;

layout(binding = 0 ,offset = 0) uniform atomic_uint indexCounter;
layout(binding = 1, r32ui) uniform uimage2DRect headPointerImage;

//layout(binding = 2, rgba32ui) uniform uimageBuffer listBuffers;

layout(std430, binding =3) buffer ListBuffer{uvec4 buf[];}listBuffers;

// Phong Shading
in vec3 frag_normal;
in vec3 frag_pos;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec4 object_color;
uniform vec3 view_pos;

float near = 10; 
float far  = 1000.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}


out vec4 frag_color;

vec4 PhongShading()
{
	vec3 lpos = view_pos;

	vec3 view_norm = normalize(view_pos-frag_pos);

    vec3 frag_norm = normalize(frag_normal);

	if(dot(view_norm,frag_norm) < 0)
		frag_norm = -frag_norm;

   // the light normalized vector points to light
    vec3 light_norm = normalize(lpos-frag_pos);

    //ambient
    vec4 ambient = 0.4*vec4(light_color,1.0);

   // diffuse
    float diff_strength = max(dot(light_norm,frag_norm),0.0)*0.5;
    vec4 diffuse = diff_strength*vec4(light_color,1.0);

    //specular
    
    vec3 light_reflect_dir = reflect(-light_norm,frag_norm);
    float spec = pow(max(dot(light_reflect_dir,view_norm),0.0),64);

    vec4 specular = 0.5*spec*vec4(light_color,1.0);
   //vec4 result = (ambient+diffuse+specular)*object_color;
	vec4 result = (ambient) * object_color + diffuse* object_color  + specular * vec4(1.0);
    //fragment output
	//frag_color = vec4(result, 1.0);
	return vec4(result);
}
// Phong Shading End

void main(void){

	// TODO::
	vec4 fragColor = PhongShading();

	uint newHead = atomicCounterIncrement(indexCounter);
	uint oldHead = imageAtomicExchange(headPointerImage,ivec2(gl_FragCoord.xy),newHead);

	uvec4 item;
	item.x = oldHead;
	item.y = packUnorm4x8(fragColor);
	item.z = floatBitsToUint(LinearizeDepth(gl_FragCoord.z));
	//item.z = floatBitsToUint(gl_FragCoord.z);
	item.w = 0;
	//imageStore(listBuffers,int(newHead),item);
	listBuffers.buf[newHead] = item;
	//frag_color = vec4(1.0);
}