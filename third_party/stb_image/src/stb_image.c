#define STB_IMAGE_IMPLEMENTATION

#define STBI_NO_LINEAR

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG

#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif

#include <stb_image.h>
