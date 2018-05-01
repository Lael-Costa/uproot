#version 410 core

uniform vec2 screenSize;
uniform vec2 pos;
uniform float scale;

#define MAX_DEGREE 10
#define MAX_ITERATIONS 50

uniform int degree;
uniform vec2 polynomial[MAX_DEGREE + 1];
uniform vec2 derivative[MAX_DEGREE];
uniform vec2 roots[MAX_DEGREE];

out vec4 colorFrag;

vec2 cmult(vec2 a, vec2 b)
{
  float x = a.x * b.x - a.y * b.y;
  float y = a.x * b.y + a.y * b.x;
  return vec2(x, y);
}

vec2 cdiv(vec2 a, vec2 b)
{
  // bad things if length(b) is close to 0
  if (length(b) < 0.000001) return vec2(10000);
  vec2 conj = vec2(b.x, -b.y);
  return cmult(a, conj) / length(b);
}

vec2 evalP(vec2 x)
{
  vec2 s = vec2(0);
  vec2 p = vec2(1);
  for (int i = 0; i <= degree; i++)
  {
    s += cmult(p, polynomial[i]);
    p = cmult(p, x);
  }
  return s;
}

vec2 evalD(vec2 x)
{
  vec2 s = vec2(0);
  vec2 p = vec2(1);
  for (int i = 0; i < degree; i++)
  {
    s += cmult(p, derivative[i]);
    p = cmult(p, x);
  }
  return s;
}

int checkAgainstRoots(vec2 v, int iter) {
  int bestI = -1;
  float bestL = 0.01 * pow(1.5, iter);
  for (int i = 0; i < degree; i++) {
    if (length(v - roots[i]) < bestL) {
      bestI = i;
      bestL = length(v - roots[i]);
    }
  }
  return bestI;
}

// vec2(root, iterations)
ivec2 attractor(vec2 start)
{
  int i = 0;
  int v;
  vec2 x0 = start;
  while (i < MAX_ITERATIONS)
  {
    // update currentPosition
    v = checkAgainstRoots(x0, i + 1);
    if (v != -1) return ivec2(v, i);

    vec2 y = evalP(x0);
    vec2 yp = evalD(x0);

    x0 = x0 - cdiv(y, yp);
    i++;
  }
  return ivec2(-1);
}

void main()
{
  // polynomial: x^5 - 1
  vec3 colors[5];
  colors[0] = vec3(1, 0, 0);
  colors[1] = vec3(1, 1, 0);
  colors[2] = vec3(1, 0, 1);
  colors[3] = vec3(0, 1, 1);
  colors[4] = vec3(0, 0, 1);

  degree = 5;
  polynomial[0] = vec2(-1, 0);
  polynomial[1] = vec2(0, 0);
  polynomial[2] = vec2(0, 0);
  polynomial[3] = vec2(0, 0);
  polynomial[4] = vec2(0, 0);
  polynomial[5] = vec2(1, 0);

  derivative[0] = vec2(0, 0);
  derivative[1] = vec2(0, 0);
  derivative[2] = vec2(0, 0);
  derivative[3] = vec2(0, 0);
  derivative[4] = vec2(5, 0);

  roots[0] = vec2(1, 0);
  roots[1] = vec2(-0.80902, -0.58779);
  roots[2] = vec2(0.30902, 0.95106);
  roots[3] = vec2(0.30902, -0.95106);
  roots[4] = vec2(-0.80902, 0.58779);

  float screenDim = max(screenSize.x, screenSize.y);
  float xMax = float((gl_FragCoord.x - screenSize.x / 2) / screenDim);
  float yMax = float((gl_FragCoord.y - screenSize.y / 2) / screenDim);

  float x = scale * xMax + pos.x;
  float y = scale * yMax + pos.y;
  colorFrag = vec4(abs(x), abs(y), abs(x - y), 1);
  // okay, so now we have a complex number x + iy
  ivec2 basin = attractor(vec2(x, y));

  if (basin.x > -1) {
    colorFrag = clamp(vec4(colors[int(basin.x)], 1), 0, 1);
  } else {
    colorFrag = vec4(0);
  }
  colorFrag = clamp(colorFrag, 0, 1);
}
