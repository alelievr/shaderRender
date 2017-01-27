float map(vec3 p)
{
	vec3 q = fract(p) * 2 - 1;

	return length(q) - sin(iGlobalTime) * .2 - .2;
}

float trace(vec3 o, vec3 r)
{
	float t = 0;

	for (int i = 0; i < 200; i++)
	{
		vec3 p = o + r * t;
		float d = map(p);
		t += d * 0.5;
	}
	return t;
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
	float t = trace(o, r);
	//float fog = 1 / (1 + t * t * 0.1);

	vec3 col = vec3(t / 50);

	fragColor = vec4(col, 1);
}
