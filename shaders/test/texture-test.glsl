#pragma iChannel0 textures/kifs0.png
void		mainImage(vec2 fragCoord)
{
	fragColor = texture(iChannel0, fragCoord / iResolution);
}
