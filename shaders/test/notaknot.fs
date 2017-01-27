#define	I_MAX	25
#define	E		0.0001
#define PI		3.14
// change this from 0 to 2
#define	MOBIUS	0
// change this from 0 to 4
#define	SHAPE	0
// abs(POWER) > 1.0 and an integer
#define POWER	2

void	rotate(inout vec2 v, float angle);
float	DE(vec3 p);
vec4	march(vec3 pos, vec3 dir);

void mainImage( in vec2 fragCoord)
{
	vec2    uv = (fragCoord / iResolution) * 2 - 1;
	vec3    cameraDir = iForward;

	//window ratio correciton:
	uv.x *= iResolution.x / iResolution.y;

	//perspective view
	float   fov = 1.5;
	vec3    forw = normalize(iForward);
	vec3    right = normalize(cross(forw, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, forw));
	vec3    dir = normalize(uv.x * right + uv.y * up + fov * forw);


	vec3	col;
	vec3	pos = iMoveAmount.xyz / 2;

   	vec4 inter = (march(pos, dir));
    col.xyz = inter.xyz * 3;
	fragColor = vec4(col, 1.0);
}

/*
* branching ahead, prefered this than comment and uncomment ... different combinations give different results
*/

float DE(vec3 p)
{
	float mobius;
    if (0 == MOBIUS || 2 < MOBIUS)
    	mobius = (1.0-1.0/(POWER)) * atan(PI*(p.y*p.x)*sin(iGlobalTime), PI*(-p.y*p.x)*cos(iGlobalTime));
    else if (1 == MOBIUS)
    	mobius = (1.0-1.0/(POWER)) * atan(PI*(p.z*p.x)*sin(iGlobalTime)- PI*(p.z/p.x)*cos(iGlobalTime));
    else if (2 == MOBIUS)
		mobius = (1.0-1.0/(POWER)) * atan(PI*(p.y*p.x)*sin(iGlobalTime)* PI*(p.y*p.x)*cos(iGlobalTime));
	p.x = length(p.xy) - 1.2;
	rotate(p.xz, mobius);
	float m = (1.0 - ((1.0+POWER)))/(1.0*PI);
	float angle = floor(0.5 + m * (PI/2.0-atan(p.x,p.z)))/m;
	rotate(p.xz, angle);
	p.x -= 0.8+abs(cos(iGlobalTime)/2.4);
    if (0 == SHAPE || 4 < SHAPE)
		return length(p.xz)-0.2+abs(sin(iGlobalTime*2.0))*0.1;
    else if (1 == SHAPE)
		return length(p.xy)-0.4-abs(sin(iGlobalTime*3.0))*0.2; // 2-torus
    else if (2 == SHAPE)
		return length(p.yx)-length(p.xz)-0.72+abs(sin(iGlobalTime*3.0))*0.4; // surface
    else if (3 == SHAPE)
		return length(p.yx)-length(p.yz)+0.2-abs(sin(iGlobalTime*2.0))*0.2; // torus construction
    else if (4 == SHAPE)
		return length(p.x+p.y/2.0)-0.2+abs(sin(iGlobalTime*2.0))*0.1; // torus interior from exterior
}

vec4	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec4	step = vec4(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
    	p = pos + dir * dist.y;
        dist.x = DE(p);
        dist.y += dist.x;
        if (dist.x < E)
           break;
        step.w++;
    }
    // cheap colors
    step.x = ((abs(sin(iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    step.y = ((abs(sin(6.0/PI+iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    step.z = ((abs(sin(12.0/PI+iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    return (step);
}

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}
