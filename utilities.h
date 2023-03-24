#ifndef UTILITIES_H
#define UTILITIES_H

#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static constexpr float width{ 800 };
static constexpr float height{ 600};
static constexpr int bufferLength{ 512 };

static const float vertices[] =
{
	-0.5f, -0.5f,  0.5f, 0.00667f, 0.36121f,
	 0.5f, -0.5f,  0.5f, 0.65833f, 0.36121f,
	-0.5f, -0.5f, -0.5f, 0.00667f, 0.00000f,
	 0.5f, -0.5f, -0.5f, 0.65833f, 0.00000f,
     0.0f,  0.5f,  0.0f, 0.33167f, 0.67739f, 
     0.5f, -0.5f, -0.5f, 0.99000f, 0.67923f,
    -0.5f, -0.5f, -0.5f, 0.66167f, 0.99632f, 
	-0.5f, -0.5f,  0.5f, 0.00167f, 0.99632f, 
};

static const unsigned int indices[] =
{
	0, 1, 2, 
	2, 3, 1, 
	0, 1, 4, 
	1, 5, 4, 
	5, 6, 4, 
	6, 7, 4  
};

/*------------------------------------------------------------------------------------------*/

class GLFWcore
{
public:
	static GLFWcore& GLFWcoreCreate();

	~GLFWcore()
	{
		glfwTerminate();
	}
private:
	GLFWcore();
};

/*------------------------------------------------------------------------------------------*/

class Shader
{
public:
	Shader(const GLenum shaderType, const char* const sourceFilePath, char* const tempBuffer);

	void attachToShaderProgram(const unsigned int shaderProgramid);
	void detachFromShaderProgram();

	~Shader() = default;
private:
	unsigned int shaderProgramID{ 0 };
	
	struct ShaderCore
	{
		unsigned int ID;

		explicit ShaderCore(const GLenum shaderType)
		{
			this->ID = glCreateShader(shaderType);
		}

		~ShaderCore()
		{
			glDeleteShader(this->ID);
		}
	} coreInfo;

	void getSourceCode(char* const sourceCode, const char* const filePath);
	void compileShader(const char* const sourceCode);
};

/*------------------------------------------------------------------------------------------*/

class ShaderProgram
{
public:
	ShaderProgram(const char* const vertexPath, const char* const fragmentPath);

	void use() const
	{
		glUseProgram(this->coreInfo.ID);
	}

	void setInt(const char* const name, const int value);
	void setFloat(const char* const name, const float value);
	void setMatrixfv(const char* const name, const float* const value, const bool columnMajorOrdering);

	~ShaderProgram() = default;
private:
	struct ShaderProgramCore
	{
		unsigned int ID;

		ShaderProgramCore()
		{
			this->ID = glCreateProgram();
		}

		~ShaderProgramCore()
		{
			glDeleteProgram(this->ID);
		}
	} coreInfo;

	void linkShaderProgram(Shader& vertex, Shader& fragment, char* const tempBuffer);
};

/*------------------------------------------------------------------------------------------*/
enum cameraMovement {
	worldForward, worldBackward, worldRight, worldLeft, worldUp, worldDown,
	cameraForward, cameraBackward, cameraRight, cameraLeft, cameraUp, cameraDown,
	pitchUp, pitchDown, yawRight, yawLeft, rollRight, rollLeft
};

class Camera
{
public:
	Camera(glm::vec3 cameraPosit, glm::vec3 observedObjectPosition);

	void processKeyboard(const cameraMovement direction, const float deltaTime);
	void processMouse(const float xoffset, const float yoffset);

	glm::mat4 calculateLookAt();

	~Camera() = default;
private:
	static constexpr float movementSpeed = 2.5f;
	static constexpr glm::vec3 worldY{ 0.0f, 1.0f, 0.0f };
	static constexpr glm::vec3 worldX{ 1.0f, 0.0f, 0.0f };
	static constexpr glm::vec3 worldZ{ 0.0f, 0.0f, 1.0f };

	glm::vec3 cameraZ;
	glm::vec3 cameraY;
	glm::vec3 cameraX;
	glm::vec3 cameraPosition;

	glm::vec3 gramSchmidtProcess();
	void rotateTransform(glm::vec3& vector, const glm::mat4& rotateOperator);
	void rotateAround(glm::vec3 rotateAxis, const float angle);
};

/*------------------------------------------------------------------------------------------*/

void framebuffer_size_callback(GLFWwindow* window, int new_width, int new_height);

void processInput(GLFWwindow* window, Camera& cam, const float currentFrame);

void configureBuffers(unsigned int& vao);
void configureTexture(unsigned int& tex);
void configureWorldView(ShaderProgram& shaderProgram);

void createWindow(GLFWwindow*& window);
void gladLoad();

#endif