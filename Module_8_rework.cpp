#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/ext.hpp>

#include "SOIL2/SOIL2.h"

using namespace std;

#define WINDOW_TITLE "Modern OpenGl"

#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

GLint cubeShaderProgram, lampShaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, CubeVAO, LightVAO, texture;

/* Cube Data */
GLfloat vertices[] = {
		/* Position    |   Texture Coordinates */

		// front
		 -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, // V0
		  0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, // V0
		  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // V0

		// right
		 0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // V0
		 0.0f, 0.5f,  0.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, // V0
		 0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // V0

		// back
		 -0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, // V0
		  0.0f, 0.5f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, // V0
		  0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // V0

		// left
		 -0.5f, 0.0f,  0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // V0
		  0.0f, 0.5f,  0.0f, 0.5f, 1.0f, -1.0f, 0.0f, 0.0f, // V0
		 -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, // V0

		// bottom
		 -0.5f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // V0
		  0.5f, 0.0f, -0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, // V0
		 -0.5f, 0.0f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // V0
		 -0.5f, 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, // V0
		  0.5f, 0.0f,  0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, // V0
		  0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, // V0
};
glm::vec3 cubePosition(0.0f, 0.0f, 0.0f);
glm::vec3 cubeScale(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(0.6f, 0.5f, 0.75f);

/* Light Data */
glm::vec3 lightPositions[] = {
		glm::vec3(0.5f, 0.5f, -3.0f), // Light-Zero
		glm::vec3(0.5f, 1.5f, 50.0f) // Light-One
};
glm::vec3 lightColor[] = {
		glm::vec3(1.0f, 2.0f, 1.0f), // Light-Zero
		glm::vec3(1.0f, 1.0f, 50.0f) // Light-One
};

float constant = 1.0f, linear = 0.9, quadratic = 0.032;
glm::vec3 lightScale(0.3f), specular(1.0f, 1.0f, 1.0f), diffuse(0.8f, 0.8f, 0.8f), ambient(0.05f, 0.05f, 0.05f);

/* Camera Data */
bool ortho = false;
GLfloat cameraSpeed = 0.05f, zoom = 45.0f;
glm::vec3 cameraPosition(0.0f, 0.5f, -2.0f), front;
glm::vec3 CameraUpY = glm::vec3(0.0f, 0.5f, 0.0f), CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f);

/* Input Data */
bool mouseDetected = true, leftMouse = true, rightMouse = true;
GLfloat lastMouseX = 400, lastMouseY = 300;
GLfloat mouseXOffset, mouseYOffset, yaw= 0.0f, pitch = 0.0f;
GLfloat sensitivity = 0.005f;
GLchar currentKey, keyMod, mouseClick;

/* Functions */

/* Core */
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);

/* Input */
void UKeyboard(unsigned char key, int x, int y);
void UMouseClick(int button, int state, int x, int y);
void UMouseCameraDefault(int x, int y);
void UMouseCameraAdvanced(int x, int y);

/* Services */
void lampCreator(int i);
void shaderErrorHandler(GLint shaders[]);

/* Cube Vertex Shader */
const GLchar * cubeVertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 textureCoordinates;

	out vec3 Normal;
	out vec3 FragmentPos;
	out vec2 TexCoords;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		FragmentPos = vec3(model * vec4(position, 1.0f));
		Normal = mat3(transpose(inverse(model))) * normal;
		TexCoords = textureCoordinates;

		gl_Position = projection * view * model * vec4(position, 1.0f);
	}

);

/* Cube Fragment Shader */
const GLchar * cubeFragmentShaderSource = GLSL(330,

	out vec4 cubeColor;
	out vec4 gpuTexture;

	struct Light {
		vec3 position;
		vec3 ambient;
		vec3 diffuse;
		vec3 specular;
		vec3 color;

		float constant;
		float linear;
		float quadratic;
	};

	in vec3 Normal;
	in vec3 FragmentPos;
	in vec2 TexCoords;

	uniform vec3 viewPosition;
	uniform vec3 objectColor;
	uniform Light light[];
	uniform sampler2D uTexture;

	vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 FragmentPos, sampler2D uTexture);

	void main() {

		vec3 result;
		float attenuation;

		/* Ambient Lighting */
		float ambientStrength = 0.1f;

		/* Diffuse Lighting */
		vec3 norm = normalize(Normal);
		vec3 viewDir = normalize(viewPosition - FragmentPos);

		for(int i = 0; i < 2; i++) {
			result += CalcDirLight(light[i], norm, viewDir, FragmentPos, uTexture);
		}

		/* Phong Result */
		vec3 phong = result * objectColor;
		cubeColor = vec4(phong, 1.0f);

		gpuTexture = texture(uTexture, TexCoords);

	}

	vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 FragmentPos, sampler2D uTexture) {
		vec3 lightDirection = normalize(light.position - FragmentPos);
	    vec3 lightDir = normalize(-lightDirection);
	    // diffuse shading
	    float diff = max(dot(normal, lightDir), 0.0);
	    // specular shading
	    vec3 reflectDir = reflect(-lightDir, normal);
	    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 16.0f);
	    // combine results
	    vec3 ambient = light.ambient * vec3(texture(uTexture, TexCoords));
	    vec3 diffuse = light.diffuse * light.color * diff * vec3(texture(uTexture, TexCoords));
	    vec3 specular = light.specular * spec * vec3(texture(uTexture, TexCoords));
	    return (ambient + diffuse + specular);
	}

);

const GLchar * lampVertexShaderSource = GLSL(330,
	layout (location = 0) in vec3 position;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f);
	}
);

const GLchar * lampFragmentShaderSource = GLSL(330,
	out vec4 color;
	void main() {
		color = vec4(1.0f);
	}
);

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);
	glutReshapeFunc(UResizeWindow);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Failed" << std::endl;
		return -1;
	}

	UCreateShader();
	UCreateBuffers();
	UGenerateTexture();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glutDisplayFunc(URenderGraphics);
	glutKeyboardFunc(UKeyboard);
	glutPassiveMotionFunc(UMouseCameraDefault);
	glutMotionFunc(UMouseCameraAdvanced);
	glutMouseFunc(UMouseClick);

	glutMainLoop();

	/* Clean Up */
	glDeleteVertexArrays(1, &CubeVAO);
	glDeleteVertexArrays(1, &LightVAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

void UResizeWindow(int w, int h) {
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

void URenderGraphics(void) {

	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLint modelLoc, viewLoc, projLoc, objectColorLoc, viewPositionLoc;

	int size = sizeof(vertices);

	glm::mat4 model, view, projection;
	CameraForwardZ = front;

	glUseProgram(cubeShaderProgram);
	glBindVertexArray(CubeVAO);

	/* Transform Object */
	model = glm::translate(model, cubePosition);
	model = glm::rotate(model, glutGet(GLUT_ELAPSED_TIME) * -0.00005F, glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, cubeScale);

	/* Transform Camera */
	view = glm::lookAt(cameraPosition - CameraForwardZ, cameraPosition + CameraForwardZ, CameraUpY);

	/* Set Camera Projection */
	projection = glm::perspective(zoom, (GLfloat) WindowWidth / (GLfloat) WindowHeight, 0.1f, 100.0f);

	/* Reference Matrix From Cube Shader */
	modelLoc = glGetUniformLocation(cubeShaderProgram, "model");
	viewLoc = glGetUniformLocation(cubeShaderProgram, "view");
	projLoc = glGetUniformLocation(cubeShaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	/* Reference Matrix for Cube Light */
	objectColorLoc = glGetUniformLocation(cubeShaderProgram, "objectColor");
	viewPositionLoc = glGetUniformLocation(cubeShaderProgram, "viewPosition");

	/* Pass Lights Data to Cube Shader */
	glUniform3f(objectColorLoc, objectColor.r, objectColor.g, objectColor.b);
	glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	/* Light One */
	lampCreator(0);
	glDrawArrays(GL_TRIANGLES, 0, size);
	/* Light Two */
	lampCreator(1);
	glDrawArrays(GL_TRIANGLES, 0, size);

	glBindVertexArray(0);

	/* Draws Lights */
	glUseProgram(lampShaderProgram);
	glBindVertexArray(LightVAO);

	for (int i = 0; i < 2; ++i) {
		model = glm::translate(model, lightPositions[i]);
		model = glm::scale(model, lightScale);
		modelLoc = glGetUniformLocation(lampShaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, size);
	}

	viewLoc = glGetUniformLocation(lampShaderProgram, "view");
	projLoc = glGetUniformLocation(lampShaderProgram, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	/* Posts Results */
	glutPostRedisplay();
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, size);
	glBindVertexArray(0);
	glutSwapBuffers();

}

void UCreateShader() {

	/* Cube Shader */
	GLint cubeVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(cubeVertexShader, 1, &cubeVertexShaderSource, NULL);
	glCompileShader(cubeVertexShader);

	GLint cubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(cubeFragmentShader, 1, &cubeFragmentShaderSource, NULL);
	glCompileShader(cubeFragmentShader);

	cubeShaderProgram = glCreateProgram();
	glAttachShader(cubeShaderProgram, cubeVertexShader);
	glAttachShader(cubeShaderProgram, cubeFragmentShader);
	glLinkProgram(cubeShaderProgram);

	/* Light Shader */
	GLint lampVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(lampVertexShader, 1, &lampVertexShaderSource, NULL);
	glCompileShader(lampVertexShader);

	GLint lampFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(lampFragmentShader, 1, &lampFragmentShaderSource, NULL);
	glCompileShader(lampFragmentShader);

	lampShaderProgram = glCreateProgram();
	glAttachShader(lampShaderProgram, lampVertexShader);
	glAttachShader(lampShaderProgram, lampFragmentShader);
	glLinkProgram(lampShaderProgram);

	GLint shaders[] = {
			cubeVertexShader,
			cubeFragmentShader,
			lampVertexShader,
			lampFragmentShader
	};

	shaderErrorHandler(shaders);

	/* Cleanup */
	glDeleteShader(cubeVertexShader);
	glDeleteShader(cubeFragmentShader);
	glDeleteShader(lampVertexShader);
	glDeleteShader(lampFragmentShader);

}

void UCreateBuffers() {

	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(CubeVAO);
	/* Positions */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0);

	/* Normals */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	/* Texture Coordinates */
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &LightVAO);
	glBindVertexArray(LightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

}

void UGenerateTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int width, height;
	unsigned char* image = SOIL_load_image("snhu.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* Input Functions */
void UKeyboard(unsigned char key, GLint x, GLint y) {
	switch (key) {
		case 'w':
			cameraPosition += cameraSpeed * CameraForwardZ;
			break;
		case 's':
			cameraPosition -= cameraSpeed * CameraForwardZ;
			break;
		case 'a':
			cameraPosition -= glm::normalize(glm::cross(CameraForwardZ, CameraUpY));
			break;
		case 'd':
			cameraPosition += glm::normalize(glm::cross(CameraForwardZ, CameraUpY));
			break;
	}
}

void UMouseClick(int button, int state, int x, int y) {
		leftMouse = button == GLUT_LEFT_BUTTON  ? true : false;
		rightMouse = button == GLUT_RIGHT_BUTTON  ? true : false;
}

/* First Person Camera */
void UMouseCameraDefault(int x, int y) {
	if (mouseDetected) {
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;
	lastMouseX = x;
	lastMouseY = y;
	mouseXOffset *= 0.5f;
	mouseYOffset *= 0.5f;
	yaw += mouseXOffset;
	pitch += mouseYOffset;
	pitch = pitch >  89.0f ?  89.0f : pitch;
	pitch = pitch < -89.0f ? -89.0f : pitch;
	glm::vec3 direction;
	direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	direction.y = sin(glm::radians(pitch));
	direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	front = normalize(direction);
}

/* Orbit Camera */
void UMouseCameraAdvanced(int x, int y) {
	if (mouseDetected) {
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y;
	lastMouseX = x;
	lastMouseY = y;
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;
	yaw += mouseXOffset;
	pitch += mouseYOffset;
	if (rightMouse && GLUT_ACTIVE_ALT == glutGetModifiers()) {
		front.x = 10.0f * cos(yaw);
		front.y = 10.0f * sin(pitch);
		front.z = sin(yaw) * cos(pitch) * 10.0f;
	}
}

/* Extra Functions */
void lampCreator(int i) {

	GLint lightColorLoc, lightPositionLoc, ambientLoc, diffuseLoc, specularLoc, constantLoc, linearLoc, quadraticLoc;
	string light = "light[" + to_string(i) + "]";

	lightPositionLoc = glGetUniformLocation(cubeShaderProgram, (light + ".position").c_str());
	lightColorLoc = glGetUniformLocation(cubeShaderProgram, (light + ".color").c_str());
	ambientLoc = glGetUniformLocation(cubeShaderProgram, (light + ".ambient").c_str());
	diffuseLoc = glGetUniformLocation(cubeShaderProgram, (light + ".diffuse").c_str());
	specularLoc = glGetUniformLocation(cubeShaderProgram, (light + ".specular").c_str());
	constantLoc = glGetUniformLocation(cubeShaderProgram, (light + ".specular").c_str());
	linearLoc = glGetUniformLocation(cubeShaderProgram, (light + ".specular").c_str());
	quadraticLoc = glGetUniformLocation(cubeShaderProgram, (light + ".specular").c_str());

	glUniform3f(lightPositionLoc, lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
	glUniform3f(lightColorLoc, lightColor[i].r, lightColor[i].g, lightColor[i].b);
	glUniform3f(ambientLoc, ambient.x, ambient.y, ambient.z);
	glUniform3f(diffuseLoc, diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(specularLoc, specular.x, specular.y, specular.z);
	glUniform1f(constantLoc, constant);
	glUniform1f(linearLoc, linear);
	glUniform1f(quadraticLoc, quadratic);
}

void shaderErrorHandler(GLint shaders[]) {
	for (int i = 0; i <= 3; ++i) {
		int result;

		glGetShaderiv(shaders[i], GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) {
			int length;
			glGetShaderiv(shaders[i], GL_INFO_LOG_LENGTH, &length);
			char *message = (char*) alloca(length * sizeof(char));
			glGetShaderInfoLog(shaders[i], length, &length, message);
			std::cout << "Failed to compile cube fragment shader" << std::endl;
			std::cout << message << std::endl;
		}
	}
}
