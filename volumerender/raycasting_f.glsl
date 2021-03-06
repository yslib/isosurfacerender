#version 430 core

//uniform sampler1D texTransfunc;
uniform sampler2D texTransfunc;
uniform sampler1DArray iTexTransfunc;
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler3D texVolume;
uniform sampler3D maskVolumeTex;



uniform float step;
uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;

uniform vec3 lightdir;
uniform vec3 halfway;
//in vec2 textureRectCoord;
out vec4 fragColor;

uniform vec3 eye;
uniform vec3 forward;
uniform float tmin;
uniform float alpha;
uniform float beta;
uniform float radius;
uniform vec3 axis;


vec3 PhongShading(vec3 samplePos, vec3 diffuseColor)
{
	vec3 shadedValue = vec3(0, 0, 0);

	vec3 N;
	N.x = (texture(texVolume, samplePos+vec3(step,0,0) ).w - texture(texVolume, samplePos+vec3(-step,0,0) ).w) - 1.0;
	N.y = (texture(texVolume, samplePos+vec3(0,step,0) ).w - texture(texVolume, samplePos+vec3(0,-step,0) ).w) - 1.0;
	N.z = (texture(texVolume, samplePos+vec3(0,0,step) ).w - texture(texVolume, samplePos+vec3(0,0,-step) ).w) - 1.0;

	N = N * 2.0 - 1.0;
	N = -normalize(N);

	vec3 L = lightdir;
	vec3 H = halfway;

	//specularcolor
	//vec3 H = normalize(V+L);
	float NdotH = pow(max(dot(N, H), 0.0), shininess);
	float NdotL = max(dot(N, L), 0.0);

	vec3 ambient = ka * diffuseColor.rgb;
	vec3 specular = ks * NdotH * vec3(1.0, 1.0, 1.0);
	vec3 diffuse = kd * NdotL * diffuseColor.rgb;

	shadedValue = specular + diffuse + ambient;
	return shadedValue;
}
vec4 bg = vec4(1.0f, 1.0f, 1.0f, 1.00f);


void main()
{
	vec3 rayStart = texture2DRect(texStartPos, vec2(gl_FragCoord)).xyz;
	vec3 rayEnd = texture2DRect(texEndPos, vec2(gl_FragCoord)).xyz;
	
	vec3 start2end = rayEnd - rayStart;
	vec3 direction = normalize(start2end);
	//vec4 color = texture2DRect(texIntermediateResult,textureRectCoord);
	vec4 color = vec4(0,0,0,0);
	float distance = dot(direction, start2end);
	int steps = int(distance / step);

	if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
		//fragColor=vec4(1.0,0.0,0.0,1.0);
		fragColor = bg; // Background Colors
		return;
	}
	ivec3 texSize = textureSize(texVolume,0);

	for (int i = 0; i < steps; ++i) {
		vec3 samplePoint = rayStart + direction * step * (float(i) + 0.5);
		vec4 scalar = texture(texVolume, samplePoint);
		float mask = float(texture(maskVolumeTex,samplePoint));
		//if(mask == 0)continue;
		vec4 sampledColor = vec4(0);
		if(mask*255 - int(mask*255) <= 10e-5)
		{
			sampledColor = texture(iTexTransfunc,vec2(scalar.r,mask*255));
		}else{
			sampledColor = texture(texTransfunc,vec2(scalar.r,mask*255/(textureSize(texTransfunc,0).y - 1.0)));
		}
		sampledColor.rgb = PhongShading(samplePoint, sampledColor.rgb);
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
			break;
	}
	if (color.a == 0.0)discard;
	

	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;

	//float mask = int(imageLoad(maskVolume,ivec3(vec3(0,1,1))));
	//fragColor = vec4(mask,mask,mask,1.0);
}
