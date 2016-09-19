void	mainImage(vec2 fragCoord)
{
	fragColor = vec4(1, 0, 0, 1);
	if (fragCoord.x > iMouse.x)
		fragColor = vec4(0, 1, 0, 0);
	if (fragCoord.y > iMouse.y)
		fragColor = vec4(0, 0, 1, 0);
}	
