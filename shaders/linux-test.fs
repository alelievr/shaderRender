void	mainImage(vec2 coord)
{
	vec2 uv = (coord / iResolution) * 2. - 1.;

	if (iResolution.x > 100)
		fragColor = vec4(1, 1, 1, 1);
	else
		fragColor = vec4(0, 0, 0, 1);
}
