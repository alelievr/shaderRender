#pragma iChannel0 textures/fire.jpg
#pragma iChannel1 textures/fire.jpg

void	mainImage(vec2 coord)
{
	vec2	uv = coord / iResolution;

	uv.x *= iResolution.x / iResolution.y;

	fragColor = vec4(texture(iChannel0, uv).xyz, 1);
}
