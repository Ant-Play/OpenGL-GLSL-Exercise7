uniform vec3 l_dir;//��Դλ��
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shininess;
varying vec4 color;//��Ƭ����ɫ�䴫�ݵĲ���

void main(){
	// set the specular term to black
	vec4 spec = vec4(0.0);

	vec3 n = normalize(gl_NormalMatrix * gl_Normal);

	float intensity = max(dot(n,l_dir), 0.0);

	// if the vertex is lit compute the specular color
	if(intensity > 0.0){
		// compute position in camera space
		vec3 pos = vec3(gl_ModelViewMatrix * gl_Vertex);
		// compute eye vector and normalize it 
		vec3 eye = normalize(-pos);
		// compute the half vector
		vec3 h = normalize(l_dir + eye);

		// compute the specular term into spec
		float intSpec = max(dot(h,n), 0.0);
		spec = specular * pow(intSpec, shininess);
	}

	color = max(intensity * diffuse + spec, ambient);
	gl_Position = ftransform();
}