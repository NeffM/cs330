//Michael Neff
// R Resets The Image
// Left click allows for free movement 
// WASD keys work as should
// Q amd E keys work as should


#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include <SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;

// Declare Input Callback Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

// Declare View Matrix
glm::mat4 viewMatrix;

// Camera Field of View
GLfloat fov = 45.0f;

GLfloat cameraSpeed = 2.0f; // Adjust the default camera movement speed
GLfloat orbitSpeed = 0.1f; // Adjust the default orbit speed 
bool isOrthographic = false;


void initiateCamera();
// Define Camera Attributes
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // Move 3 units back in z towards screen
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f); // What the camera points to
glm::vec3 cameraDirection = glm::normalize(cameraPosition - target); // direction z
glm::vec3 worldUp = glm::vec3(0.0, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));// right vector x
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight)); // up vector y
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 1 unit away from lense



// Camera Transformation Prototype
void TransformCamera();

// Boolean array for keys and mouse buttons
bool keys[1024], mouseButtons[3];

// Input state booleans
bool isPanning = false, isOrbiting = false;

// Pitch and Yaw
GLfloat radius = 3.0f, rawYaw = 0.0f, rawPitch = 0.0f, degYaw, degPitch;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat lastX = 320, lastY = 240, xChange, yChange; // Center mouse cursor
bool firstMouseMove = true;

// light source position added in light mod
glm::vec3 lightPosition(2.0f, 1.0f, 1.f); // x y z cords to move light 




// Draw Primitive(s)
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 16;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}

void draw2()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 200;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}


// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}

// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}


int main(void)
{
	width = 640; height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;

	GLfloat vertices[] = {

		// Triangle 1
		-0.5, -0.5, 0.0, // index 0
		1.0, 0.0, 0.0, // red
		0.0, 0.0, // UV (bl)
		0.0f, 0.0f, 1.0f,

		-0.5, 0.5, 0.0, // index 1
		0.0, 1.0, 0.0, // green
		0.0, 1.0, // UV (br)
		0.0f, 0.0f, 1.0f,

		0.5, -0.5, 0.0,  // index 2	
		0.0, 0.0, 1.0, // blue
		1.0, 0.0, // UV (tl)
		0.0f, 0.0f, 1.0f,

		// Triangle 2	
		0.5, 0.5, 0.0,  // index 3	
		1.0, 0.0, 1.0, // purple
		1.0, 1.0, // UV (tr)
		0.0f, 0.0f, 1.0f
	};

	// for lamp 
	GLfloat lampVertices[] = {
			-0.5, -0.5, 0.0, // index 0
			-0.5, 0.5, 0.0, // index 1
			0.5, -0.5, 0.0,  // index 2	
			0.5, 0.5, 0.0  // index 3	
	};


	GLfloat candleBaseVertices[] = {

		0.0f, 0.0f, 0.0f,  // vert 0

		0.1f, 0.0f, 0.2f, // vert 1

		-0.1f, 0.0f, 0.2f, // vert 2



		-0.1f, -0.1f, 0.2f, // vert 3

		0.0f, -0.1f, 0.2f, // vert 4

		0.1f, -0.1f, 0.2f, // vert 5

	};


	GLfloat candleStickVertices[] = {

		0.0f, 0.0f, 0.0f,  // vert 0

		0.1f, 0.0f, 0.2f, // vert 1

		-0.1f, 0.0f, 0.2f, // vert 2



		-0.1f, -0.1f, 0.2f, // vert 3

		0.0f, -0.1f, 0.2f, // vert 4

		0.1f, -0.1f, 0.2f, // vert 5

	};



	// Define element indices
	GLubyte indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	// Define element indices
	GLubyte indices2[] = {
		0, 1, 2,
		2, 0, 1,

		2, 3, 1,
		3, 2, 1,

		1, 3, 5,
		5, 1, 3,

	};

	// Plane Transforms
	glm::vec3 planePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.5f),  //0
		glm::vec3(0.5f,  0.0f,  0.0f),  // 1
		glm::vec3(0.0f,  0.0f,  -0.5f), // 2
		glm::vec3(-0.5f, 0.0f,  0.0f), // 3
		glm::vec3(0.0f, 0.5f,  0.0f), // 4
		glm::vec3(0.0f, -0.49f,  0.0f) // 5
	};

	// Candle base transforms
	glm::vec3 candleBasePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),  // index 0
		// Add additional positions as needed
	};


	glm::float32 planeRotations[] = {
		0.0f, 90.0f, 180.0f, -90.0f, -90.f, 90.f
	};


	glm::float32 candleBaseRotations[] = {
		0.0f, 45.0f, 90.0f, -45.0f  // Add additional rotations as needed
	};

	glm::float32 triRotations[] = { 0.0f, 45.0f, 90.0f, 135.0f, 180.0f, 225.0f, 270.0f, 315.0f, };
	// Setup some OpenGL options

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode uncomment to enable
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	GLuint cubeVBO, cubeEBO, cubeVAO, floorVBO, floorEBO, floorVAO, lampVBO, lampEBO, lampVAO, candleVAO, candleVBO, candleEBO;

	glGenBuffers(1, &cubeVBO); // Create VBO
	glGenBuffers(1, &cubeEBO); // Create EBO

	glGenBuffers(1, &candleVBO); // Create VBO
	glGenBuffers(1, &candleEBO); // Create EBO

	glGenBuffers(1, &floorVBO); // Create VBO
	glGenBuffers(1, &floorEBO); // Create EBO

	glGenBuffers(1, &lampVBO); // Create VBO
	glGenBuffers(1, &lampEBO); // Create EBO

	glGenVertexArrays(1, &cubeVAO); // Create VOA
	glGenVertexArrays(1, &floorVAO); // Create VOA
	glGenVertexArrays(1, &lampVAO); // Create VOA
	glGenVertexArrays(1, &candleVAO); // Create VOA

	glBindVertexArray(cubeVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO); // Select EBO

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

	 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

	glBindVertexArray(floorVAO);

      glBindBuffer(GL_ARRAY_BUFFER, floorVBO); // Select VBO
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO); // Select EBO

	  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

	  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

	// lamp VAO has its own shader. 

		glBindVertexArray(lampVAO);

		glBindBuffer(GL_ARRAY_BUFFER, lampVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO); // Select EBO

		glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		// lamp VAO has its own shader. 
		glBindVertexArray(candleVAO);

		// VBO and EBO Placed in User-Defined VAO

		glBindBuffer(GL_ARRAY_BUFFER, candleVBO); // Select VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, candleEBO); // Select EBO

		glBufferData(GL_ARRAY_BUFFER, sizeof(candleBaseVertices), candleBaseVertices, GL_STATIC_DRAW); // Load vertex attributes
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW); // Load indices 

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);

		// Load texture maps
		int crateTexWidth, crateTexHeight, gridTexWidth, gridTexHeight;
		unsigned char* crateImage = SOIL_load_image("silver.png", &crateTexWidth, &crateTexHeight, 0, SOIL_LOAD_RGB);
		unsigned char* crateImage2 = SOIL_load_image("silver.png", &crateTexWidth, &crateTexHeight, 0, SOIL_LOAD_RGB);
		unsigned char* gridImage = SOIL_load_image("tabletopwhite.png", &gridTexWidth, &gridTexHeight, 0, SOIL_LOAD_RGB);

		// Generate Textures  
		GLuint crateTexture; // for texture ID
		glGenTextures(1, &crateTexture);// Generate texture id
		glBindTexture(GL_TEXTURE_2D, crateTexture); // Activate texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, crateTexWidth, crateTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, crateImage); // Generate texture
		glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
		SOIL_free_image_data(crateImage); // Free imge from memory
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

		// Generate Textures  
		GLuint crateTexture2; // for texture ID
		glGenTextures(1, &crateTexture2);// Generate texture id
		glBindTexture(GL_TEXTURE_2D, crateTexture2); // Activate texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, crateTexWidth, crateTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, crateImage2); // Generate texture
		glGenerateMipmap(GL_TEXTURE_2D); // Texture resolution managment
		SOIL_free_image_data(crateImage2); // Free imge from memory
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind or close texture object

		GLuint gridTexture;
		glGenTextures(1, &gridTexture);
		glBindTexture(GL_TEXTURE_2D, gridTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gridTexWidth, gridTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, gridImage);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(gridImage);
		glBindTexture(GL_TEXTURE_2D, 0);

	// Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 2) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;" // scale of the cube would allow to calculate if we change the cube 
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;" // added for light
		"uniform vec3 lightColor;"  // added for light 
		"uniform vec3 lightPos;" // added for light we have to tell open GL
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"//Ambient\n"
		"float ambientStrength = 0.3f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"//diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * lightColor;"
		"//specularity\n"
		"float specularStrength = 2.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 180);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular) * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"}\n";

	//for lamp
	string lampVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// for lamp 
	string lampFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(1.0f);" // rgb color changing light source will be color of white 
		"}\n";


	// for candle stick
	string candleStickVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// for lamp 
	string candleStickFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);" // White light source
		"}\n";

	// candle stick base
	string candleStickBaseVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string candleStickBaseFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);" // Dark gray light source
		"}\n";

	// candle stick base
	string candleStickBaseBotVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string candleStickBaseBotFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);" // black
		"}\n";

	// candle stick base
	string candleStickShaftVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string candleStickShaftFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);" // Dark gray light source
		"}\n";


	// candle stick base
	string candleStickShaftTopVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string candleStickShaftTopFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);" // Dark gray light source
		"}\n";

	// candle stick base
	string muffVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string muffFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(.454f, 0.149f, 0.027f, 1.0f);" // Dark red
		"}\n";

	// candle stick base
	string wineGlassVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"

		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"}\n";

	// Fragment shader source code
	string wineGlassFragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"out vec4 fragColor;"
		"uniform sampler2D myTexture;"
		"void main()\n"
		"{\n"
		"fragColor = vec4(0.901, 0.941f, 0.929f, 0.0f);" 
		"}\n";

	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource); // this handles the shaders inputted in seminar

	GLuint candleStickShaderProgram = CreateShaderProgram(candleStickVertexShaderSource, candleStickFragmentShaderSource);
	GLuint candleStickBaseShaderProgram = CreateShaderProgram(candleStickBaseBotVertexShaderSource, candleStickBaseBotFragmentShaderSource);
	GLuint candleStickShaftShaderProgram = CreateShaderProgram(candleStickShaftVertexShaderSource, candleStickShaftFragmentShaderSource);
	GLuint candleStickShaftTopShaderProgram = CreateShaderProgram(candleStickShaftTopVertexShaderSource, candleStickShaftTopFragmentShaderSource);


	GLuint wineGlassTopShaderProgram = CreateShaderProgram(wineGlassVertexShaderSource, wineGlassFragmentShaderSource);
	GLuint wineGlassStemShaderProgram = CreateShaderProgram(wineGlassVertexShaderSource, wineGlassFragmentShaderSource);
	GLuint wineGlassBowlShaderProgram = CreateShaderProgram(wineGlassVertexShaderSource, wineGlassFragmentShaderSource);

	GLuint rightEarMuffShaderProgram = CreateShaderProgram(muffVertexShaderSource, muffFragmentShaderSource);
	GLuint rightEarMuff2ShaderProgram = CreateShaderProgram(muffVertexShaderSource, muffFragmentShaderSource);
	GLuint rightEarMuff3ShaderProgram = CreateShaderProgram(muffVertexShaderSource, muffFragmentShaderSource);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes


		// Declare transformations (can be initialized outside loop)		
		glm::mat4 projectionMatrix;

		if (keys[GLFW_KEY_P] && !isOrthographic) {
			isOrthographic = true;
			projectionMatrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
		}

		if (keys[GLFW_KEY_O] && isOrthographic) {
			isOrthographic = false;
			projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		}

		// Define LookAt Matrix
		viewMatrix = glm::lookAt(cameraPosition, target, worldUp);

		// Define projection matrix
		projectionMatrix = glm::perspective(fov, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		// get light and object color location, and light position location added in light module 
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		
		// assign colors and light, remember the photo shop technique 

		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f); //  to get color divide by 255 for RGB 
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // light white

		// set light position 
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);

		//view camera position
		glUniform3f(viewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindTexture(GL_TEXTURE_2D, crateTexture); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(cubeVAO); // User-defined VAO must be called before draw. 

// laptop back
		// Transform planes to form cube
		glm::mat4 modelMatrix1;
		modelMatrix1 = glm::translate(modelMatrix1, planePositions[2]);
		modelMatrix1 = glm::rotate(modelMatrix1, planeRotations[2] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix1));
		draw();

		glBindTexture(GL_TEXTURE_2D, crateTexture2); // Apply crate texture (Auto detected by Uniform Sampler)
		glBindVertexArray(cubeVAO); // User-defined VAO must be called before draw. 

		std::vector<float> lapTopscreen = { 0.001f, -0.002f, -0.003f, -0.004f, -0.005f, -0.006f, -0.007f, 0.008f, -0.009f, -0.0010f, -0.0011f, };

		for (const auto& y : lapTopscreen) {
			glm::mat4 modelMatrix2;

			// Translating and rotating the laptop screen
			modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(planePositions[2].x, planePositions[2].y, planePositions[2].z + y));
			modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[2] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[2] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix2));

			draw(); // Render each laptop screen
		}

		// laptop 
		glm::mat4 modelMatrix2;
		modelMatrix2 = glm::translate(modelMatrix2, planePositions[5]);
		modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[5] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[5] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix2));
		draw();

		std::vector<float> lapTopBase = { -0.498f, -0.497f, -0.496f, -0.495f, -0.494f, -0.493f, -0.492f, -0.491f, -0.490f, -0.489f, -0.487, -0.486f, -0.485f, -0.484, -0.483f, -0.482f, -0.481, -0.480f, -0.479, -0.478f, -0.477f, -0.476 };

		for (const auto& y : lapTopBase) {
			glm::mat4 modelMatrix2;
			modelMatrix2 = glm::translate(modelMatrix2, glm::vec3(planePositions[5].x, y, planePositions[5].z));
			modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[5] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix2 = glm::rotate(modelMatrix2, planeRotations[5] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix2));
			draw(); // Assuming draw() is a function to render your 3D model
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glBindTexture(GL_TEXTURE_2D, gridTexture); // Apply grid texture
		// Select and transform floor
		glBindVertexArray(floorVAO);
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -.5f, 0.f));
		modelMatrix = glm::rotate(modelMatrix, 90.f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(10.f, 10.f, 10.f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		draw();
		glBindVertexArray(0); //Incase different VAO will be used after

		glUseProgram(0); // Incase different shader will be used after


		glUseProgram(lampShaderProgram);

			// Get matrix's uniform location and set matrix
			GLint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
			GLint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
			GLint lampProjLoc = glGetUniformLocation(lampShaderProgram, "projection");

			glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

			glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 

					// Transform planes to form cube
			for (GLuint i = 0; i < 6; i++)
			{
				glm::mat4 modelMatrix;
				modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(8., 8., 8.) + lightPosition);
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125));
				if (i >= 4)
					modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
				glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
				// Draw primitive(s)
				draw();
			}

			// Unbind Shader exe and VOA after drawing per frame
			glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(candleStickShaftShaderProgram);

		glm::vec3 candleStickShaftPosition(-1.0f, 0.2f, -0.5f); //moves candle stick 
// Get matrix's uniform location and set matrix
		GLint candle2ModelLoc = glGetUniformLocation(candleStickShaftShaderProgram, "model");
		GLint candle2ViewLoc = glGetUniformLocation(candleStickShaftShaderProgram, "view");
		GLint candle2ProjLoc = glGetUniformLocation(candleStickShaftShaderProgram, "projection");

		glUniformMatrix4fv(candle2ViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(candle2ProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + candleStickShaftPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 5.8f, 0.08f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(candle2ModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			draw2(); // Render each lamp component
		}


		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(candleStickShaftTopShaderProgram);

		glm::vec3 candleStickShaftTopPosition(-1.0f, 0.2f, -0.5f); //moves candle stick 
// Get matrix's uniform location and set matrix
		GLint candle3ModelLoc = glGetUniformLocation(candleStickShaftTopShaderProgram, "model");
		GLint candle3ViewLoc = glGetUniformLocation(candleStickShaftTopShaderProgram, "view");
		GLint candle3ProjLoc = glGetUniformLocation(candleStickShaftTopShaderProgram, "projection");

		glUniformMatrix4fv(candle3ViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(candle3ProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + candleStickShaftTopPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			modelMatrix = glm::scale(modelMatrix, glm::vec3(.25f, .5f, 0.3f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(candle3ModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			draw2(); // Render each lamp component
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(candleStickShaderProgram);

		glm::vec3 candleStickPosition(-1.0f, 0.75f, -0.5f); //moves candle stick 
// Get matrix's uniform location and set matrix
		GLint candleModelLoc = glGetUniformLocation(candleStickShaderProgram, "model");
		GLint candleViewLoc = glGetUniformLocation(candleStickShaderProgram, "view");
		GLint candleProjLoc = glGetUniformLocation(candleStickShaderProgram, "projection");

		glUniformMatrix4fv(candleViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(candleProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + candleStickPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 5.5f, 0.1f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(candleModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			draw2(); // Render each lamp component
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(candleStickBaseShaderProgram);
		glm::vec3 candleStickBasePosition(-1.0f, -0.4f, -0.5f); //moves candle stick base
		// Get matrix's uniform location and set matrix
		GLint candleStickBaseModelLoc = glGetUniformLocation(candleStickBaseShaderProgram, "model");
		GLint candleStickBaseViewLoc = glGetUniformLocation(candleStickBaseShaderProgram, "view");
		GLint candleStickBaseProjLoc = glGetUniformLocation(candleStickBaseShaderProgram, "projection");

		glUniformMatrix4fv(candleStickBaseViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(candleStickBaseProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw. 

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + candleStickBasePosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			//modelMatrix = glm::scale(modelMatrix, glm::vec3(.5f, .5f, .5));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(candleStickBaseModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			draw2(); // Render each lamp component
		}

		glUseProgram(wineGlassTopShaderProgram);

		glm::vec3 wineGlassTopPosition(1.0f, -0.48f, -0.5f); 
	// Get matrix's uniform location and set matrix
		GLint wineTopModelLoc = glGetUniformLocation(candleStickBaseShaderProgram, "model");
		GLint wineTopViewLoc = glGetUniformLocation(candleStickBaseShaderProgram, "view");
		GLint wineTopProjLoc = glGetUniformLocation(candleStickBaseShaderProgram, "projection");

		glUniformMatrix4fv(candleStickBaseViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(candleStickBaseProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw.

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 modelMatrix;
			modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + wineGlassTopPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			modelMatrix = glm::rotate(modelMatrix, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			modelMatrix = glm::scale(modelMatrix, glm::vec3(.8f, .2f, 0.9f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(wineTopModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

			draw2(); // Render each lamp component
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(wineGlassStemShaderProgram);

		glm::vec3 wineGlassStemPosition(1.0f, 0.15f, -0.5f);
		// Get matrix's uniform location and set matrix
		GLint wineGlassStemModelLoc = glGetUniformLocation(wineGlassStemShaderProgram, "model");
		GLint wineGlassStemViewLoc = glGetUniformLocation(wineGlassStemShaderProgram, "view");
		GLint wineGlassStemProjLoc = glGetUniformLocation(wineGlassStemShaderProgram, "projection");

		glUniformMatrix4fv(wineGlassStemViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(wineGlassStemProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw.

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 wineGlassStem;
			wineGlassStem = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + wineGlassStemPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			wineGlassStem = glm::rotate(wineGlassStem, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			wineGlassStem = glm::rotate(wineGlassStem, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			wineGlassStem = glm::scale(wineGlassStem, glm::vec3(.06f, 6.5f, 0.06f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(wineGlassStemModelLoc, 1, GL_FALSE, glm::value_ptr(wineGlassStem));

			draw2(); // Render each lamp component
		}


		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

	glUseProgram(0);

		glUseProgram(wineGlassBowlShaderProgram);

		glm::vec3 wineGlassBowlPosition(1.0f, 0.6f, -0.5f);
		// Get matrix's uniform location and set matrix
		GLint wineGlassBowlModelLoc = glGetUniformLocation(wineGlassBowlShaderProgram, "model");
		GLint wineGlassBowlViewLoc = glGetUniformLocation(wineGlassBowlShaderProgram, "view");
		GLint wineGlassBowlProjLoc = glGetUniformLocation(wineGlassBowlShaderProgram, "projection");

		glUniformMatrix4fv(wineGlassBowlViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(wineGlassBowlProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw.

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 wineGlassBowl;
			wineGlassBowl = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + wineGlassBowlPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			wineGlassBowl = glm::rotate(wineGlassBowl, glm::radians(0.f), glm::vec3(1.0f, 0.0f, 0.0f));
			wineGlassBowl = glm::rotate(wineGlassBowl, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			wineGlassBowl = glm::scale(wineGlassBowl, glm::vec3(.8f, 5.5f, 0.8f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(wineGlassBowlModelLoc, 1, GL_FALSE, glm::value_ptr(wineGlassBowl));

			draw2(); // Render each lamp component
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(rightEarMuffShaderProgram);

		glm::vec3 rightEarMuffPosition(-0.9f, -0.25f, 0.5f);
		// Get matrix's uniform location and set matrix
		GLint rightEarMuffModelLoc = glGetUniformLocation(rightEarMuffShaderProgram, "model");
		GLint rightEarMuffViewLoc = glGetUniformLocation(rightEarMuffShaderProgram, "view");
		GLint rightEarMuffProjLoc = glGetUniformLocation(rightEarMuffShaderProgram, "projection");

		glUniformMatrix4fv(rightEarMuffViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(rightEarMuffProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw.

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 rightEarMuff;
			rightEarMuff = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + rightEarMuffPosition);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			rightEarMuff = glm::rotate(rightEarMuff, glm::radians(55.f), glm::vec3(1.0f, 0.0f, 0.0f));
			rightEarMuff = glm::rotate(rightEarMuff, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			rightEarMuff = glm::scale(rightEarMuff, glm::vec3(.9f, 1.0f, 0.9f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(rightEarMuffModelLoc, 1, GL_FALSE, glm::value_ptr(rightEarMuff));

			draw2(); // Render each lamp component
		}


		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		glUseProgram(rightEarMuff2ShaderProgram);

		glm::vec3 rightEarMuff2Position(-1.1f, -0.33f, 0.0f);
		// Get matrix's uniform location and set matrix
		GLint rightEarMuff2ModelLoc = glGetUniformLocation(rightEarMuff2ShaderProgram, "model");
		GLint rightEarMuff2ViewLoc = glGetUniformLocation(rightEarMuff2ShaderProgram, "view");
		GLint rightEarMuff2ProjLoc = glGetUniformLocation(rightEarMuff2ShaderProgram, "projection");

		glUniformMatrix4fv(rightEarMuff2ViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(rightEarMuff2ProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(candleVAO); // User-defined VAO must be called before draw.

		// Transform planes to form cube
		for (GLuint i = 0; i < 9; i++)
		{
			glm::mat4 rightEarMuff2;
			rightEarMuff2 = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0) + rightEarMuff2Position);
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			//test below
			rightEarMuff2 = glm::rotate(rightEarMuff2, glm::radians(117.f), glm::vec3(1.0f, 0.0f, 0.0f));
			rightEarMuff2 = glm::rotate(rightEarMuff2, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.5f, 0.0f));

			// end test

			rightEarMuff2 = glm::scale(rightEarMuff2, glm::vec3(.9f, 1.0f, 0.9f));
			//modelMatrix = glm::rotate(modelMatrix, candleBaseRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(rightEarMuff2ModelLoc, 1, GL_FALSE, glm::value_ptr(rightEarMuff2));

			draw2(); // Render each lamp component
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Poll Camera Transformations
		TransformCamera();
	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteBuffers(1, &floorEBO);


	glfwTerminate();
	return 0;
}

// Define input functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Display ASCII Key code
	//std::cout <<"ASCII: "<< key << std::endl;	

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Assign true to Element ASCII if key pressed
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE) // Assign false to Element ASCII if key released
		keys[key] = false;

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	// Adjust the camera speed based on the mouse scroll
	cameraSpeed += yoffset * 0.1f;

	// Clamp camera speed to avoid extreme values
	cameraSpeed = glm::clamp(cameraSpeed, 0.1f, 10.0f);

	// Adjust the orbit speed based on the mouse scroll
	orbitSpeed += yoffset * 0.01f;

	// Clamp orbit speed to avoid extreme values
	orbitSpeed = glm::clamp(orbitSpeed, 0.01f, 1.0f);

	// Print camera and orbit speeds if needed
	// cout << "Camera Speed: " << cameraSpeed << ", Orbit Speed: " << orbitSpeed << endl;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouseMove)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouseMove = false;
    }

    // Calculate mouse offset
    xChange = xpos - lastX;
    yChange = lastY - ypos; // Inverted cam

    // Get current mouse position
    lastX = xpos;
    lastY = ypos;

    if (isOrbiting)
    {
        // Update raw yaw and pitch with mouse movement
        rawYaw += xChange;
        rawPitch += yChange;

        // Convert yaw and pitch to degrees, and clamp pitch
        degYaw = glm::radians(rawYaw);
        degPitch = glm::clamp(glm::radians(rawPitch), -glm::pi<float>() / 2.f + .1f, glm::pi<float>() / 2.f - .1f);

        // Azimuth Altitude formula
        cameraPosition.x = target.x + radius * cosf(degPitch) * sinf(degYaw);
        cameraPosition.y = target.y + radius * sinf(degPitch);
        cameraPosition.z = target.z + radius * cosf(degPitch) * cosf(degYaw);

        // Set up your view matrix using glm::lookAt
        glm::mat4 viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));

        // Use the viewMatrix in your rendering pipeline
        // glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	// Assign boolean state to element Button code
	if (action == GLFW_PRESS)
		mouseButtons[button] = false;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = true;
}

// Define TransformCamera function
void TransformCamera() {
	// Pan camera
	float deltaTimeSpeed = deltaTime * cameraSpeed;

	if (keys[GLFW_KEY_W]) {
		cameraPosition += cameraFront * deltaTimeSpeed;
	}
	if (keys[GLFW_KEY_S]) {
		cameraPosition -= cameraFront * deltaTimeSpeed;
	}
	if (keys[GLFW_KEY_A]) {
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * deltaTimeSpeed;
	}
	if (keys[GLFW_KEY_D]) {
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * deltaTimeSpeed;
	}

	// Lift camera
	if (keys[GLFW_KEY_Q]) {
		cameraPosition -= cameraUp * deltaTimeSpeed;
	}
	if (keys[GLFW_KEY_E]) {
		cameraPosition += cameraUp * deltaTimeSpeed;
	}

	// Orbit camera
	if (keys[GLFW_KEY_LEFT_ALT] && mouseButtons[GLFW_MOUSE_BUTTON_LEFT]) {
		isPanning = true;
	}
	else {
		isPanning = false;
	}

	// Orbit camera
	if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT]) {
		isOrbiting = true;
	}
	else {
		isOrbiting = false;
	}

	// Reset camera
	if (keys[GLFW_KEY_R]) {
		initiateCamera();
	}

	// Adjust the orbit speed
	rawYaw += xChange * orbitSpeed;
	rawPitch += yChange * orbitSpeed;

}

// Define 
void initiateCamera()
{	// Define Camera Attributes
	cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f); // Move 3 units back in z towards screen
	target = glm::vec3(0.0f, 0.0f, 0.0f); // What the camera points to
	cameraDirection = glm::normalize(cameraPosition - cameraDirection); // direction z
	worldUp = glm::vec3(0.0, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));// right vector x
	cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight)); // up vector y
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 1 unit away from lense
}