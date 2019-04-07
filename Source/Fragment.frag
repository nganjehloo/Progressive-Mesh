#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 ViewDir;
in vec3 ViewPos;

out vec4 color;

void main()
{
	vec3 lightdir = normalize(vec3(3.0f, 1.0f, 1.0f));
	vec3 diffuse = dot(Normal, lightdir)*Normal;


	vec3 halfang = normalize(lightdir + ViewDir);
	float specular = pow(clamp(dot(Normal, halfang), 0, 1), 16);

    color = vec4(diffuse + vec3(1.0f)*specular , 1.0f);
	//color = vec4(1.0f);
}