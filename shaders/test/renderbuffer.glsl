void		mainImage(vec2 fragCoord)
{
	fragColor = vec4(fragCoord.x / iResolution.x, fragCoord.y / iResolution.y, sin(iGlobalTime) / 2 + .5, 1);
}
