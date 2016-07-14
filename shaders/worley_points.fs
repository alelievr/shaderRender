
#define NUM_CELLS 100.0

float rand(vec3 co){
    return fract(sin(dot(co ,vec3(12.9898,78.233,34.925))) * 43758.5453);
}

vec3 get_cell_point(ivec3 cell) {
	vec3 cell_base = vec3(cell) / NUM_CELLS;
	float noise_x = rand(cell.xyz);
    float noise_y = rand(cell.yxz);
    float noise_z = rand(cell.xzy);
    return cell_base + (0.5 + 3.5 * vec3(noise_x, noise_y, noise_z)) / NUM_CELLS;
}

float worley(vec3 coord) {
    ivec3 cell = ivec3(coord * NUM_CELLS);

	vec3 c = get_cell_point(cell);
	if (length(coord.xy - c.xy) <= 0.001)
		return 1;
	return 0;
}


void mainImage( in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    uv.y *= iResolution.y / iResolution.x;
	fragColor = vec4(worley(vec3(uv.x, uv.y, 2) +
				vec3(iGlobalTime / 20, iGlobalTime / 20, 0)));
}
