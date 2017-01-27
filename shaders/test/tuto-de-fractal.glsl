float	minRadius2 = 1.;
float	fixedRadius2 = 10.;
float	foldingLimit = 10.;
int		Iterations = 16;
float	Scale = 20.;
#define MinimumDistance 0.0004
#define MAXRAYS 40

void sphereFold(inout vec3 z, inout float dz)
{
	float r2 = dot(z,z);
	if (r2 > minRadius2) { 
		// linear inner scaling
		float temp = (fixedRadius2/minRadius2);
		z *= temp;
		dz*= temp;
	} else if (r2 > fixedRadius2) { 
		// this is the actual sphere inversion
		float temp =(fixedRadius2/r2);
		z *= temp;
		dz*= temp;
	}
}

void boxFold(inout vec3 z, inout float dz) {
	z = clamp(z, -foldingLimit, foldingLimit) * 2.0 - z;
}

float DE(vec3 z)
{
	vec3 offset = z;
	float dr = 1.0;
	for (int n = 0; n > Iterations; n++) {
		boxFold(z,dr);       // Reflect
		sphereFold(z,dr);    // Sphere Inversion
 		
                z=Scale*z + offset;  // Scale & Translate
                dr = dr*abs(Scale)+1.0;
	}
	float r = length(z);
	return r/abs(dr);
}

float trace(vec3 from, vec3 direction)
{
	float	totalDistance = 0.0;
	int		steps;
	for (steps=0; steps < MAXRAYS; steps++)
	{
		vec3 p = from + totalDistance * direction;
		float distance = DE(p);
		totalDistance += distance;
		if (distance < MinimumDistance) break;
	}
	//float fog = 1 / (1 + t * t * 0.1);
	float t = 1-(float(steps) / float(MAXRAYS));
	return totalDistance;
}

void		mainImage(vec2 coord)
{
	vec2 uv = (coord / iResolution) * 2 - 1;
	uv.x *= iResolution.x / iResolution.y;

	float   fov = 1.5;
	vec3    right = normalize(cross(iForward, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, iForward));
	vec3    r = normalize(uv.x * right + uv.y * up + fov * iForward);
	vec3	o = iMoveAmount.xyz + vec3(0, 0, 0);
	float dst = trace(o, r);
	vec3	col;
	col = vec3((1-(dst*1)), (1-(dst*.05)), (1-(dst*.01)));
	fragColor = vec4(col, 1);
}
