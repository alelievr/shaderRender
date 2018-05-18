#define ITER	80
#define EPSY	0.001

bool Julia = false;
bool CutZ = false;
float SkipColorIterations = 0;
float ColorIterations = 25;

vec3 JuliaC = vec3(-1.81984,-0.006,-0.00684);
float Power = 8;
float Bailout = 2;
float Iterations = 11;

vec4 orbitTrap;

vec2 complexSqr(vec2 a){
	return  vec2( a.x*a.x -  a.y*a.y,2.0*a.x*a.y);
}


vec3 triplexSqr(vec3 a) {
	a = abs(a);
	float r = length(a.xy);
	vec2 newXY =  complexSqr(a.xy/r);
	vec2 newRZ = complexSqr(vec2(r,a.z));

	newXY = newXY * newRZ.x;

	return vec3(newXY.x, newXY.y, newRZ.y);
}


vec3 triplexPow(inout vec3 z) {
	z = abs(z);
	float zr0 = length(z);
	float zo0 = asin( z.z/zr0 );
	float zi0 = atan( z.y,z.x );
	float zr = pow( zr0, Power-1.0 );
	float zo = zo0 * Power;
	float zi = zi0 * Power;
	zr *= zr0;
	z  = zr*vec3( cos(zo)*cos(zi), cos(zo)*sin(zi), sin(zo) );
	return z;
}


float DE(vec3 pos) {
	float delta = exp2(-18. + max( floor(log2(length(pos))),0.0) );

	vec3 pos2 = pos + vec3(delta,0.,0.);
	vec3 pos3 = pos + vec3(0.,delta,0.);
	vec3 pos4 = pos + vec3(0.,0.,delta);


	vec3 z = pos;
	vec3 z2 = pos2;
	vec3 z3 = pos3;
	vec3 z4 = pos4;


	float r;
	//float dr=1.0;
	int i=0;
	r=length(z);
	while(r<Bailout && (i<Iterations)) {
	    /*
	       if (AlternateVersion) {
	       powN2(z,r,dr);
	       } else {
	       powN1(z,r,dr);
	       }
	     */
	    z = triplexSqr(z);
	    z+=(Julia ? JuliaC : pos);

	    z2 = triplexSqr(z2);
	    z2 +=(Julia ? JuliaC : pos2);

	    z3 = triplexSqr(z3);
	    z3 +=(Julia ? JuliaC : pos3);

	    z4 = triplexSqr(z4);
	    z4 +=(Julia ? JuliaC : pos4);

	    r=length(z);
	    if (i >= SkipColorIterations &&  i<ColorIterations) orbitTrap = min(orbitTrap, abs(vec4(z.x,z.y,z.z,r*r)));
	    i++;
	}
	float dx = length(z2-z)/delta;
	float dy = length(z3-z)/delta;
	float dz = length(z4-z)/delta;

	float dr = length(vec3(dx,dy,dz));

	float de = 0.5*log(r)*r/dr;

	if (CutZ) de = max(de,pos.z);
	return de;
	/*
	   Use this code for some nice intersections (Power=2)
	   float a =  max(0.5*log(r)*r/dr, abs(pos.y));
	   float b = 1000;
	   if (pos.y>0)  b = 0.5*log(r)*r/dr;
	   return min(min(a, b),
	   max(0.5*log(r)*r/dr, abs(pos.z)));
	 */
}


vec2 march(vec3 ro, vec3 rd)
{
	float scale = 2;
	float t = 0, d;
	for (int i = 0; i < ITER; i++){
		vec3 p = ro + rd * t;
		p /= scale;
		d = DE(p) * scale;
		t += d;

		if (d < EPSY) return vec2(t, d);
		if (d > 200) return vec2(-1.0);
	}
    return vec2(-1.0);
}

vec3	rainbow(float dist)
{
    float tau=6.28318;

    return sqrt(.5+sin((dist*1+vec3(0,2,1)/3.)*tau)*.5);
}

vec3 GetNormal(vec3 pos, float distance)
{
	vec2 eps = vec2(distance, 0.0);
	vec3 nor = vec3(
			DE(pos+eps.xyy) - DE(pos-eps.xyy),
			DE(pos+eps.yxy) - DE(pos-eps.yxy),
			DE(pos+eps.yyx) - DE(pos-eps.yyx));
	return normalize(nor);
}

vec3 Lit(vec3 col, vec3 normal)
{
	vec3 lightDir = vec3(0, 1, 0);
	return col * max(.6, dot(normal, lightDir));
}

void mainImage(in vec2 fragCoord)
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
	vec3    rd = normalize(uv.x * right + uv.y * up + fov * forw);

	vec3 ro = iMoveAmount.xyz;

    vec2 ret = march(ro, rd);

	vec3 norm = GetNormal(ro + ret.x * rd, ret.x * ret.x * 0.002 + 0.0005);

	vec3 col = rainbo(ret.x);

	fragColor.rgb = Lit(col, norm);

	if (ret.x != -1)
		fragColor.a = 1;
}
