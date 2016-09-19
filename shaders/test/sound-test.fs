void		mainImage(vec2 coord)
{
	fragColor = texture(iSoundChannel0, coord / iResolution);
}
