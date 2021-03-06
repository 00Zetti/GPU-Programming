// DONE: Uniform-Parameter einf�gen.
uniform float time;

void main() {

	gl_Position = gl_ModelViewMatrix * gl_Vertex;
	vec3 normal = gl_NormalMatrix * gl_Normal;
	normal = normalize(normal);
	vec4 normal4d = {normal[0], normal[1], normal[2], 1};

	// DONE: Pumping-Teapot Formel einf�gen.
	// feel free to change the factor 0.25f to have some fun. 
	// 1.f HULKPOT!!!
	gl_Position = gl_Position + 1.f * abs(sin(0.125f * time)) * normal4d;

	vec4 outColor = gl_FrontMaterial.emission
				+ gl_FrontMaterial.ambient * gl_LightModel.ambient
				+ gl_FrontMaterial.ambient * gl_LightSource[0].ambient;

	// Calculate normalized light vector (from vertex to light source)
	vec3 light = normalize(gl_LightSource[0].position.xyz - gl_Position.xyz);
	
	// Calculate diffuse lighting
	outColor += gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse
               * max(0.0, dot(light, normal));

	// Calculate halfway vector
	// This is the vector halfway between the directions from the vertex to
	// the light source and to the viewer. Since the camera is at the origin,
	// the vector to the viewer is (0 0 1).
	vec3 half = normalize(light + vec3(0, 0, 1));

	// Calculate specular lighting
	outColor +=  gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(max(0.0, dot(half, normal)), gl_FrontMaterial.shininess);

	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ProjectionMatrix * gl_Position; 

	gl_FrontColor = outColor;
}
