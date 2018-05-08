#version 410 core

uniform vec2 screenSize;
uniform vec2 pos;
uniform float scale;
uniform float time;

#define MAX_DEGREE 10
#define MAX_ITERATIONS 8
#define M_PI 3.1415926535897932

uniform int degree;
uniform highp vec2 polynomial[MAX_DEGREE + 1];
uniform highp vec2 derivative[MAX_DEGREE];
uniform highp vec2 roots[MAX_DEGREE];

out vec4 colorFrag;

vec2 cmult(vec2 a, vec2 b)
{
  highp float x = a.x * b.x - a.y * b.y;
  highp float y = a.x * b.y + a.y * b.x;
  return vec2(x, y);
}

vec2 cdiv(vec2 a, vec2 b)
{
  // bad things if length(b) is close to 0
  if (length(b) < 0.0000001) return vec2(0);
  highp vec2 conj = vec2(b.x, -b.y);
  return cmult(a, conj) / pow(length(b), 2);
}

float carg(vec2 a)
{
  if (a.x > 0) {
    return atan(a.y / a.x);
  } else if (a.x < 0) {
    if (a.y >= 0) {
      return atan(a.y / a.x) + M_PI;
    } else {
      return atan(a.y / a.x) - M_PI;
    }
  } else {
    if (a.y > 0) {
      return M_PI / 2;
    } else if (a.y < 0) {
      return -M_PI / 2;
    } else {
      return 0;
    }
  }
}

vec2 cpow(vec2 a, float e)
{
  highp float powarg = e * carg(a);
  highp vec2 rot = vec2(cos(powarg), sin(powarg));
  return rot * pow(length(a), e);
}

vec2 cpow(vec2 a, int e)
{
  highp float powarg = e * carg(a);
  highp vec2 rot = vec2(cos(powarg), sin(powarg));
  return rot * pow(length(a), e);
}

vec2 evalP(vec2 x)
{
  highp vec2 s = vec2(0);
  for (int i = 0; i <= degree; i++)
  {
    s += cmult(cpow(x, i), polynomial[i]);
  }
  return s;
}

vec2 evalD(vec2 x)
{
  highp vec2 s = vec2(0);
  for (int i = 0; i < degree; i++)
  {
    s += cmult(cpow(x, i), derivative[i]);
  }
  return s;
}

int checkAgainstRoots(vec2 z, int iter) {
  int bestI = -1;
  highp float bestL = 0.000001;
  for (int i = 0; i < degree; i++) {
    vec2 diff = z - roots[i];
    if (length(diff) < bestL) {
      bestI = i;
      bestL = length(diff);
    }
  }
  return bestI;
}

// vec2(root, iterations)
ivec2 attractor(vec2 start)
{
  int i = 0;
  int v;
  highp vec2 x0 = start;
  while (i < MAX_ITERATIONS)
  {
    // update currentPosition
    v = checkAgainstRoots(x0, i + 1);
    if (v != -1) return ivec2(v, i);

    highp vec2 y = evalP(x0);
    highp vec2 yp = evalD(x0);

    x0 -= cdiv(y, yp);
    i++;
  }
  return ivec2(0, MAX_ITERATIONS + 1);
}

vec3 dettractor(vec2 start)
{
  int i = 0;
  int v;
  vec2 x0 = start;
  while (i < MAX_ITERATIONS)
  {
    // update currentPosition
    vec2 y = evalP(x0);
    vec2 yp = evalD(x0);

    x0 -= cdiv(y, yp);
    i++;
  }
  return vec3(x0, 0);
}

vec3 colorFromResult(vec2 root, float k)
{
  if (length(root) < 0.00001) return vec3(0.5);
  float magnitude = (MAX_ITERATIONS - k) / MAX_ITERATIONS;
  highp float angle = carg(root) + time;
  return magnitude * vec3(
    sin(angle) * 0.5 + 0.5,
    sin(angle + M_PI / 3) * 0.5 + 0.5,
    sin(angle + 2 * M_PI / 3) * 0.5 + 0.5);
}

vec2 apply(vec2 z)
{
  return z - cdiv(evalP(z), evalD(z));
}

vec3 fixedPoint(vec2 start)
{
  vec2 z = start;
  vec2 z0 = apply(start);

  int iters = 0;
  while (length(z0 - z) > 0.0000001 && iters < MAX_ITERATIONS)
  {
    z = z0;
    z0 = apply(z0);
    iters += 1;
  }
  if (iters == MAX_ITERATIONS - 1) return vec3(0);

  return colorFromResult(z, iters);
}

void main()
{
  float screenDim = max(screenSize.x, screenSize.y);
  float xMax = float((gl_FragCoord.x - screenSize.x / 2) / screenDim);
  float yMax = float((gl_FragCoord.y - screenSize.y / 2) / screenDim);

  float x = scale * xMax + pos.x;
  float y = scale * yMax + pos.y;
  highp vec2 z = vec2(x, y);

  // okay, so now we have a complex number x + iy
  // colorFrag = clamp(vec4(fixedPoint(z), 1), 0, 1);

  ivec2 basin = attractor(z);
  if (basin.x == -2) {
    discard;
  } else if (basin.x == -1) {
    // colorFrag = clamp(vec4(dettractor(z), 1), 0, 1);
    discard;
  } else {
    colorFrag = clamp(vec4(colorFromResult(roots[basin.x], basin.y), 1), 0, 1);
  }
  // int i = 0;
  // while (length(evalP(z)) > 0.00001 && i < MAX_ITERATIONS) {
  //   z -= cdiv(evalP(z), evalD(z));
  //   i++;
  // }
  // int done = 0;
  // for (int j = 0; j < degree; j++) {
  //   if (length(z - roots[j]) < 0.1) {
  //     colorFrag = clamp(abs(vec4(float(i) / MAX_ITERATIONS * colors[j], 1)), 0, 1);
  //     done = 1;
  //   }
  // }
  // if (done == 0) {
  //   colorFrag = clamp(abs(vec4(float(i) / MAX_ITERATIONS)), 0, 1);
  // }
  // colorFrag = clamp(vec4(colorFromResult(z, MAX_ITERATIONS * (1 - length(z))), 1), 0, 1);
}
