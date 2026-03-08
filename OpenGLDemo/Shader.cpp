#include "Shader.h"

Shader::Shader(const std::filesystem::path& vertexPath,
	const std::filesystem::path& fragmentPath)
{
	// Read shader source from files
	std::string vertexSource = readFile(vertexPath);
	std::string fragmentSource = readFile(fragmentPath);

	//std::cerr << vertexSource;
	//std::cerr << fragmentSource;

	if (vertexSource.empty() || fragmentSource.empty()) {
		std::cerr << "ERROR::SHADER::FILE_READ_FAILED\n";
		std::cerr << "Vertex path: " << vertexPath << "\n";
		std::cerr << "Fragment path: " << fragmentPath << std::endl;
		return;
	}

	compileAndLink(vertexSource, fragmentSource);
}

Shader Shader::fromSource(std::string_view vertexSource,
	std::string_view fragmentSource)
{
	Shader shader;
	shader.compileAndLink(vertexSource, fragmentSource);
	return shader;
}

Shader::Shader(Shader&& other) noexcept
	: programID_(other.programID_)
{
	other.programID_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other) {
		// Delete current program if valid
		if (programID_ != 0) {
			glDeleteProgram(programID_);
		}

		// Take ownership of other's resources
		programID_ = other.programID_;
		other.programID_ = 0;
	}
	return *this;
}

Shader::~Shader()
{
	if (programID_ != 0) {
		glDeleteProgram(programID_);
	}
}

void Shader::use() const noexcept
{
	if (programID_ != 0) {
		glUseProgram(programID_);
	}
}

bool Shader::compileAndLink(std::string_view vertexSource,
	std::string_view fragmentSource)
{
	// Compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (!compileShader(vertexShader, vertexSource, "VERTEX")) {
		glDeleteShader(vertexShader);
		return false;
	}

	// Compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(fragmentShader, fragmentSource, "FRAGMENT")) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return false;
	}

	// Create and link program
	programID_ = glCreateProgram();
	glAttachShader(programID_, vertexShader);
	glAttachShader(programID_, fragmentShader);

	bool linkSuccess = linkProgram(programID_);

	// Clean up individual shaders (they're linked into program now)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (!linkSuccess) {
		glDeleteProgram(programID_);
		programID_ = 0;
		return false;
	}

	return true;
}

bool Shader::compileShader(GLuint shader, std::string_view source,
	std::string_view shaderType)
{
	const char* sourcePtr = source.data();
	const GLint sourceLength = static_cast<GLint>(source.length());

	glShaderSource(shader, 1, &sourcePtr, &sourceLength);
	glCompileShader(shader);

	// Check for compilation errors
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar infoLog[1024];
		glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n"
			<< infoLog << std::endl;
		return false;
	}

	return true;
}

bool Shader::linkProgram(GLuint program)
{
	glLinkProgram(program);

	// Check for linking errors
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		GLchar infoLog[1024];
		glGetProgramInfoLog(program, 1024, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
			<< infoLog << std::endl;
		return false;
	}

	return true;
}

std::string Shader::readFile(const std::filesystem::path& filePath)
{
	std::ifstream file;

	// Ensure ifstream can throw exceptions
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(filePath);
		std::stringstream fileStream;
		fileStream << file.rdbuf();
		file.close();
		return fileStream.str();
	}
	catch (const std::ifstream::failure& e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
			<< filePath << "\n"
			<< "Exception: " << e.what() << std::endl;
		return "";
	}
}

GLint Shader::getUniformLocation(std::string_view name) const
{
	// Note: For better performance, consider caching uniform locations
	// in a std::unordered_map<std::string, GLint>
	return glGetUniformLocation(programID_, name.data());
}

// Uniform setters
void Shader::setBool(std::string_view name, bool value) const
{
	glUniform1i(getUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(std::string_view name, int value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(std::string_view name, float value) const
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(std::string_view name, const glm::vec2& value) const
{
	glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec2(std::string_view name, float x, float y) const
{
	glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(std::string_view name, const glm::vec3& value) const
{
	glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(std::string_view name, float x, float y, float z) const
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void Shader::setVec4(std::string_view name, const glm::vec4& value) const
{
	glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(std::string_view name, float x, float y, float z, float w) const
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(std::string_view name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(std::string_view name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(std::string_view name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}