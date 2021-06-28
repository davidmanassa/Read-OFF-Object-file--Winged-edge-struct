#include "file_off.hpp"

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

#include "common/shader.hpp"

static GLuint vertexbuffer, colorbuffer;

int loadMesh( Mesh mesh ) {

	int length = 0;
	for (int i = 0; i < mesh.faces->size(); i++) {
		length += mesh.faces->at(i)->edges->size() + (2 * (mesh.faces->at(i)->edges->size() - 3));
	}
	length *= 3; // 3 valores por vetor (x, y, z)

	static GLfloat* g_vertex_buffer_data = (GLfloat*) malloc(length * sizeof(GLfloat)); 
	static GLfloat* g_color_buffer_data = (GLfloat*) malloc(length * sizeof(GLfloat));
	
	int i = 0;
	for (int f0 = 0; f0 < mesh.faceSize; f0++) {

		Face *face = mesh.faces->at(f0);

		int numberOfEdgesInTheFace = face->edges->size() + (2 * (face->edges->size() - 3));
		int numberOfTriangles = numberOfEdgesInTheFace / 3;

		// VERTEX BUFFER
		// Formação de triangulos
		Vertex *escolhido = face->edges->at(0)->v2;
		Vertex *naoEscolhido = face->edges->at(0)->v1;
		Vertex *inicial = NULL;
		int nTriangle = 1, nPoint = 1;

		// std::cout << " START " << std::endl;

		for (int e0 = 0; e0 < face->edges->size(); e0++) {

			Edge *edge = face->edges->at(e0);
			Vertex *v;
			
			if (edge->v1->id == naoEscolhido->id) {
				v = edge->v1;
				escolhido = edge->v1;
				naoEscolhido = edge->v2;
			} else if (edge->v1->id == escolhido->id) {
				v = edge->v2;
				escolhido = edge->v2;
				naoEscolhido = edge->v1;
			} else if (edge->v2->id == naoEscolhido->id) {
				v = edge->v2;
				escolhido = edge->v2;
				naoEscolhido = edge->v1;
			} else if (edge->v2->id == escolhido->id) {
				v = edge->v1;
				escolhido = edge->v1;
				naoEscolhido = edge->v2;
			} else {
				v = edge->v1;
				std::cout << " ERROR ? " << std::endl;
			}

			if (inicial == NULL)
				inicial = v;

			g_vertex_buffer_data[i] = v->point->x;
			g_vertex_buffer_data[i + 1] = v->point->y;
			g_vertex_buffer_data[i + 2] = v->point->z;
			g_color_buffer_data[i] = face->color->r;
			g_color_buffer_data[i + 1] = face->color->g;
			g_color_buffer_data[i + 2] = face->color->b;
			i += 3;
			nPoint += 1;

			if ((3 * nTriangle) == nPoint && nTriangle != 1) { // 6, 9, 12, ... (ultimo ponto dos triangulos depois do primeiro)
				g_vertex_buffer_data[i] = inicial->point->x;
				g_vertex_buffer_data[i + 1] = inicial->point->y;
				g_vertex_buffer_data[i + 2] = inicial->point->z;
				g_color_buffer_data[i] = face->color->r;
				g_color_buffer_data[i + 1] = face->color->g;
				g_color_buffer_data[i + 2] = face->color->b;
				i += 3;
				nPoint += 1;

				// std::cout << " F " << inicial->id << std::endl;
			}
			if (nPoint == ((3 * nTriangle) + 1) && numberOfEdgesInTheFace != 3 && nTriangle < numberOfTriangles) { // 4, 7, 10, ... (primeiro de cada triangulo depois do primeiro)
				g_vertex_buffer_data[i] = v->point->x;
				g_vertex_buffer_data[i + 1] = v->point->y;
				g_vertex_buffer_data[i + 2] = v->point->z;
				g_color_buffer_data[i] = face->color->r;
				g_color_buffer_data[i + 1] = face->color->g;
				g_color_buffer_data[i + 2] = face->color->b;
				i += 3;
				nPoint += 1;
				nTriangle += 1;

				// std::cout << " I " << v->id << std::endl;
			}
		
		}
	}

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW); // float 4 bits

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), g_color_buffer_data, GL_STATIC_DRAW);

	return length / 3;
}



int main() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Loading a custom object file", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
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

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 2, 4), // Camera pos
		glm::vec3(0, 0, 0), // origem
		glm::vec3(0, 1, 0)  // Head is up 
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	Mesh mesh1 = Mesh();
	mesh1.init("mushroom.off");

	int nTriangles = loadMesh(mesh1);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

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
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Draw the triangles
		glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
    
}