float rand(vec2 co)
{
	return fract(sin(dot(co ,vec2(12.9898,78.233))) * 43758.5453);
}

void mainImage(vec2 coord)
{
	vec2 uv = coord / iResolution;

	fragColor = vec4(rand(uv * iGlobalTime));
}
