#version 410 core

uniform vec2 screenSize;
uniform float scale;

#define MAX_DEGREE 2
#define MAX_ITERATIONS 10

/*uniform*/ int degree;
/*uniform*/ vec2 polynomial[MAX_DEGREE + 1];
/*uniform*/ vec2 derivative[MAX_DEGREE];
/*uniform*/ vec2 roots[MAX_DEGREE];

out vec4 colorFrag;

vec2 cmult(vec2 a, vec2 b)
{
  if (length(b) < 0.0001) return vec2(0, 0);
  float x = a.x * b.x - a.y * b.y;
  float y = a.x * b.y + a.y * b.x;
  return vec2(x, y);
}

vec2 cdiv(vec2 a, vec2 b)
{
  // bad things if length(b) is close to 0
  vec2 conj = vec2(b.x, -b.y);
  return cmult(a, conj) / conj.x;
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

int checkAgainstRoots(vec2 v) {
  for (int i = 0; i < degree; i++) {
    if (length(v - roots[i]) < 0.001) {
      return i;
    }
  }
  return -1;
}

int attractor(vec2 start)
{
  int i = 0;
  int v;
  vec2 currentPosition = start;
  while (i < MAX_ITERATIONS)
  {
    // update currentPosition
    vec2 y = evalP(currentPosition);
    vec2 yp = evalD(currentPosition);
    vec2 x1 = currentPosition - cdiv(y, yp);
    v = checkAgainstRoots(currentPosition);
    if (v != -1) return v;
  }
}

void main()
{

  // polynomial: x^2 - 1 = 0
  // roots: (1, 0), (-1, 0)
  degree = 2;
  polynomial[0] = vec2(-1, 0);
  polynomial[1] = vec2(0, 0);
  polynomial[2] = vec2(1, 0);
  derivative[0] = vec2(0, 0);
  derivative[1] = vec2(2, 0);
  roots[0] = vec2(1, 0);
  roots[0] = vec2(-1, 0);
  
  float x = 2 * scale * (float(gl_FragCoord.x / screenSize.x) - 0.5f);
  float y = 2 * scale * (float(gl_FragCoord.y / screenSize.y) - 0.5f);
  
  // okay, so now we have a complex number x + iy
  // int v = attractor(vec2(x, y));
  vec2 e = evalP(vec2(x, y));
  colorFrag = vec4(abs(e), 0.5, 1);
  if (abs(length(e) - 1) < 0.005) colorFrag = vec4(1);
}