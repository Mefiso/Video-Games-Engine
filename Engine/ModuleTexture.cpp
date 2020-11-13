#include "ModuleTexture.h"
#include "GL/glew.h"
#include "IL/il.h"
#include "IL/devil_cpp_wrapper.hpp"

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	{
		return false;
	}
	ilInit();
	iluInit();

	return true;
}

bool ModuleTexture::CleanUp()
{
	// TODO: clean textures vector
	return true;
}

unsigned int ModuleTexture::LoadTexture(const std::string path)
{
	ILuint imgId;
	ILboolean success;
	ILinfo info;

	unsigned int textureId;

	ilGenImages(1, &imgId); /* Generation of one image name */
	ilBindImage(imgId); /* Binding of image name */
	success = ilLoadImage(path.c_str()); /* Loading of image "image.jpg" */
	if (success) /* If no error occured: */
	{
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success)
		{
			/* Error occured */
			return false;
		}
		iluGetImageInfo(&info);
		if (info.Origin == IL_ORIGIN_UPPER_LEFT)
			iluFlipImage();

		glGenTextures(1, &textureId); /* Texture name generation */
		glBindTexture(GL_TEXTURE_2D, textureId); /* Binding of texture name */
		
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
			ilGetData()); /* Texture specification */
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		/* Error occured */
		return false;
	}
	ilDeleteImages(1, &imgId);

	return textureId;
}