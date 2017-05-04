#define Power 8
#define Bailout 4
#define MinimumDistance 0.00004
#define MAXRAYS 300
float pi=3.14159265;

float DistanceEstimator(vec3 pos)
{
	/*vec3 q = fract(pos) * 2 - 1;

	return length(q) - .1;*/
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	int i = 0;
	for (i = 0; i < 12; i++) {
		r = length(z);
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y, z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power + 20*sin(iGlobalTime);
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z += pos + r / dr;
	}
	return .5 * log(r) * r / dr;
	/*float x = pos.x;
	float y = pos.y;
	float z = pos.z;
	float r    = sqrt(x*x + y*y + z*z );
	for (int i = 0; i < 10; i++)
	{
		float yang = atan(sqrt(x*x + y*y) , z  );
		float zang = atan(y , x);
		x = (r*r) * sin( yang*2 + 0.5f*pi ) * cos(zang*2 +pi);
		y = (r*r) * sin( yang*2 + 0.5f*pi ) * sin(zang*2 +pi);
		z = (r*r) * cos( yang*2 + 0.5f*pi );
	}
	return log(r) * r;*/
}

vec3	fractNormal(vec3 pos, vec3 dir)
{
	return (normalize(vec3(DistanceEstimator(pos+dir.x)-DistanceEstimator(pos-dir.x),
				DistanceEstimator(pos+dir.y)-DistanceEstimator(pos-dir.y),
				DistanceEstimator(pos+dir.z)-DistanceEstimator(pos-dir.z))));
}

float trace(vec3 from, vec3 direction)
{
	float	totalDistance = 0.0;
	int		steps;
	for (steps=0; steps < MAXRAYS; steps++)
	{
		vec3 p = from + totalDistance * direction;
		float distance = DistanceEstimator(p);
		totalDistance += distance;
		if (distance < MinimumDistance) break;
	}
	//float fog = 1 / (1 + t * t * 0.1);
	float t = 1-(float(steps) / float(MAXRAYS));
	return totalDistance;
}

float Shadow( in vec3 ro, in vec3 rd)
{
    float res = 1.0;
    float t = 0.05;
    float h;

    for (int i = 0; i < 8; i++)
    {
        h = DistanceEstimator( ro + rd*t );
        res = min(6.0*h / t, res);
        t += h;
    }
    return max(res, 0.0);
}

#define SCALE 2.8
#define MINRAD2 .25
float minRad2 = clamp(MINRAD2, 1.0e-9, 1.0);
vec4 scale = vec4(SCALE, SCALE, SCALE, abs(SCALE)) / minRad2;
vec3 surfaceColour1 = vec3(.1, .0, 0.);
vec3 surfaceColour2 = vec3(0,0,1);
vec3 surfaceColour3 = vec3(.6, 0.3, 1.00);
vec3		Colour(vec3 pos)
{
	scale.z = 0.0;
	vec3 p = pos;
	vec3 p0 = p;
	float trap = 2.0;
    
	for (int i = 0; i < 8; i++)
	{
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;
		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(minRad2/r2, minRad2), 0.0, 1.0);

		p = p * scale.xyz + p0.zyx;
		trap = min(trap, r2);
	}
	// |c.x|: log final distance (fractional iteration count)
	// |c.y|: spherical orbit trap at (0,0,0)
	vec2 c = clamp(vec2( 0.3333*log(dot(p,p))-1.0, sqrt(trap) ), 0.0, 1.0);

    surfaceColour1 = sin(iGlobalTime) * mix( surfaceColour1, vec3(2, 2.0, 5.), 1);
	surfaceColour2 = cos(iGlobalTime) * surfaceColour2;
	return mix(mix(surfaceColour1, surfaceColour2, c.y * 1.5), surfaceColour3, c.y);
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
	float	dst = trace(o, r);
	vec3	col;
	//vec3 nor = fractNormal(o, r);
//	vec3 nor = vec3(1);
//	vec3 light = vec3(0, 0, 0) - o;
//	float atten = length(light);

//	light /= atten;

//	float bri = max(dot(light, nor), 0.0) / pow(atten, 1.5) * .15;

//	float shaSpot = Shadow(o, light);

	if (dst < 1)
		col = mix(col, vec3(1, 1, .5), 1-dst);

	//col = Colour(o + r * dst);
//	col = (col * bri * shaSpot) + (col);
//	vec3 ref = reflect(r, nor);
//	col += pow(max(dot(light,  ref), 0.0), 10.0) * 2.0 * shaSpot * bri;

	fragColor = vec4(col, 1);
}
