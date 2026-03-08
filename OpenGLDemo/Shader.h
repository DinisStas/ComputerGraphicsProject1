#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

class Shader {
public:
	// Constructors
	Shader() = default;

	// Construct from file paths
	explicit Shader(const std::filesystem::path& vertexPath,
		const std::filesystem::path& fragmentPath);

	// Construct from source strings
	static Shader fromSource(std::string_view vertexSource,
		std::string_view fragmentSource);

	// Deleted copy semantics (OpenGL resources shouldn't be copied)
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	// Move semantics
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other) noexcept;

	// Destructor
	~Shader();

	// Activate the shader
	void use() const noexcept;

	// Check if shader is valid
	[[nodiscard]] bool isValid() const noexcept { return programID_ != 0; }

	// Get the program ID
	[[nodiscard]] GLuint getID() const noexcept { return programID_; }

	// Utility uniform functions
	void setBool(std::string_view name, bool value) const;
	void setInt(std::string_view name, int value) const;
	void setFloat(std::string_view name, float value) const;
	void setVec2(std::string_view name, const glm::vec2& value) const;
	void setVec2(std::string_view name, float x, float y) const;
	void setVec3(std::string_view name, const glm::vec3& value) const;
	void setVec3(std::string_view name, float x, float y, float z) const;
	void setVec4(std::string_view name, const glm::vec4& value) const;
	void setVec4(std::string_view name, float x, float y, float z, float w) const;
	void setMat2(std::string_view name, const glm::mat2& mat) const;
	void setMat3(std::string_view name, const glm::mat3& mat) const;
	void setMat4(std::string_view name, const glm::mat4& mat) const;

private:
	// Compile and link shaders from source strings
	bool compileAndLink(std::string_view vertexSource, std::string_view fragmentSource);

	// Helper functions
	static bool compileShader(GLuint shader, std::string_view source, std::string_view shaderType);
	static bool linkProgram(GLuint program);
	static std::string readFile(const std::filesystem::path& filePath);

	// Get uniform location with caching potential
	[[nodiscard]] GLint getUniformLocation(std::string_view name) const;

	// OpenGL program ID
	GLuint programID_ = 0;
};