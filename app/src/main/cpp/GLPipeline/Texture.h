#ifndef GL_TEXTURE_H
#define GL_TEXTURE_H

// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

class Texture {
public:
	Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height);
	Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth);
	Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data);
	Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth, void* data);
	~Texture();

	unsigned int Width() const { return mWidth; }
	unsigned int Height() const { return mHeight; }
	unsigned int Depth() const { return mDepth; }
	GLuint GLTexture() const { return mTexture; }
	void Update(void* data);

	static void initFBO(GLuint& fbo, Texture* colorTex, Texture* depthTex);

private:
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mDepth;
	GLenum mFormat;
	GLenum mType;
	GLenum mInternalFormat;
	GLuint mTexture;
};

#endif