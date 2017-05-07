void		mainImage(vec2 fragCoord)
{
	fragColor = vec4(fragCoord.x / 800, fragCoord.y / 800, fragCoord.x / 800, 1) * sin(iGlobalTime) * .5 + .5;
}
