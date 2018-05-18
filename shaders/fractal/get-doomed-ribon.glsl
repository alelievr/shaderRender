//#pragma iChannel0 shaders/anotherWorld.glsl
//#define GREYSCALE
//#define MORE_SYMMETRY

#define MAGIC(x,y)	fract(dot(x*2., y*2.)*1.)
#define ITER		18
#define TIMESCALE	0.5

float	contrast = 8;
float	light = 1;

void mainImage( in vec2 fg )
{
	vec2 uv = (fg / iResolution.xy) * 2 - 1;

	uv.x *= iResolution.x / iResolution.y;

	float gTime = iGlobalTime * TIMESCALE + 100;
	float f = 0., g = 0., h = 0.;
	vec2 res = iResolution.xy;
	vec2 mou;
	float demoniColor = sin(gTime * 0.0823456) / 2 + .5;
	float dist = cos(gTime * 0.01) / 4 + .65;
	mou.x = mix(-1.1, 0.2, sin(gTime * 0.142) / 2 + .5) * dist;
#ifdef MORE_SYMMETRY
	mou.y = (cos(gTime * 0.08) / 2 + .5) * 2 * dist;
#else
	mou.y = clamp(cos(gTime / 200) / 1.5 + .25, 0, 1) * dist * (1 / clamp(mou.x, 0.4, 1));
#endif
	mou = (mou+1.0) * res;
	vec2 z = -uv * 2;
	vec2 p = ((-res+2.0+mou) / res.y);
	//  z.x = abs(z.x);
	for( int i = 0; i < ITER; i++)
	{
	    float d = dot(z,z);
	    z = (vec2( z.x, -z.y ) / d) + p;
	    z.x = abs(z.x);
#ifdef MORE_SYMMETRY
		z.y = abs(z.y);
#endif
	    f = max(f, MAGIC(z-p,z-p));
	    g = max(g, MAGIC(z+p,z-p));
	    h = max(h, MAGIC(p-z,p+z));
	}
	vec3 col = ((vec3(f, g, h) - (1-(1/contrast)))*contrast);
	col = abs(col-demoniColor);
#ifdef GREYSCALE
	col = vec3((col.x + col.y + col.z) / 3);
#endif
	col *= light;
	fragColor = vec4(col, 1);
}

