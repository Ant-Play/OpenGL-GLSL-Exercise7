uniform vec3 l_dir;//光源位置
uniform vec4 l_diffuse;
uniform vec4 l_ambient;
uniform vec4 l_specular;
uniform float l_shininess;
uniform vec4 m_diffuse;
uniform vec4 m_ambient;
uniform vec4 m_specular;
uniform float m_shininess;
varying vec4 color;//向片段着色其传递的参数

void main(){

	vec4 ambient = l_ambient;
	vec4 diffuse = l_diffuse;
	vec4 specular = l_specular;
	float shininess = l_shininess;
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