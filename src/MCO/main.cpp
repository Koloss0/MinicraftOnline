#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
{
	// INIT
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// CREATING A WINDOW
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
 		std::cout << "Failed to create GLFW window" << std::endl;
    		glfwTerminate();
    		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// LOAD GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
    		std::cout << "Failed to initialize GLAD" << std::endl;
    		return -1;
	}
	
	// SET THE VIEWPORT SIZE
	glViewport(0, 0, 800, 600);
	
	// RENDER LOOP
	while(!glfwWindowShouldClose(window))
	{
		// BG
		glClearColor(0.5f, 0.4f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

    		glfwSwapBuffers(window);
    		glfwPollEvents();    
	}
  	
	glfwTerminate();
	return 0;
}
