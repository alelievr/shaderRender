void	mainImage(vec2 coord)
{
	vec3 col = vec3(0, abs(sin(iGlobalTime)) / 4 + .75,  abs(cos(iGlobalTime)) / 2 + .25);
	fragColor = vec4(col, 1);
}
