void		mainImage(vec2 coord)
{
	vec2 uv = coord / iResolution;
	uv.x *= iResolution.y / iResolution.x;
	vec3 col = vec3(0, 0, 1);

	uv += iGlobalTime;

	col = vec3(.5 + .5 * sin(uv), 1);

	fragColor = vec4(col, 1);
}
