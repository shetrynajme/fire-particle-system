#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>

namespace util {
inline constexpr float GRAVITY = -9.81f;

inline GLuint loadTexture(const std::string &path) {
  int width, height, nrChannels;
  unsigned char *data =
      stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    std::cerr << "Failed to load texture: " << path << std::endl;
    return 0;
  }

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return textureID;
}

static glm::vec3 computeCurlNoise(const glm::vec3 &p) {
  float eps = 1e-4f; // or so

  // Offsets to sample the noise
  glm::vec3 dx(eps, 0.0f, 0.0f);
  glm::vec3 dy(0.0f, eps, 0.0f);
  glm::vec3 dz(0.0f, 0.0f, eps);

  // Let’s assume we have a function noise3D(...) that returns Perlin (or
  // Simplex) noise for a 3D coordinate. You can reuse glm::perlin for that, but
  // do it 3 times:
  auto noise = [](const glm::vec3 &q) { return glm::perlin(q); };

  float n_x1 = noise(p + dy) - noise(p - dy);
  float n_x2 = noise(p + dz) - noise(p - dz);

  float n_y1 = noise(p + dz) - noise(p - dz);
  float n_y2 = noise(p + dx) - noise(p - dx);

  float n_z1 = noise(p + dx) - noise(p - dx);
  float n_z2 = noise(p + dy) - noise(p - dy);

  // Combine them into the final curl vector
  float x = n_x1 - n_x2;
  float y = n_y1 - n_y2;
  float z = n_z1 - n_z2;

  return glm::vec3(x, y, z);
}

} // namespace util
