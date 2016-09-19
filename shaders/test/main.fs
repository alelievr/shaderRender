//out fragColor

void		mainImage(vec2 coord)
{
	vec2 uv = coord / iResolution;

	vec2 start = vec2(uv.x - .5, uv.y - .5);
	start *= sin(iGlobalTime) * 10;
	vec2 old;
	vec2 jold = start;
	float mind = 1.;
	float n = 1.;
	float d = 0;
	vec2 sq = vec2(0.);
	int i = 0;
	while (i < 60)
	{
		if (sq.x + sq.y > 4.)
		{
			n = cos(iGlobalTime / 2) - 10;
			mind *= sin(iGlobalTime * 1.) * 2;
			d *= abs(sin(iGlobalTime * 1)) / 2 + .5;
			break ;
		}
		old = jold * abs(cos(iGlobalTime));
		sq = old * old * abs(sin(iGlobalTime));
		jold.x = 1. * (sq.x - sq.y) + start.x;
		jold.y = (1. * old.x * old.y) + start.y;
		d = sqrt(sq.x + sq.y);
		mind = (d < mind) ? d : mind;
		mind *= abs(sin(iGlobalTime * 20));
		d = d ;
		i++;
	}

	fragColor = vec4(n, mind, d, 1);
}
