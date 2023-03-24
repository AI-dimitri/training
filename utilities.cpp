#include "utilities.h"

#include <iostream>

#include "stb_image.h"

/*------------------------------------------------------------------------------------------*/

GLFWcore::GLFWcore()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWcore& GLFWcore::GLFWcoreCreate()
{
	static GLFWcore object;
	return object;
}

/*------------------------------------------------------------------------------------------*/

Shader::Shader(const GLenum shaderType, const char* const sourceFilePath, char* const tempBuffer) : coreInfo{shaderType}
{
	if (this->coreInfo.ID == 0)
		throw std::runtime_error("An error uccured creating the shader object\n");

	this->getSourceCode(tempBuffer, sourceFilePath);
	this->compileShader(tempBuffer);
}

void Shader::getSourceCode(char* const sourceCode, const char* const filePath)
{
	std::ifstream shaderFile{ filePath };
	shaderFile.exceptions(std::ifstream::badbit);

	int i = 0;
	while (!shaderFile.eof())
	{
		sourceCode[i] = shaderFile.get();

		if (i >= (bufferLength - 1) && !shaderFile.fail())
			throw std::runtime_error("Buffer size is too small");
		if (shaderFile.fail())
			break;
		++i;
	}
	sourceCode[i] = '\0';
}

void Shader::compileShader(const char* const sourceCode)
{
	glShaderSource(this->coreInfo.ID, 1, &sourceCode, nullptr);
	glCompileShader(this->coreInfo.ID);

	int success;
	glGetShaderiv(this->coreInfo.ID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(this->coreInfo.ID, bufferLength, nullptr, const_cast<char* const>(sourceCode));
		std::cerr << sourceCode << std::endl;
		throw std::runtime_error("An error occured compiling the shader source code\n");
	}
}

void Shader::attachToShaderProgram(const unsigned int shaderProgramid)
{
	if (this->shaderProgramID != 0)
		throw std::runtime_error("Shader already attached to some shader programm");
	glAttachShader(shaderProgramid, this->coreInfo.ID);
	this->shaderProgramID = shaderProgramid;
}

void Shader::detachFromShaderProgram()
{
	if (this->shaderProgramID == 0)
		throw std::runtime_error("Shader cannot be detached because he is not attached to any shader program");
	glDetachShader(this->shaderProgramID, this->coreInfo.ID);
	this->shaderProgramID = 0;
}

/*------------------------------------------------------------------------------------------*/

ShaderProgram::ShaderProgram(const char* const vertexPath, const char* const fragmentPath)
{
	if (this->coreInfo.ID == 0)
		throw std::runtime_error("An error uccured creating the shader program object");

	char buffer[bufferLength];
	Shader vertex(GL_VERTEX_SHADER, vertexPath, buffer);
	Shader fragment(GL_FRAGMENT_SHADER, fragmentPath, buffer);

	linkShaderProgram(vertex, fragment, buffer);
}

void ShaderProgram::linkShaderProgram(Shader& vertex, Shader& fragment, char* const tempBuffer)
{
	vertex.attachToShaderProgram(this->coreInfo.ID);
	fragment.attachToShaderProgram(this->coreInfo.ID);
	glLinkProgram(this->coreInfo.ID);
	vertex.detachFromShaderProgram();
	fragment.detachFromShaderProgram();

	int success;
	glGetProgramiv(this->coreInfo.ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->coreInfo.ID, bufferLength, nullptr, tempBuffer);
		std::cerr << tempBuffer << std::endl;
		throw std::runtime_error("An error occured lonking the shader program\n");
	}
}

void ShaderProgram::setInt(const char* const name, const int value)
{
	this->use();
	int uniformLocation = glGetUniformLocation(this->coreInfo.ID, name);
	if (uniformLocation == -1)
		throw std::runtime_error("There is no such uniform");
	glUniform1i(uniformLocation, value);
}

void ShaderProgram::setFloat(const char* const name, const float value)
{
	this->use();
	int uniformLocation = glGetUniformLocation(this->coreInfo.ID, name);
	if (uniformLocation == -1)
		throw std::runtime_error("There is no such uniform");
	glUniform1f(uniformLocation, value);
}

void ShaderProgram::setMatrixfv(const char* const name, const float* const value, const bool columnMajorOrdering)
{
	this->use();
	int transformLoc = glGetUniformLocation(this->coreInfo.ID, name);
	if (transformLoc == -1)
		throw std::runtime_error("There is no such uniform");
	glUniformMatrix4fv(transformLoc, 1, columnMajorOrdering, value);
}

/*------------------------------------------------------------------------------------------*/

Camera::Camera(glm::vec3 cameraPosit, glm::vec3 observedObjectPosition)
	: cameraPosition(cameraPosit)
{
	this->cameraZ = glm::normalize(this->cameraPosition - observedObjectPosition);
	this->cameraY = glm::normalize(gramSchmidtProcess());
	this->cameraX = glm::normalize(glm::cross(this->cameraY, this->cameraZ));
}

void Camera::processKeyboard(const cameraMovement direction, const float deltaTime)
{
	if (direction == worldForward)
		this->cameraPosition -= this->worldZ * this->movementSpeed * deltaTime;
	if (direction == worldBackward)
		this->cameraPosition += this->worldZ * this->movementSpeed * deltaTime;
	if (direction == worldRight)
		this->cameraPosition += this->worldX * this->movementSpeed * deltaTime;
	if (direction == worldLeft)
		this->cameraPosition -= this->worldX * this->movementSpeed * deltaTime;
	if (direction == worldUp)
		this->cameraPosition += this->worldY * this->movementSpeed * deltaTime;
	if (direction == worldDown)
		this->cameraPosition -= this->worldY * this->movementSpeed * deltaTime;

	if (direction == cameraForward)
		this->cameraPosition -= this->cameraZ * this->movementSpeed * deltaTime;
	if (direction == cameraBackward)
		this->cameraPosition += this->cameraZ * this->movementSpeed * deltaTime;
	if (direction == cameraRight)
		this->cameraPosition += this->cameraX * this->movementSpeed * deltaTime;
	if (direction == cameraLeft)
		this->cameraPosition -= this->cameraX * this->movementSpeed * deltaTime;
	if (direction == cameraUp)
		this->cameraPosition += this->cameraY * this->movementSpeed * deltaTime;
	if (direction == cameraDown)
		this->cameraPosition -= this->cameraY * this->movementSpeed * deltaTime;

	if (direction == pitchUp)
		rotateAround(this->cameraX, 0.5f * deltaTime);
	if (direction == pitchDown)
		rotateAround(this->cameraX, -0.5f * deltaTime);
	if (direction == yawRight)
		rotateAround(this->cameraY, 0.5f * deltaTime);
	if (direction == yawLeft)
		rotateAround(this->cameraY, -0.5f * deltaTime);
	if (direction == rollRight)
		rotateAround(this->cameraZ, 0.5f * deltaTime);
	if (direction == rollLeft)
		rotateAround(this->cameraZ, -0.5f * deltaTime);
}

glm::mat4 Camera::calculateLookAt()
{
	glm::mat4 translation = glm::mat4(1.0f);
	translation[3][0] = -this->cameraPosition.x;
	translation[3][1] = -this->cameraPosition.y;
	translation[3][2] = -this->cameraPosition.z;

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0][0] = cameraX.x;
	rotation[1][0] = cameraX.y;
	rotation[2][0] = cameraX.z;
	rotation[0][1] = cameraY.x;
	rotation[1][1] = cameraY.y;
	rotation[2][1] = cameraY.z;
	rotation[0][2] = cameraZ.x;
	rotation[1][2] = cameraZ.y;
	rotation[2][2] = cameraZ.z;

	return glm::mat4{ rotation * translation };
}

glm::vec3 Camera::gramSchmidtProcess()
{
	float numerator = glm::dot(this->worldY, this->cameraZ);
	float denominator = glm::dot(this->cameraZ, this->cameraZ);
	float projectionOperator = numerator / denominator;
	glm::vec3 projectionVector = projectionOperator * this->cameraZ;
	return glm::vec3{ this->worldY - projectionVector };
};

void Camera::rotateTransform(glm::vec3& vector, const glm::mat4& rotateOperator)
{
	glm::vec4 homogeneousVector{ vector.x, vector.y, vector.z, 1.0f };
	homogeneousVector = rotateOperator * homogeneousVector;
	vector.x = homogeneousVector.x;
	vector.y = homogeneousVector.y;
	vector.z = homogeneousVector.z;
}

void Camera::rotateAround(glm::vec3 rotateAxis, const float angle)
{
	glm::mat4 A{ 1.0f };
	const float sin = glm::sin(angle);
	const float cos = glm::cos(angle);
	const float x = rotateAxis.x;
	const float y = rotateAxis.y;
	const float z = rotateAxis.z;
	A[0][0] = cos + x * x * (1 - cos);
	A[0][1] = x * y * (1 - cos) - z * sin;
	A[0][2] = x * z * (1 - cos) + y * sin;
	A[1][0] = y * x * (1 - cos) + z * sin;
	A[1][1] = cos + y * y * (1 - cos);
	A[1][2] = y * z * (1 - cos) - x * sin;
	A[2][0] = z * x * (1 - cos) - y * sin;
	A[2][1] = z * y * (1 - cos) + x * sin;
	A[2][2] = cos + z * z * (1 - cos);

	if (this->cameraX != rotateAxis)
		rotateTransform(this->cameraX, A);
	if (this->cameraY != rotateAxis)
		rotateTransform(this->cameraY, A);
	if (this->cameraZ != rotateAxis)
		rotateTransform(this->cameraZ, A);
}

/*------------------------------------------------------------------------------------------*/

void framebuffer_size_callback(GLFWwindow* window, int new_width, int new_height)
{
	glViewport(0, 0, new_width, new_height);
}

void processInput(GLFWwindow* window, Camera& cam, const float currentFrame)
{
	static float deltaTime{ 0 };
	static float lastFrame{ 0 };
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cam.processKeyboard(worldForward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cam.processKeyboard(worldBackward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cam.processKeyboard(worldLeft, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cam.processKeyboard(worldRight, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
		cam.processKeyboard(worldUp, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		cam.processKeyboard(worldDown, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.processKeyboard(cameraForward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.processKeyboard(cameraBackward, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.processKeyboard(cameraLeft, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.processKeyboard(cameraRight, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.processKeyboard(cameraUp, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cam.processKeyboard(cameraDown, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cam.processKeyboard(pitchUp, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		cam.processKeyboard(pitchDown, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cam.processKeyboard(yawLeft, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cam.processKeyboard(yawRight, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		cam.processKeyboard(rollLeft, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cam.processKeyboard(rollRight, deltaTime);
}

void configureBuffers(unsigned int& vao)
{
	glGenVertexArrays(1, &vao);
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	unsigned int ebo;
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
}

void configureTexture(unsigned int& tex)
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = nullptr;
	data = stbi_load("Obamium.jpg", &width, &height, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void configureWorldView(ShaderProgram& shaderProgram)
{
	glm::mat4 model{1.0f};
	model = glm::translate(model, glm::vec3(1.0f, 0.0f, -3.0f));
	model = glm::rotate(model, glm::radians(25.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 projection{1.0f};
	projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);

	shaderProgram.setMatrixfv("model", glm::value_ptr(model), false);
	shaderProgram.setMatrixfv("projection", glm::value_ptr(projection), false);
}

void createWindow(GLFWwindow*& window)
{
	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failder to create GLFW window");
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void gladLoad()
{
	int success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	if (!success)
		throw std::runtime_error("Failed to initialize GLAD");

	glEnable(GL_DEPTH_TEST);
}