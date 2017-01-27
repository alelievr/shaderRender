void		mainImage(vec2 coord)
{
	vec2 uv = coord / iResolution;
	vec2 q = uv - vec2(.3, .5);
	vec3 col = mix(vec3(0, .8, .6), vec3(.8, .6, 0), length(q));

	float r = .2 + 0.1 * cos(atan(q.x, q.y) * 10 - q.x * 20 + 4 + iGlobalTime * 2);
	col *= smoothstep(r, r + 0.004, length(q));

	if (q.y < 0)
	{
		float r2 = 0.015;
		r2 += 0.002 * sin(120*q.y);
		r2 += exp(-80 * uv.y);
		col *= smoothstep(r2, r2 + 0.005, abs(q.x + sin(2 * q.y) * 0.2));
	}

	uv.x *= iResolution.x / iResolution.y;
	col = mix(col, vec3(1, 1, .5), 1 - smoothstep(.1, .12, length(uv - vec2(1.2, .8))));
	float f = 1 - smoothstep(0, .3, length(uv - vec2(1.2, .8)));
	col += f;

	fragColor = vec4(col, 1);
}
