void		mainImage(vec2 coord)
{
	vec2 uv = coord / iResolution;
	vec2 q = uv - vec2(.3, .5);
	vec3 col = mix(vec3(0, .8, .6), vec3(.8, .6, 0), length(q));

	if (q.y < 0)
	{
		float r2 = 0.0;
		r2 += 0.115 * sin(120*q.y) + 0.3 * cos(28*q.x);
		col *= smoothstep(r2, r2 + 0.005, abs(q.x + sin(q.y) * 0.01));
	}

	uv.x *= iResolution.x / iResolution.y;
	fragColor = vec4(col, 1);
}
