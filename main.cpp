#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// glfw: wenever the window size changed <by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window);
// glfw: initializate and cofigure
void glInit();
// glfw: window creation
void glCreateWindow(GLFWwindow* &window);
// glad: load all OpenGL function pointers
void gladLoad(int& success);
// build our shader program
void glMakeShader(int& success, unsigned int& shaderProgram);
// build vertex shader
void glCreateVertexShader(int& success, unsigned int& vertexShader);
// build fragment shader
void glCreateFragmentShader(int& success, unsigned int& fragmentShader);
// set up vertex data (and buffer(s)) and configure vertex attributes
void glCreateRectangleVertexArray(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO);

// settings
const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;
const unsigned int message_length = 512;

// error`s info output
char infoLog[message_length];

// vertex shader source code
const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
"}\0";
// fragment shader source code
const char *fragmentShaderSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

int main()
{
	GLFWwindow* window = nullptr;
	unsigned int shaderProgram = NULL;
	unsigned int VAO = NULL;
	unsigned int VBO = NULL;
	unsigned int EBO = NULL;
	int success = NULL;

	glInit();
	glCreateWindow(window);
	gladLoad(success);
	if (window == nullptr or success == NULL)
		goto create_window_error;

	glMakeShader(success, shaderProgram);
	if (success == NULL)
		goto shader_build_error;

	glCreateRectangleVertexArray(VAO, VBO, EBO);
	// seeing as we only have a single VAO there`s no need to bind it every time
	glBindVertexArray(VAO);
	// draw primitives as lines
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		// rendering commands here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// draw our first rectangle
		glUseProgram(shaderProgram);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
shader_build_error:
	glDeleteProgram(shaderProgram);
create_window_error:
	glfwTerminate();
	
	if (success == NULL)
		return -1;
	else
		return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void glInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void glCreateWindow(GLFWwindow*& window)
{
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void gladLoad(int& success)
{
	success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	if (success == NULL)
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return;
	}
}

void glMakeShader(int& success, unsigned int& shaderProgram)
{
	unsigned int vertexShader;
	glCreateVertexShader(success, vertexShader);
	if (success == NULL)
		return;
	unsigned int fragmentShader;
	glCreateFragmentShader(success, fragmentShader);
	if (success == NULL)
		return;
	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILDER\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void glCreateVertexShader(int& success, unsigned int& vertexShader)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	// check for shader compile errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == NULL)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void glCreateFragmentShader(int& success, unsigned int& fragmentShader)
{
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == NULL)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void glCreateRectangleVertexArray(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO)
{
	float verticles[] =
	{
		 0.5f,  0.5f, 0.0f, // top right
		 0.5f, -0.5f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f  // top left
	};

	unsigned int indices[] =
	{
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first
	glBindVertexArray(VAO);
	// then bind and set vertex buffer(s)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticles), verticles, GL_STATIC_DRAW);
	// copy our index in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// and then configure vertex attribute(s)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	// the call to glVertexAttribPointer registred VBO so afterwards we can safely unbind it
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// unbind the VAO afterwards so other VAO calls won`t accidientally modify this VAO
	glBindVertexArray(0);
	// 	do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}