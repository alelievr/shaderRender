#define REFLECTIONS 10

#define FOG_STRENGTH 1.0
#define REFLECTION_STRENGTH 100000.0
#define COLOR_STRENGTH 15.0
#define BRIGHTNESS 15.0

struct ray {
	vec3 p;
	vec3 d;
};

struct material {
	vec3 color;
	vec3 emmision;
	float diffusion;
};

struct hit {
	vec3 p;
	vec3 n;
	float t;
	material m;
};

void plane (vec3 v, float f, ray r, material m, inout hit h) {
	float t = (f - dot (v, r.p)) / dot (v, r.d);
	
	if (t < 0.0)
		return;
	
	if (t < h.t) {
		h.t = t;
		h.p = r.p + r.d * t;
		h.n = /*normalize*/ (faceforward (v, v, r.d));
		
		h.m = m;
	}
}

void sphere (vec3 v, float f, ray r, material m, inout hit h) {
	vec3 d = r.p - v;
	
	float a = dot (r.d, r.d);
	float b = dot (r.d, d);
	float c = dot (d, d) - f * f;
	
	float g = b*b - a*c;
	
	if (g < 0.0)
		return;
	
	float t = (-sqrt (g) - b) / a;
	
	if (t < 0.0)
		return;
	
	if (t < h.t) {
		h.t = t;
		h.p = r.p + r.d * t;
		h.n = (h.p - v) / f * (sin(iGlobalTime) / 2 - .6);
		
		h.m = m;
	}
}

hit scene (ray r) {
	hit h;
	h.t = 1e20;
	
	material m0 = material (vec3 (1), vec3 (0), 0.5);
	
	plane	(vec3 ( 1.0, 0.1, 0.0), 3.0, r, m0, h);
	plane	(vec3 ( 1.0, 0.4, 0.0),-3.0, r, m0, h);
	plane	(vec3 ( 0.0, 1.0, 0.0), 0.0, r, m0, h);
	plane	(vec3 ( 0.0, 1.0, 0.0), 5.0, r, m0, h);
	plane	(vec3 ( 0.0, 0.0, 1.0), 5.0, r, m0, h);
	plane	(vec3 ( 1.0, 0.0, 1.0),-6.0, r, m0, h);
	sphere	(vec3 (-1.0, 3.0, 0), 1.0, r, m0, h);
	sphere	(vec3 ( 1.0, 1.0, 2.0), 1.0, r, m0, h);
	
	h.m.color *= h.n * h.n;
	
	return h;
}

ray getRay (in vec3 origin, in vec3 forw, in vec2 uv) {
	ray r;
	
	r.p = origin;
	float fov = 1.5;
	vec3 right = normalize(cross(forw, vec3(0, 1, 0)));
	vec3 up = normalize(cross(right, forw));
	r.d = normalize(uv.x * right + uv.y * up + fov * forw);

	return r;
}

vec3 surface (ray r) {
	vec3 color = vec3 (0);
	
	float depth = 0.0;
	
	float l = 0.0;
	
	for (int i = 0; i < REFLECTIONS; i++) {
		hit h = scene (r);
		
		float c = dot (h.n, r.d);
		depth += (1.0 / REFLECTION_STRENGTH) + h.t * FOG_STRENGTH;
		
		r = ray (h.p + h.n * 0.0001, reflect (normalize (r.d), h.n));
		
		float d = 1.0 / (depth * depth);
		color = (color + c*c*d) * (1.0 - h.m.color * d * COLOR_STRENGTH);
	}
	
	return color * BRIGHTNESS;
}

void mainImage( in vec2 fragCoord ) {
	vec3 camera = iMoveAmount.xyz * 2. + vec3(0, 2, 0);
	vec3 forward = iForward;
	
	vec2 uv = (fragCoord.xy * 2.0 - iResolution.xy) / iResolution.xx;
	ray r = getRay (camera, forward, uv);
	
	fragColor = vec4 (surface (r) * (1.3 - max (abs (uv.x), abs (uv.y * 1.5))), 1.0);
}

