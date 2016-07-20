
#define NUM_CELLS 100.0


float r(float n)
{
	return fract(cos(n*89.42)*343.42);
}

vec2 r(vec2 n)
{
	return vec2(r(n.x*23.62-300.0+n.y*34.35),r(n.x*45.13+256.0+n.y*38.89)); 
}

vec3 r(vec3 n)
{
	return vec3(r(n.x*23.62-300.0+n.y*34.35+n.z*63.48),r(n.x*45.13+256.0+n.y*38.89+n.z*-19.64),r(n.x*78.0+213.98+n.y*42.51+n.z*172.0)); 
}

float worley(vec3 n, float s) //s -> scale
{
	float dis = 2.0;
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			for(int z = -1; z <= 1; z++)
			{
				vec3 p = floor(n / s) + vec3(x, y, z);
				vec3 f = fract(n / s);
				vec3 center = r(p) + vec3(x, y, z) - f;
				float d = length(center) * 3;
				if (d < dis)
					dis = d;
			}
		}
	}
	return dis;
}

void mainImage( in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    uv.y *= iResolution.y / iResolution.x;
	uv *= 10;
	float w = worley(vec3(uv.x, uv.y, 0) +
			vec3(iGlobalTime / 2, iGlobalTime / 2, 0), 1);
	fragColor = vec4(w, w, w, 1);
}
