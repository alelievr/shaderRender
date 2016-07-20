/*
  Written by Alan Wolfe
  http://demofox.org/
  http://blog.demofox.org/
*/

/*
FUTURE SHADERS TODO:
* move the camera over time... maybe flying through some stuff.
* light absorbance in transparent objects
* fog (volumetric and distance fog)

*/

//=======================================================================================

#define FLT_MAX 3.402823466e+38

//=======================================================================================
struct SMaterial
{
	float m_diffuse;
	vec3 m_diffuseColor;
	float m_specular;
	vec3 m_specularColor;
	vec3 m_emissiveColor;
	float m_reflection;	
};

//=======================================================================================
struct SCollisionInfo
{
	int			m_Id;
	bool		m_foundHit;
	bool 		m_fromInside;
	float 		m_collisionTime;
	vec3		m_intersectionPoint;
	vec3		m_surfaceNormal;
	SMaterial 	m_material;
};

//=======================================================================================
struct SSphere
{
	int			m_Id;
	vec3   		m_center;
	float  		m_radius;
	SMaterial	m_material;
};
	
//=======================================================================================
struct SAxisAlignedBox
{
	int			m_Id;
	vec3		m_pos;
	vec3		m_scale;
	SMaterial	m_material;
};
	
//=======================================================================================
struct SPointLight
{
	vec3		m_pos;
	vec3		m_color;
};
	
//=======================================================================================
struct SDirLight
{
	vec3		m_reverseDir;
	vec3		m_color;
};
	
//=======================================================================================
// Scene parameters
//=======================================================================================
	
//----- settings
#define DO_SHADOWS true // setting this to false will speed up rendering
const int c_maxRayBounces 		= 3;	

//----- camera
vec2 mouse = iMouse.xy / iResolution.xy;

vec3 cameraAt 	= vec3(0.0,0.0,0.0);

float angleX = 3.14 + 6.28 * mouse.x;
float angleY = (mouse.y * 3.90) - 0.4;
vec3 cameraPos	= (vec3(sin(angleX)*cos(angleY), sin(angleY), cos(angleX)*cos(angleY))) * 4.0;

vec3 cameraFwd  = normalize(cameraAt - cameraPos);
vec3 cameraLeft  = normalize(cross(normalize(cameraAt - cameraPos), vec3(0.0,sign(cos(angleY)),0.0)));
vec3 cameraUp   = normalize(cross(cameraLeft, cameraFwd));

float cameraViewWidth	= 6.0;
float cameraViewHeight	= cameraViewWidth * iResolution.y / iResolution.x;
float cameraDistance	= 6.0;  // intuitively backwards!
	
//----- lights
vec3 lightAmbient				= vec3(0.1,0.1,0.1);

SDirLight lightDir1 =
	SDirLight
	(
		normalize(vec3(-1.0,1.0,-1.0)),
		vec3(0.8,0.8,0.8)
	);

SPointLight lightPoint1 =
	SPointLight
	(
		vec3(sin(iGlobalTime*1.1),0.0,cos(iGlobalTime*1.1)),
		vec3(0.5,0.5,1.0)                                            
	);

SPointLight lightPoint2 =
	SPointLight
	(
		vec3(sin(iGlobalTime*1.9), cos(iGlobalTime*1.9), 0.0),
		vec3(0.5,1.0,0.5)                                             
	);

//----- primitives
SSphere lightPoint1Sphere =
	SSphere
	(
		1,						//id
		lightPoint1.m_pos,		//center
		0.03,					//radius
		SMaterial
		(
			1.0,				//diffuse amount
			vec3(0.0,0.0,0.0),	//diffuse color
			1.0,				//specular amount
			vec3(0.0,0.0,0.0),	//specular color
			lightPoint1.m_color,//emissive color
			0.0					//reflection amount
		)
	);	

SSphere lightPoint2Sphere =
	SSphere
	(
		2,						//id
		lightPoint2.m_pos,		//center
		0.03,					//radius
		SMaterial
		(
			1.0,				//diffuse amount
			vec3(0.0,0.0,0.0),	//diffuse color
			1.0,				//specular amount
			vec3(0.0,0.0,0.0),	//specular color
			lightPoint2.m_color,//emissive color
			0.0					//reflection amount			
		)
	);	

SSphere sphere1 =
	SSphere
	(
		4,						//id
		vec3(0.0,0.0,0.0),		//center
		0.3,					//radius
		SMaterial
		(
			1.0,				//diffuse amount
			vec3(0.05,0.1,0.05),//diffuse color
			80.0,				//specular amount
			vec3(1.0,1.0,1.0),	//specular color
			vec3(0.0,0.0,0.0),	//emissive color
			1.0					//reflection amount			
		)
	);

SSphere sphere2 =
	SSphere
	(
		5,						//id
		vec3(sin(iGlobalTime*1.3),-0.2,cos(iGlobalTime*1.3)),	//center
		0.15,					//radius
		SMaterial
		(
			1.0,				//diffuse amount
			vec3((sin(iGlobalTime*0.25)+1.0)*0.5,0.2,0.3),	//diffuse color
			40.0,				//specular amount
			vec3(1.0,1.0,1.0),	//specular color
			vec3(0.0,0.0,0.0),	//emissive
			1.0					//reflection amount			
		)
	);

SSphere sphere3 =
	SSphere
	(
		6,						//id
		vec3(sin(iGlobalTime*1.4)*2.0 + 0.5,-0.6,3.5),		//center
		0.5,					//radius
		SMaterial
		(
			0.25,				//diffuse amount
			vec3(1.0,1.0,1.0),	//diffuse color
			3.0,				//specular amount
			vec3(0.8,0.4,0.2),	//specular color
			vec3(0.0,0.0,0.0),	//emissive
			1.0					//reflection amount			
		)
	);

SSphere sphere4 =
	SSphere
	(
		7,						//id
		vec3(-1.3,sin(iGlobalTime*2.0), 2.5),//center
		0.6,					//radius
		SMaterial
		(
			1.0,				//diffuse amount
			vec3(0.4,0.2,0.8),  //diffuse color
			20.0,				//specular amount
			vec3(1.0,1.0,0.0),	//specular color
			vec3(0.0,0.0,0.0),	//emissive
			1.0					//reflection amount			
		)
	);

SAxisAlignedBox floorBox = 
	SAxisAlignedBox
	(
		8,						//Id
		vec3(0.0,-1.6,0.0),		//center
		vec3(10.0,0.1,10.0),	//scale
		SMaterial
		(
			0.5,				//diffuse amount
			vec3(0.15,0.15,0.15),//diffuse color
			20.0,				//specular amount
			vec3(0.0,0.0,0.0),	//specular color
			vec3(0.0,0.0,0.0),	//emissive
			0.2					//reflection amount			
		)
	);

SAxisAlignedBox backBox1 = 
	SAxisAlignedBox
	(
		9,						//Id
		vec3(0.0,0.0,5.0),		//center
		vec3(10.0,2.0,0.1),		//scale
		SMaterial
		(
			0.5,				//diffuse amount
			vec3(0.15,0.15,0.15),//diffuse color
			20.0,				//specular amount
			vec3(0.0,0.0,0.0),	//specular color
			vec3(0.0,0.0,0.0),	//emissive
			0.2					//reflection amount			
		)
	);

//----- macro lists

// sphere primitive list
#define SPHEREPRIMLIST PRIM(sphere1) PRIM(sphere2) PRIM(sphere3) PRIM(sphere4)

// sphere primitive list with light primitives
#define SPHEREPRIMLISTWITHLIGHTS SPHEREPRIMLIST PRIM(lightPoint1Sphere) PRIM(lightPoint2Sphere)

// box primitive list
#define BOXPRIMLIST PRIM(floorBox) PRIM(backBox1)

// point light list
#define POINTLIGHTLIST LIGHT(lightPoint1) LIGHT(lightPoint2)

// directional light list
#define DIRLIGHTLIST LIGHT(lightDir1)

//=======================================================================================
bool RayIntersectSphere (inout SSphere sphere, inout SCollisionInfo info, in vec3 rayPos, in vec3 rayDir, in int ignorePrimitiveId)
{
	if (ignorePrimitiveId == sphere.m_Id)
		return false;

	//get the vector from the center of this circle to where the ray begins.
	vec3 m = rayPos - sphere.m_center;

    //get the dot product of the above vector and the ray's vector
	float b = dot(m, rayDir);

	float c = dot(m, m) - sphere.m_radius * sphere.m_radius;

	//exit if r's origin outside s (c > 0) and r pointing away from s (b > 0)
	if(c > 0.0 && b > 0.0)
		return false;

	//calculate discriminant
	float discr = b * b - c;


	//a negative discriminant corresponds to ray missing sphere
	if(discr < 0.0)
		return false;

	//not inside til proven otherwise
	bool fromInside = false;

	//ray now found to intersect sphere, compute smallest t value of intersection
	float collisionTime = -b - sqrt(discr);

	//if t is negative, ray started inside sphere so clamp t to zero and remember that we hit from the inside
	if(collisionTime < 0.0)
	{
		collisionTime = -b + sqrt(discr);
		fromInside = true;
	}

	//enforce a max distance if we should
	if(info.m_collisionTime >= 0.0 && collisionTime > info.m_collisionTime)
		return false;

	// set all the info params since we are garaunteed a hit at this point
	info.m_fromInside = fromInside;
	info.m_collisionTime = collisionTime;
	info.m_material = sphere.m_material;

	//compute the point of intersection
	info.m_intersectionPoint = rayPos + rayDir * info.m_collisionTime;

	// calculate the normal
	info.m_surfaceNormal = info.m_intersectionPoint - sphere.m_center;
	info.m_surfaceNormal = normalize(info.m_surfaceNormal);

	// we found a hit!
	info.m_foundHit = true;
	info.m_Id = sphere.m_Id;
	return true;
}

//=======================================================================================
bool RayIntersectAABox (inout SAxisAlignedBox box, inout SCollisionInfo info, in vec3 rayPos, in vec3 rayDir, in int ignorePrimitiveId)
{
	if (ignorePrimitiveId == box.m_Id)
		return false;
	
	float rayMinTime = 0.0;
	float rayMaxTime = FLT_MAX;
	
	//enforce a max distance
	if(info.m_collisionTime >= 0.0)
	{
		rayMaxTime = info.m_collisionTime;
	}	
	
	// find the intersection of the intersection times of each axis to see if / where the
	// ray hits.
	for(int axis = 0; axis < 3; ++axis)
	{
		//calculate the min and max of the box on this axis
		float axisMin = box.m_pos[axis] - box.m_scale[axis] * 0.5;
		float axisMax = axisMin + box.m_scale[axis];

		//if the ray is paralel with this axis
		if(abs(rayDir[axis]) < 0.0001)
		{
			//if the ray isn't in the box, bail out we know there's no intersection
			if(rayPos[axis] < axisMin || rayPos[axis] > axisMax)
				return false;
		}
		else
		{
			//figure out the intersection times of the ray with the 2 values of this axis
			float axisMinTime = (axisMin - rayPos[axis]) / rayDir[axis];
			float axisMaxTime = (axisMax - rayPos[axis]) / rayDir[axis];

			//make sure min < max
			if(axisMinTime > axisMaxTime)
			{
				float temp = axisMinTime;
				axisMinTime = axisMaxTime;
				axisMaxTime = temp;
			}

			//union this time slice with our running total time slice
			if(axisMinTime > rayMinTime)
				rayMinTime = axisMinTime;

			if(axisMaxTime < rayMaxTime)
				rayMaxTime = axisMaxTime;

			//if our time slice shrinks to below zero of a time window, we don't intersect
			if(rayMinTime > rayMaxTime)
				return false;
		}
	}
	
	//if we got here, we do intersect, return our collision info
	info.m_fromInside = (rayMinTime == 0.0);
	if(info.m_fromInside)
		info.m_collisionTime = rayMaxTime;
	else
		info.m_collisionTime = rayMinTime;
	info.m_material = box.m_material;
	
	info.m_intersectionPoint = rayPos + rayDir * info.m_collisionTime;

	// figure out the surface normal by figuring out which axis we are closest to
	float closestDist = FLT_MAX;
	for(int axis = 0; axis < 3; ++axis)
	{
		float distFromPos= abs(box.m_pos[axis] - info.m_intersectionPoint[axis]);
		float distFromEdge = abs(distFromPos - (box.m_scale[axis] * 0.5));

		if(distFromEdge < closestDist)
		{
			closestDist = distFromEdge;
			info.m_surfaceNormal = vec3(0.0,0.0,0.0);
			if(info.m_intersectionPoint[axis] < box.m_pos[axis])
				info.m_surfaceNormal[axis] = -1.0;
			else
				info.m_surfaceNormal[axis] =  1.0;
		}
	}

	// we found a hit!
	info.m_foundHit = true;
	info.m_Id = box.m_Id;
	return true;	
}

//=======================================================================================
bool PointCanSeePoint(in vec3 startPos, in vec3 targetPos, in int ignorePrimitiveId)
{
	// see if we can hit the target point from the starting point
	SCollisionInfo collisionInfo =
		SCollisionInfo
		(
			0,
			false,
			false,
			-1.0,
			vec3(0.0,0.0,0.0),
			vec3(0.0,0.0,0.0),
			SMaterial(
				1.0,
				vec3(0.0,0.0,0.0),
				1.0,
				vec3(0.0,0.0,0.0),
				vec3(0.0,0.0,0.0),
				0.0
			)
		);	
	
	vec3 rayDir = targetPos - startPos;
	collisionInfo.m_collisionTime = length(rayDir);
	rayDir = normalize(rayDir);

	// run intersection against all non light primitives. return false on first hit found
	return true
	#define PRIM(x) && !RayIntersectSphere(x, collisionInfo, startPos, rayDir, ignorePrimitiveId)
	SPHEREPRIMLIST
	#undef PRIM
	#define PRIM(x) && !RayIntersectAABox(x, collisionInfo, startPos, rayDir, ignorePrimitiveId)
	BOXPRIMLIST
	#undef PRIM
	;
}

//=======================================================================================
void ApplyPointLight (inout vec3 pixelColor, in SCollisionInfo collisionInfo, in SPointLight light, in float reflectionAmount, in vec3 rayDir)
{
	if (DO_SHADOWS == false || PointCanSeePoint(collisionInfo.m_intersectionPoint, light.m_pos, collisionInfo.m_Id))
	{
		// diffuse
		vec3 hitToLight = normalize(light.m_pos - collisionInfo.m_intersectionPoint);
		float dp = dot(collisionInfo.m_surfaceNormal, hitToLight);
		if(dp > 0.0)
			pixelColor += collisionInfo.m_material.m_diffuseColor * dp * light.m_color * collisionInfo.m_material.m_diffuse * reflectionAmount;
				
		// specular
		vec3 reflection = reflect(hitToLight, collisionInfo.m_surfaceNormal);
		dp = dot(rayDir, reflection);
		if (dp > 0.0)
			pixelColor += collisionInfo.m_material.m_specularColor * pow(dp, collisionInfo.m_material.m_specular) * light.m_color * reflectionAmount;
	}
}

//=======================================================================================
void ApplyDirLight (inout vec3 pixelColor, in SCollisionInfo collisionInfo, in SDirLight light, in float reflectionAmount, in vec3 rayDir)
{
	if (DO_SHADOWS == false || PointCanSeePoint(collisionInfo.m_intersectionPoint, collisionInfo.m_intersectionPoint + light.m_reverseDir * 1000.0, collisionInfo.m_Id))
	{
		// diffuse
		float dp = dot(collisionInfo.m_surfaceNormal, light.m_reverseDir);
		if(dp > 0.0)
			pixelColor += collisionInfo.m_material.m_diffuseColor * dp * light.m_color * collisionInfo.m_material.m_diffuse * reflectionAmount;
		
		// specular
		vec3 reflection = reflect(light.m_reverseDir, collisionInfo.m_surfaceNormal);
		dp = dot(rayDir, reflection);
		if (dp > 0.0)
			pixelColor += collisionInfo.m_material.m_specularColor * pow(dp, collisionInfo.m_material.m_specular) * light.m_color * reflectionAmount;			
	}
}

//=======================================================================================
void TraceRay (in vec3 rayPos, in vec3 rayDir, inout vec3 pixelColor, in int depth)
{
	int lastHitPrimitiveId = 0;
	float reflectionAmount = 1.0;
	
	for(int index = 0; index < c_maxRayBounces; ++index)
	{
		
		SCollisionInfo collisionInfo =
			SCollisionInfo
			(
				0,
				false,
				false,
				-1.0,
				vec3(0.0,0.0,0.0),
				vec3(0.0,0.0,0.0),
				SMaterial(
					1.0,
					vec3(0.0,0.0,0.0),
					1.0,
					vec3(0.0,0.0,0.0),
					vec3(0.0,0.0,0.0),
					0.0
				)
			);

		// run intersection against all objects, including light objects		
		#define PRIM(x) RayIntersectSphere(x, collisionInfo, rayPos, rayDir, lastHitPrimitiveId);
		SPHEREPRIMLISTWITHLIGHTS
		#undef PRIM
			
		// run intersections against all boxes
		#define PRIM(x) RayIntersectAABox(x, collisionInfo, rayPos, rayDir, lastHitPrimitiveId);
		BOXPRIMLIST
		#undef PRIM

	
		if (collisionInfo.m_foundHit)
		{	
			// point lights
			#define LIGHT(light) ApplyPointLight(pixelColor, collisionInfo, light, reflectionAmount, rayDir);
			POINTLIGHTLIST
			#undef LIGHT
				
			// directional lights
			#define LIGHT(light) ApplyDirLight(pixelColor, collisionInfo, light, reflectionAmount, rayDir);
			DIRLIGHTLIST				
			#undef LIGHT

			// ambient light
			pixelColor += lightAmbient * collisionInfo.m_material.m_diffuseColor * collisionInfo.m_material.m_diffuse * reflectionAmount;
			
			// emissive color
			pixelColor += collisionInfo.m_material.m_emissiveColor * reflectionAmount;		
			
			//set up our next ray
			rayPos = collisionInfo.m_intersectionPoint;
			rayDir = reflect(rayDir, collisionInfo.m_surfaceNormal);
			
			lastHitPrimitiveId = collisionInfo.m_Id;
			reflectionAmount *= collisionInfo.m_material.m_reflection;
			
			if (reflectionAmount < 0.1)
				return;
		}
		// no hit means bail out
		else
		{
			pixelColor+= vec3(0.1,0.1,0.1) * reflectionAmount;
			return;
		}
	}
			
}

//=======================================================================================
void mainImage( in vec2 fragCoord )
{
	vec2 rawPercent = (fragCoord.xy / iResolution.xy);
	vec2 percent = rawPercent - vec2(0.5,0.5);
	
	vec3 rayPos;
	vec3 rayTarget;
	
	// if the mouse button is down
	if( iMouse.z > 0.0 ) {
		rayTarget = (cameraFwd * cameraDistance)
				  + (cameraLeft * percent.x * cameraViewWidth)
		          + (cameraUp * percent.y * cameraViewHeight);
		
		rayPos = cameraPos;
	}
	// else handle the case of the mouse button not being down
	else {
		rayPos = vec3(0.0,1.0,-4.0);
		vec3 f = normalize(cameraAt - rayPos);
		vec3 l = normalize(cross(f,vec3(0.0,1.0,0.0)));
		vec3 u = normalize(cross(l,f));
		
		rayTarget = (f * cameraDistance)
				  + (l * percent.x * cameraViewWidth)
		          + (u * percent.y * cameraViewHeight);		
	}
	
	vec3 rayDir = normalize(rayTarget);
	
	vec3 pixelColor = vec3(0.0,0.0,0.0);
	TraceRay(rayPos, rayDir, pixelColor, 0);
	fragColor = vec4(pixelColor, 1.0);
}
