#pragma iChannel0 shaders/test/renderbuffer.glsl
#pragma iChannel1 shaders/test/renderbuffer.glsl

void		mainImage(vec2 fragCoord)
{
	vec2 uv = fragCoord / iResolution;

	fragColor = vec4(texture(iChannel0, uv).xyz, 1);
}
