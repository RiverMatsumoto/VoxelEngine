#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

class Shader
{
public:
	unsigned int ID;

	// read and build shader
	Shader(const char* vertexPath, const char* fragmentPath);

	// use/activate shader
	void use();
	void setBool(const std::string &, bool) const;
	void setInt(const std::string &, int) const;
	void setFloat(const std::string &, float) const;
	void setVec2(const std::string&, const glm::vec2&) const;
	void setVec2(const std::string&, float x, float y) const;
	void setVec3(const std::string&, const glm::vec3&) const;
	void setVec3(const std::string&, float x, float y, float z) const;
	void setVec4(const std::string&, const glm::vec4&) const;
	void setVec4(const std::string&, float x, float y, float z, float w) const;
	void setMat2(const std::string&, const glm::mat2&) const;
	void setMat3(const std::string&, const glm::mat3&) const;
	void setMat4(const std::string&, const glm::mat4&) const;
};

#endif
