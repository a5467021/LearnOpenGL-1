// Sept 5, 2019 by a5467021
// This code is mainly instructed by LearnOpenGL.
// See here: https://learnopengl.com/
// Or Chinese-translated version here: https://learnopengl-cn.github.io
// If you want something challenging, read my code and think about how I number
// my remarks before reading the tutorial:)

#include <iostream>

/* 1 glad should be loaded before GLFW */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

/* 14 register functions here */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

/* 6 settings here */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/** 23
 * Source code of a simple vertex shader that can be compiled through OpenGL.
 * "gl_Position" is a predefined variable used as the result of the shader.
 * (F**k the P here is in UPPER CASE)
 */
const char *vertexShaderSource = \
"#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
\
void main()\
{\
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\
}";

/* 26 fragment shader source code */
const char *fragmentShaderSource = \
"#version 330 core\n\
out vec4 FragColor;\n\
\
void main()\
{\
	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\
}";

int main()
{
	glfwInit();
	/* 2 set the values for GLFW context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	/* 2 the following line is for MacOS compatibility */
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/** 3
	 * Create a new window with GLFW,
	 * and set the size and the title of the window.
	 */
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Learn OpenGL", NULL, NULL);
	if(window == NULL)
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	/* 4 make the window main context -> "processing in this window"? */
	glfwMakeContextCurrent(window);

	/* 5 glad is for loading external functions */
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize glad" << endl;
		return -1;
	}

	/** 7
	 * Set the render area property for OpenGL. -> The term is "viewport".
	 * Can be set smaller to the window size to display other components
	 * in the same window.
	 */
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	/* 16 bind the callback function to the window */
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);        // 13 set a status for the CONTEXT

	/* 19 define an OpenGL triangle */
	float vertices[] = {
		 0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f
	};

	/* 20 generate Vertex Buffer Object and get its unique id */
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	/** 21
	 * Bind(open) the registered VBO for use.
	 * OpenGL will only use at most one buffer of each type in one context.
	 * Bind to another to unbind current one.
	 */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	/* 22 copy data to graphic device memory using the buffer created */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* 24 create a vertex shader at runtime */
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	/* 25 debug code for checking whether the shader build is success */
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "ERROR: Vertex shader compilation failed" << endl;
		cout << "Information is: " << infoLog << endl;
	}

	/* 27 the same for fragment shader */
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "ERROR: Fragment shader compilation failed" << endl;
		cout << "Information is: " << infoLog << endl;
	}

	/* 28 "link" these shaders into a program, as a pipe */
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "ERROR: Shader program link failed" << endl;
		cout << "Information is: " << infoLog << endl;
	}

	/* 29 delete the separated shaders as they are no longer needed */
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/** 33
	 * Create a Vertex Array Object in order to manage VBOs and their attribute configs.
	 * VAO is forced to use in OpenGL core mode.
	 */
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	/** 34
	 * Only one VAO and one VBO may connect to the context at one time.
	 * VAO saves all configurations, including which VBO to use.
	 * So we just neet to switch bewteen VAOs to do different object renders.
	 * Once a VAO is bound to the context, *ALL* configuration is on this VAO.
	 */
	glBindVertexArray(VAO);

	/** 31
	 * Specify how shaders read the attributes of vertices.
	 * Arguments of the function that tells OpenGL how the data is organized:
	 * location, which matches the position declared in the vertex shader above;
	 * dimensions, how many dimensions does a vertex in the buffer have;
	 * data type, how to interpret the binary data and the size of each value;
	 * "normalize", whether mapping the points to OpenGL's coordinate system of [-1,1];
	 * "stride", size of a vertex + indent between 2 vertices?
	 * offset, where the data begins in the buffer.
	 */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	/* 32 Enable vertex attributes. Why disabled? */
	glEnableVertexAttribArray(0);

	/* 37 Setting of this VAO is finished. Unbind it here. */
	glBindVertexArray(0);

	/** 38
	 * Use the following line to unbind VBO.
	 * Never unbind a VBO before the corresponding VAO is fully configured!!
	 */
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* 8 loop refresh to keep displaying */
	while(!glfwWindowShouldClose(window))
	{
		/* 18 our own handler of some inputs */
		process_input(window);

		/* 12 rendering instructions here*/
		glClear(GL_COLOR_BUFFER_BIT);            // fetch the status and use it

		/* 30 activate the program (all following renders use this program) */
		glUseProgram(shaderProgram);

		/** 35
		 * It may be hard to think why to bind the VAO here again.
		 * Considering multiple VAOs, it's natural to switch among them in context
		 * just before any specific one is intended to be rendered.
		 */
		glBindVertexArray(VAO);

		/** 36
		 * Finally the only thing we want!!! The triangle!!!
		 * The three parameters are:
		 * primitive type, how to connect and color the shape(point, triangle, line or etc.);
		 * "first", the index OpenGL starts to read data from a vertex?
		 * "count", how many values to read.
		 */
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/** 9
		 * The tutorial uses double buffers to avoid poor performance
		 * by rendering the pixels in the background rather than just-in-time.
		 * This function dumps all the rendered pixels to the display buffer.
		 */
		glfwSwapBuffers(window);
		/* 10 pass events to corresponding handlers(callbacks) */
		glfwPollEvents();
	}

	/* 11 release all the resources before quitting */
	glfwTerminate();

	return 0;
}

/** 15
 * Function designed for rendering area resize callback.
 * Needs to be bound to the window in the main routine.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	/* 17 "if the key status is PRESS" -> "the key is now pressed" */
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
		|| glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
	if(glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		glClearColor(0.2f, 1.0f, 0.2f, 1.0f);
	if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glClearColor(0.2f, 0.2f, 1.0f, 1.0f);
}
