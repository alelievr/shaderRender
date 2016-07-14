void mainImage( vec2 fragCoord)
{
	fragColor = vec4(1, 1, 1, 1);
	if (fragCoord.x < iMoveAmount.x)
		fragColor = vec4(1, 0, 0, 1);
	if (fragCoord.y < iMoveAmount.y)
		fragColor = vec4(0, 0, 1, 1);
}
