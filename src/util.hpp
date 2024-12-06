#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <iostream>

namespace util {
  inline constexpr float GRAVITY = -9.81f;

  
  inline GLuint loadTexture(const std::string& path) {
      int width, height, nrChannels;
      unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

      if (!data) {
          std::cerr << "Failed to load texture: " << path << std::endl;
          return 0;
      }

      GLuint textureID;
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_2D, textureID);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      stbi_image_free(data);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      return textureID;
  }
}
