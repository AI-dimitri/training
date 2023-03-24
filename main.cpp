#include "utilities.h"

int main()
{
	GLFWcore::GLFWcoreCreate();
	GLFWwindow* window = nullptr;
	createWindow(window);
	gladLoad();

	ShaderProgram shader("vertex.vert", "fragment.frag");
	unsigned int VAO;
	configureBuffers(VAO);
	unsigned int texture;
	configureTexture(texture);

	shader.use();
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, texture);

	configureWorldView(shader);

	Camera camera{ glm::vec3{3.0f, 3.0f, 3.0f}, glm::vec3{0.0f, 0.0f, 0.0f} };

	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera, static_cast<float>(glfwGetTime()));

		glClearColor(0.93f, 0.5f, 0.93f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.setMatrixfv("view", glm::value_ptr(camera.calculateLookAt()), false);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
	return 0;
}