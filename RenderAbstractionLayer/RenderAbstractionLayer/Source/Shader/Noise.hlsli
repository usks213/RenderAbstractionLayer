// ノイズシェーダー

float random(float2 p)
{
	return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float noise(float2 st)
{
	float2 p = floor(st);
	return random(p);
}

float valueNoise(float2 st)
{
	float2 p = floor(st);
	float2 f = frac(st);

	float v00 = random(p + float2(0, 0));
	float v10 = random(p + float2(1, 0));
	float v01 = random(p + float2(0, 1));
	float v11 = random(p + float2(1, 1));
            
	float2 u = f * f * (3.0 - 2.0 * f);

	float v0010 = lerp(v00, v10, u.x);
	float v0111 = lerp(v01, v11, u.x);
	return lerp(v0010, v0111, u.y);
}

float2 random2(float2 st)
{
	st = float2(dot(st, float2(127.1, 311.7)),
                           dot(st, float2(269.5, 183.3)));
	return -1.0 + 2.0 * frac(sin(st) * 43758.5453123);
}

float perlinNoise(float2 st)
{
	float2 p = floor(st);
	float2 f = frac(st);
	float2 u = f * f * (3.0 - 2.0 * f);

	float v00 = random2(p + float2(0, 0));
	float v10 = random2(p + float2(1, 0));
	float v01 = random2(p + float2(0, 1));
	float v11 = random2(p + float2(1, 1));

	return lerp(lerp(dot(v00, f - float2(0, 0)), dot(v10, f - float2(1, 0)), u.x),
                         lerp(dot(v01, f - float2(0, 1)), dot(v11, f - float2(1, 1)), u.x),
                         u.y) + 0.5f;
}

float fBm(float2 st)
{
	float f = 0;
	float2 q = st;

	f += 0.5000 * perlinNoise(q);
	q = q * 2.01;
	f += 0.2500 * perlinNoise(q);
	q = q * 2.02;
	f += 0.1250 * perlinNoise(q);
	q = q * 2.03;
	f += 0.0625 * perlinNoise(q);
	q = q * 2.01;

	return f;
}