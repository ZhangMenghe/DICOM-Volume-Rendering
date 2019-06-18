#include "Texture.h"

Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(0) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(depth) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_3D, mTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, mFormat, mType, nullptr);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_3D, 0);
}

Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, void* data)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(0) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

}
Texture::Texture(GLenum internalFormat, GLenum format, GLenum type, unsigned int width, unsigned int height, unsigned int depth, void* data)
	: mTexture(0), mInternalFormat(internalFormat), mFormat(format), mType(type), mWidth(width), mHeight(height), mDepth(depth) {
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_3D, mTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, mFormat, mType, data);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_3D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &mTexture);
}