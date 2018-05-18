#define pi			3.14159
#define ITER		100
#define EPSY		.0005
#define SHADOW_ITER	8
#define GRAIL		1
#define LIGHT		2

const vec3	skyColor = vec3(.2, .2, .2);
const float	farClipPlane = 70;
vec3		mcol;
vec3		C;
vec3		lightPosition;
vec3		lightColor;
int			material = 0;

float		DE(in vec3 p)
{
	float dr=1.0,r=length(p);
	//C=p;
	for(int i=0;i<10;i++)
	{
		if(r > 20.0)
			break;
		dr = dr * 2.0 * r;
		float psi = abs(mod(atan(p.z, p.y)+pi / 8.0, pi / 4.0) - pi / 8.0);
		p.yz = vec2(cos(psi), sin(psi)) * length(p.yz);
		vec3 p2 = p * p;
		p = vec3(vec2(p2.x - p2.y, 2.0 * p.x * p.y) * (1.0 - p2.z / (p2.x + p2.y + p2.z)), 2.0 * p.z * sqrt(p2.x + p2.y)) + C;	
		r = length(p);

		//we take the distance frmo the thrid iteration for the coloring
		if(i == 3)
			mcol = p;
	}
	return min(log(r) * r / max(dr,1.0),1.0);
}

float		lightDE(vec3 p)
{
	return length(p - lightPosition) - 0.05;
}

float		raymarch(in vec3 ro, in vec3 rd)
{
	float t = 0, d;

	for (int i = 0; i < ITER; i++)
	{
		vec3	p = ro + rd * t;
		float	d1 = DE(p);
		float	d2 = lightDE(p);

	    d = min(d1, d2);

	    t += d * .5;

	    if (d < EPSY || t > farClipPlane)
	    {
	    	material = (d1 < d2) ? GRAIL : LIGHT;
	    	return t;
	    }
	}
	return -1;
}

vec3		GetNormal(vec3 pos, float distance)
{
	vec2 eps = vec2(distance, 0.0);
	vec3 nor = vec3(
			DE(pos+eps.xyy) - DE(pos-eps.xyy),
			DE(pos+eps.yxy) - DE(pos-eps.yxy),
			DE(pos+eps.yyx) - DE(pos-eps.yyx));
	return normalize(nor);
}

float		Shadow( in vec3 ro, in vec3 rd)
{
	float res = 1.0;
	float t = 0.01;
	float h;

	for (int i = 0; i < SHADOW_ITER; i++)
	{
		h = DE( ro + rd*t );
		res = min(6.0*h / t, res);
		t += h;
	}
	return max(res, 0.4);
}

float		GetAO(in vec3 p, in vec3 nor)
{
	float dist = 0.02;
	vec3 spos = p + nor * dist;
	float sdist = DE(spos);
	return clamp(sdist / dist, 0.0, 1.0);
}

vec3		Coloring(in float dist, in vec3 point)
{
	return sqrt((sin(exp(mcol*2.1) + fract(mcol * 3) + cos(iGlobalTime / 5))));
}

void		mainImage( in vec2 fragCoord )
{
	vec2    uv = (fragCoord / iResolution) * 2 - 1;
	vec3    cameraDir = iForward;
	vec3	col;

	//window ratio correciton:
	uv.x *= iResolution.x / iResolution.y;

	//perspective view
	float   fov = 1.5;
	vec3    forw = normalize(iForward);
	vec3    right = normalize(cross(forw, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, forw));
	vec3    rd = normalize(uv.x * right + uv.y * up + fov * forw);

	vec3	ro = iMoveAmount.xyz;

	C = vec3(sin(iGlobalTime / 5.) / 2 - .5,cos(iGlobalTime / 10.) / 2 + .5, min(sin(iGlobalTime / 2) / 3, .0));
	lightColor = vec3(1, 1, 1) * 1.;
	lightPosition = vec3(sin(iGlobalTime / 4.) * 2, sin(iGlobalTime) * .5 + 1, cos(iGlobalTime) * 1);

	float	d = raymarch(ro, rd);
	vec3	p = ro + rd * d;


	if (d == -1)
	{
		fragColor = vec4(skyColor, 1);
		return ;
	}

	if (material == GRAIL)
		col = Coloring(d, p) * 3.5;
	else
	{
		fragColor = vec4(lightColor, 1);
		return ;
	}

	vec3	nor = GetNormal(p, d * d * 0.002 + 0.0005);
	float	ao = GetAO(p, nor);
	float	sha = Shadow(p, normalize(lightPosition - p));
	float	bri = max(dot(normalize(lightPosition), nor), 0.0) * .3;

	col = (col * bri * min(ao, sha)) * lightColor;


	fragColor = vec4(col, 1);
}
