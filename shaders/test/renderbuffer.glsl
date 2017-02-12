void		mainImage(vec2 fragCoord)
{
	fragColor = vec4(fragCoord.x / iResolution.x, fragCoord.y / iResolution.y, fragCoord.x / iResolution.y, 1) * sin(iGlobalTime) * .5 + .5;
}
