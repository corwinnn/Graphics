// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>


double X(double t, double a=1, double b=1) {
    return (a + b) * cos(t) - b * cos(t * ((a + b) / b));
}

double Y(double t, double a=1, double b=1) {
    return (a + b) * sin(t) - b * sin(t * ((a + b) / b));
}

void draw(GLuint& programID, GLuint& MatrixID, glm::mat4& MVP, GLuint& vertexbuffer, GLuint& colorbuffer, int cnt) {
    glUseProgram(programID);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, cnt); // 12*3 indices starting at 0 -> 12 triangles

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1200, 1200, "Dance of Triangles", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
//	glEnable(GL_DEPTH_TEST);
//	// Accept fragment if it closer to the camera than the former one
//	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
    GLuint programID2 = LoadShaders( "TransformVertexShader2.vertexshader", "ColorFragmentShader2.fragmentshader" );
    GLuint programID3 = LoadShaders( "TransformVertexShader3.vertexshader", "ColorFragmentShader3.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");
    GLuint MatrixID3 = glGetUniformLocation(programID3, "MVP");


    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(4,3,-3), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-0.7f,-0.7f,-0.4f,
		-0.7f, 0.7f, -0.4f,
		 0.7f, 0.0f, -0.4f,
	};
    static const GLfloat g_vertex_buffer_data2[] = {
            0.7,-0.7f, 0.4f,
            0.7f, 0.7f, 0.4f,
            -0.7f, 0.0f, 0.4f,
    };

    float q = 0.2;
    static const GLfloat g_vertex_buffer_data3[] = {
            q, 0, 0,
            0, 0, -q,
            0, 2*q, 0,

            0, 0, -q,
            0, 2*q, 0,
            -q, 0, 0,

            0, 2*q, 0,
            -q, 0, 0,
            0, 0, q,

            0, 2*q, 0,
            0, 0, q,
            q, 0, 0,

            q, 0, 0,
            0, 0, -q,
            0, -2*q, 0,

            0, 0, -q,
            0, -2*q, 0,
            -q, 0, 0,

            0, -2*q, 0,
            -q, 0, 0,
            0, 0, q,

            0, -2*q, 0,
            0, 0, q,
            q, 0, 0,


    };

    static const GLfloat g_color_buffer_data[] = {
            0.8f,  0.1f,  0.14f,
            0.09f,  0.8f,  0.16f,
            0.07f,  0.03f,  0.844f,
    };
    static const GLfloat g_color_buffer_data2[] = {
            0.0822f,  0.069f,  0.801f,
            0.035f,  0.8202f,  0.0223f,
            0.810f,  0.0747f,  0.0185f,
    };

    static const GLfloat g_color_buffer_data3[] = {
            0, 0.25, 0.25,
            0.75, 0.75, 0.75,
            1, 0, 0,

            0.75, 0.75, 0.75,
            1, 0.25, 0.25,
            0, 0.25, 0.25,

            1, 0, 0,
            0, 0.25, 0.25,
            0, 0.25, 0.25,

            1, 0, 0,
            0, 0.25, 0.25,
            0, 0.25, 0.25,

            0, 0.2, 0.45,
            0.75, 0.75, 0.75,
            1, 0, 0,

            0.75, 0.75, 0.75,
            1, 0, 0,
            0, 0.2, 0.45,

            1, 0, 0,
            0, 0.2, 0.45,
            0, 0.2, 0.45,

            1, 0, 0,
            0, 0.2, 0.45,
            0, 0.2, 0.45,


    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    GLuint vertexbuffer2;
    glGenBuffers(1, &vertexbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data2), g_vertex_buffer_data2, GL_STATIC_DRAW);

    GLuint colorbuffer2;
    glGenBuffers(1, &colorbuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data2), g_color_buffer_data2, GL_STATIC_DRAW);

    GLuint vertexbuffer3;
    glGenBuffers(1, &vertexbuffer3);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data3), g_vertex_buffer_data3, GL_STATIC_DRAW);

    GLuint colorbuffer3;
    glGenBuffers(1, &colorbuffer3);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data3), g_color_buffer_data3, GL_STATIC_DRAW);
    double x = 0, h = -3;

	do{
        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix

        glm::mat4 View       = glm::lookAt(
                glm::vec3(4*sin(h), X(x),Y(x)),// Camera is at (4,3,-3), in World Space
                glm::vec3(0,0,0), // and looks at the origin
                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        x+=0.001;h+=0.001;
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model      = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        draw(programID, MatrixID, MVP, vertexbuffer, colorbuffer, 3);
        draw(programID2, MatrixID2, MVP, vertexbuffer2, colorbuffer2, 3);
        draw(programID3, MatrixID3, MVP, vertexbuffer3, colorbuffer3, 3*8);


        // Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

