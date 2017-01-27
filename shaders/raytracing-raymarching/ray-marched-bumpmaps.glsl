/*
	The main cool thing here is that the distance functions are transformed by a texture image.
*/

//==RAY MARCHING CONSTANTS=========================================================
#define EPSILON .0001
#define MAX_VIEW_STEPS 100
#define MAX_SHADOW_STEPS 64
#define OCCLUSION_SAMPLES 8.0
#define OCCLUSION_FACTOR .5
#define MAX_DEPTH 10.0
#define BUMP_FACTOR .04
#define TEX_SCALE_FACTOR .4

//==OBJECT CONSTANTS========================================================
#define BOX_POS vec3 (0.0, 0.0, 0.0)
#define BOX_DIM vec3 (1.0, 1.0, 1.0)

#define SPHERE_POS vec3(0.0, 0.0, 0.0)
#define SPHERE_DIM 1.25

#define LIGHT_COLOR vec4(1.0)
#define PEN_FACTOR 50.0

#define MAT_COLOR vec4(1.0)
#define MAT_GLOSS 1.0

//==RENDERING STRUCTURES===========================================================
/*
	A structure for a simple light.
*/
struct Light
{
	vec3 position;
	vec4 color;
	float brightness;
	float penumbraFactor;
};

/*
	A structure for a less simple material.
*/
struct Material
{
	vec4 color;
	float gloss;
};
	
//==CAMERA FUNCTIONS================================================================
/*
	By TEKF: https://www.shadertoy.com/view/XdsGDB
	Set up a camera looking at the scene.
	origin - camera is positioned relative to, and looking at, this point
	dist - how far camera is from origin
	rotation - about x & y axes, by left-hand screw rule, relative to camera looking along +z
	zoom - the relative length of the lens
*/
void camPolar( out vec3 pos, out vec3 dir, in vec3 origin, in vec2 rotation, in float dist, in float zoom, in vec2 fragCoord )
{
	// get rotation coefficients
	vec2 c = cos(rotation);
	vec4 s;
	s.xy = sin(rotation);
	s.zw = -s.xy;

	// ray in view space
	dir.xy = fragCoord.xy - iResolution.xy*.5;
	dir.z = iResolution.y*zoom;
	dir = normalize(dir);
	
	// rotate ray
	dir.yz = dir.yz*c.x + dir.zy*s.zx;
	dir.xz = dir.xz*c.y + dir.zx*s.yw;
	
	// position camera
	pos = origin - dist*vec3(c.x*s.y,s.z,c.x*c.y);
}	
	
//==TEXTURING FUNCTIONS=============================================================
// by reinder. This is clever as two hamsters feeding three hamsters.
vec4 tex3D( in vec3 pos, in vec3 normal, sampler2D sampler )
{
	return 	texture2D( sampler, pos.yz )*abs(normal.x)+ 
			texture2D( sampler, pos.xz )*abs(normal.y)+ 
			texture2D( sampler, pos.xy )*abs(normal.z);
}

//==DISTANCE FUNCTIONS==============================================================
/*
	Returns the distance to the surface of a sphere with the given position and radius.
	Note that this function returns a signed value, so if you are within the sphere the
	distance will be negative.
*/
float distSphere( vec3 samplePos, vec3 spherePos, float radius)
{
	vec3 normal;
	float bump = 0.0;
	if(length(samplePos-spherePos) < radius+BUMP_FACTOR)
	{	
		normal = normalize(samplePos-spherePos);
		bump = tex3D(samplePos*TEX_SCALE_FACTOR, normal, iChannel0).r*BUMP_FACTOR;
	}
	return length(samplePos-spherePos)-radius+bump;
}

/*
	Returns the distance to the surface of a box with the given position and dimensions.
	This also returns a signed distance. If the returned value is negative, you are within
	the box.
*/
float distBox(vec3 samplePos, vec3 boxPos, vec3 boxDim)
{	
	vec3 normal;
	float bump = 0.0;
	if(length(samplePos-boxPos) < length(boxDim))
	{
	   	normal = normalize(samplePos-boxPos);
 		bump = tex3D(samplePos*TEX_SCALE_FACTOR, normal, iChannel0).r*BUMP_FACTOR;
	}
	vec3 d = abs(samplePos-boxPos) - boxDim;
	return min(max(d.x,max(d.y,d.z)),0.0) +
		length(max(d,0.0))+bump;
}

float getDist(vec3 samplePos)
{
	return min(distBox(samplePos, BOX_POS, BOX_DIM), 
			   distBox(samplePos, vec3(0.0, -5.0, 0.0), vec3(4.0, 4.0, 4.0)));
}

//==RAY MARCHING FUNCTIONS=========================================================
/*
		Marches the 3D point <pos> along the given direction.
	When the point is either stepped the maximum number of times,
	has passed the maximum distance, or is within a set distance
	from geometry the function returns. 
		Note that the position is passed by reference and is modified
	for use within the function.
*/
void marchThroughField(inout vec3 pos, vec3 dir, vec3 eye)
{
	float dist;
	for(int i = 0; i < MAX_VIEW_STEPS; i++)
	{
		dist = getDist(pos);
		if(dist < EPSILON || length(pos-eye) > MAX_DEPTH-EPSILON)			
			return;
		else	
			pos += dir*dist;
	}
	return;
}

//==LIGHT CALCULATION FUNCTIONS=====================================================
/*
	Returns the surface normal of a point in the distance function.
*/
vec3 getNormal(vec3 pos)
{
	float d=getDist(pos);
	return normalize(vec3( getDist(pos+vec3(EPSILON,0,0))-d, getDist(pos+vec3(0,EPSILON,0))-d, getDist(pos+vec3(0,0,EPSILON))-d ));
}

/*
	Calculates the ambient occlusion factor at a given point in space.
	Uses IQ's marched normal distance comparison technique.
*/
float calcOcclusion(vec3 pos, vec3 surfaceNormal)
{
	float result = 0.0;
	vec3 normalPos = vec3(pos);
	for(float i = 0.0; i < OCCLUSION_SAMPLES; i+=1.0)
	{
		normalPos += surfaceNormal * (1.0/OCCLUSION_SAMPLES);
		result += (1.0/exp2(i)) * (i/OCCLUSION_SAMPLES)-getDist(normalPos);
	}
	return 1.0-(OCCLUSION_FACTOR*result);
}

/*
	Calculates how much light remains if shadows are considered.
	Uses IQ's soft shadows. Gosh, the guy created it all.
*/
float calcShadow( vec3 origin, vec3 lightDir, Light light)
{
	float dist;
	float result = 1.0;
	float lightDist = length(light.position-origin);
	
	vec3 pos = vec3(origin)+(lightDir*(EPSILON*15.0+BUMP_FACTOR));
	
	for(int i = 0; i < MAX_SHADOW_STEPS; i++)
	{
		dist = getDist(pos);
		if(dist < EPSILON)
		{
			return 0.0;
		}
		if(length(pos-origin) > lightDist || length(pos-origin) > MAX_DEPTH)
		{
			return result;
		}
		pos+=lightDir*dist;
		if( length(pos-origin) < lightDist )
		{
			result = min( result, light.penumbraFactor*dist / length(pos-origin) );
		}
	}
	return result;
}

/*
	Returns the product of the Phong lighting equation on a point in space given
	a light and the surface's material.
*/
vec4 calcLighting(vec3 samplePos, vec3 eye, Light light, Material material)
{
	float lightDist = length(light.position-samplePos);
	vec3 lightDir = normalize(light.position-samplePos);
	vec3 eyeDir = normalize(samplePos-eye);
	vec3 surfaceNormal = getNormal(samplePos);
	vec3 reflection = normalize(reflect(eyeDir, surfaceNormal));
	
	float specular = pow(max( 0.0, dot(lightDir, reflection)), 72.0);
	float diffuse = max( 0.0, dot(lightDir, surfaceNormal));
	float ambient = .05;
	
	float attenuation = min(1.0, 1.0/(lightDist/light.brightness));
	float shadow = calcShadow(samplePos, lightDir, light);
	float occlusion = calcOcclusion(samplePos, surfaceNormal);
	
	return light.color*material.color*clamp(((specular+diffuse)*shadow*attenuation)+(ambient*occlusion), 0.0, 1.0);
}

vec4 shade(vec3 pos, vec3 eye, Light light, Material material)
{
	if(length(pos-eye) > MAX_DEPTH-EPSILON) return vec4(0.0);
	else return calcLighting(pos, eye, light, material);
}

void mainImage( in vec2 fragCoord )
{
	vec3 pos, dir, eye;
	camPolar(pos, dir, vec3(0.0), vec2(.5,iMouse.x*.0075+.025), 3.75, 1.0, fragCoord);
	eye = vec3(pos);
	Light light = Light(vec3(3.0*sin(iGlobalTime+1.3), 3.5, 3.0*cos(iGlobalTime+1.3)), LIGHT_COLOR, 3.0, PEN_FACTOR);
	Material mat = Material(MAT_COLOR, MAT_GLOSS);
	
	marchThroughField(pos, dir, eye);
	
	fragColor = shade(pos, eye, light, mat);
}

