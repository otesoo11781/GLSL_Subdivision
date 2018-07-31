#version 150 compatibility

layout(triangles) in;
layout(triangle_strip, max_vertices=85) out;

in Vertex{
	vec3 vertexEyeSpace;
    vec3 normal;
	vec2 _texcoord ;
}vertex[];
out vec3 vertexEyeSpace ;
out vec3 normal ;
out vec2 tex;

uniform int level ;
uniform float Radius;
uniform vec4 Centroid ;

vec3 V0 , V1 , V2;
vec2 T0 , T1 , T2 ;
void calculate(float s , float t ){
	vec3 v = V0 + s * V1 + t * V2 ;
	normal =normalize(gl_NormalMatrix *(v -  (gl_ModelViewMatrix*Centroid).xyz));
	vertexEyeSpace = ((gl_ModelViewMatrix*Centroid).xyz) + normal * Radius ;
	tex = T0 + s * T1 + t * T2 ; 
	gl_Position = gl_ProjectionMatrix * vec4(vertexEyeSpace , 1.0f) ;
	EmitVertex() ;
}
void main(){
    if(level == 0){
		for(int i = 0 ; i < 3 ;++i){
			vertexEyeSpace = vertex[i].vertexEyeSpace ;
			normal = vertex[i].normal ;
			tex = vertex[i]._texcoord ;
			gl_Position = gl_ProjectionMatrix * vec4(vertexEyeSpace , 1.0f) ;
			EmitVertex();
		}
		EndPrimitive();
	}
	else if(level ==1){
		V0 = vertex[0].vertexEyeSpace;
		V1 = vertex[1].vertexEyeSpace - vertex[0].vertexEyeSpace ;
		V2 = vertex[2].vertexEyeSpace - vertex[0].vertexEyeSpace ;
		
		T0 = vertex[0]._texcoord ;
		T1 = vertex[1]._texcoord - vertex[0]._texcoord ;
		T2 = vertex[2]._texcoord - vertex[0]._texcoord ;
		
		int numlayer = level * 2 ;
		for(int i = 0 ; i < numlayer ; i++){
			float top = 1 - 0.5 * i ;
			float bot = 0.5 - 0.5 * i ;
			float horizen = 0 ;
			for(int j = 0 ; j < i+1 ; j++){
				calculate(bot , horizen) ;
				calculate(top , horizen) ;
				horizen += 0.5 ;
			}
			calculate(bot ,horizen) ;
			EndPrimitive() ;
		}
	}
	else{
		V0 = vertex[0].vertexEyeSpace;
		V1 = vertex[1].vertexEyeSpace - vertex[0].vertexEyeSpace ;
		V2 = vertex[2].vertexEyeSpace - vertex[0].vertexEyeSpace ;
		
		T0 = vertex[0]._texcoord ;
		T1 = vertex[1]._texcoord - vertex[0]._texcoord ;
		T2 = vertex[2]._texcoord - vertex[0]._texcoord ;
		
		int numlayer = level * 2 ;
		for(int i = 0 ; i < numlayer ; i++){
			float top = 1 - 0.25 * i ;
			float bot = 0.75 - 0.25 * i ;
			float horizen = 0 ;
			for(int j = 0 ; j < i+1 ; j++){
				calculate(bot , horizen) ;
				calculate(top , horizen) ;
				horizen += 0.25 ;
			}
			calculate(bot ,horizen) ;
			EndPrimitive() ;
		}
	}
}

