#version 150 compatibility


out Vertex{
	vec3 vertexEyeSpace;
	vec3 normal;
	vec2 _texcoord;
};

void main(){
    vertexEyeSpace = vec3(gl_ModelViewMatrix * gl_Vertex);
    normal = normalize(gl_NormalMatrix * gl_Normal);
	_texcoord = gl_MultiTexCoord0.xy ;
    gl_Position = gl_ModelViewMatrix * gl_Vertex;
}
