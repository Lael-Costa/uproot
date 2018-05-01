#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <GL/glew.h>

/**
  * @class Texture2D
  *
  * Wrapper class for OpenGL 2D Textures. This is probably the class you'll want to use
  * for all of your texturing purposes.
  */
class Texture2D {
public:
  enum class FILTER_METHOD {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR
  };

  enum class WRAP_METHOD {
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    REPEAT = GL_REPEAT
  };

  enum class DATA_TYPE {
    FLOAT = GL_FLOAT,
    INT = GL_INT,
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE
  };

public:
  Texture2D(unsigned char *data,
    int width,
    int height,
    DATA_TYPE type = DATA_TYPE::UNSIGNED_BYTE,
    int numComponents = 4);

  void bind() const;
  unsigned int id() const;
  void unbind() const;

  void setFilterMethod(FILTER_METHOD filter);
  void setWrapMethod(WRAP_METHOD wrap);
  void setTextureParams(FILTER_METHOD filter, WRAP_METHOD wrap);

protected:
  unsigned int m_handle;
};

#endif // TEXTURE2D_H
