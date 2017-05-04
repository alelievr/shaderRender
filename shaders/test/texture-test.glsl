#pragma iChannel0 shaders/test/renderbuffer.glsl

void		mainImage(vec2 fragCoord)
{
	float rate = fragCoord.x / iResolution.y;

	fragColor = texture(iChannel0, fragCoord / 100);

/*	if (rate > 0 && rate < .1)
		fragColor = texture(iChannel0, fragCoord / iResolution);
	else if (rate > .1 && rate < .2)
		fragColor = texture(iChannel1, fragCoord / iResolution);
	else if (rate > .2 && rate < .3)
		fragColor = texture(iChannel2, fragCoord / iResolution);
	else if (rate > .3 && rate < .4)
		fragColor = texture(iChannel3, fragCoord / iResolution);
	else if (rate > .4 && rate < .5)
		fragColor = texture(iChannel4, fragCoord / iResolution);
	else if (rate > .5 && rate < .6)
		fragColor = texture(iChannel5, fragCoord / iResolution);
	else if (rate > .6 && rate < .7)
		fragColor = texture(iChannel6, fragCoord / iResolution);
	else if (rate > .7 && rate < .8)
		fragColor = texture(iChannel7, fragCoord / iResolution);*/
}
