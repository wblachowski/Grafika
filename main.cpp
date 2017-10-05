#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shader/Shader.h>
#include "Camera.h"
#include "Model.h"
#include <iostream>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void create_cube_map(const char* front,const char* back,
	const char* top,const char* bottom,
	const char* left,const char* right,
	GLuint* tex_cube);
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name);
void AddBullet(glm::vec4 pos,float angleVertical ,float angleHorizontal, float speed);
// settings
unsigned int SCR_WIDTH = 1440;
unsigned int SCR_HEIGHT = 900;
float Yaw = -90.0f;
const float MOV_SPEED = 250.0f;
//camera
Camera camera(glm::vec3(0.0f, 90.0f, 180.0f));
//timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
//lightning-lamp
glm::vec3 LightPos(106.4f, 66.0f, -400.0f);
//mouse
float lastX = 400, lastY = 300;
bool firstMouse = true;
//bullets
struct Bullet
{
	glm::vec4 pos;
	glm::vec3 speed;
	glm::mat4 model;
	float angle;
	float timeCreated;
	bool exist;
};
const int MaxBullets = 1000;
Bullet BulletContainer[MaxBullets];
int BulletsNumber = 0;
//model matrices
glm::mat4 model;
glm::mat4 modelUpper;
glm::mat4 modelTower;
glm::mat4 modelSurface;
glm::mat4 modelTree;
glm::mat4 modelHouse;
glm::mat4 modelSky;
glm::mat4 modelLamp;
glm::mat4 modelStreetLamp;
glm::mat4 modelBullet;
//vectors
glm::vec3 tankDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 towerDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraOffset = glm::vec3(0.0f, -90.0f, -180.0f);
glm::vec3 cameraMouseOffset = cameraOffset;
glm::vec3 mouseDirection = tankDirection;
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Tank", glfwGetPrimaryMonitor(), NULL);
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = mode->height;
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//skybox
	float points[] = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f,  10.0f
	};
	unsigned int vboSky,vaoSky;
	glGenVertexArrays(1, &vaoSky);
	glGenBuffers(1, &vboSky);
	glBindVertexArray(vaoSky);
	glBindBuffer(GL_ARRAY_BUFFER, vboSky);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), &points, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(3);
	//surface
	float vertices[] = {
		100.0f,  0.0f, 100.0f,  0.0f, 1.0f, 0.0f,		500.0f, 500.0f,	// top right
		100.0f, 0.0f, -100.0f,   0.0f, 1.0f, 0.0f,		500.0f, 0.0f,// bottom right
		-100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f, // bottom left
		-100.0f, 0.0f, -100.0f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f,// bottom left
		-100.0f, 0.0f, 100.0f, 0.0f, 1.0f, 0.0f,		0.0f, 500.0f,		// top left
		100.0f,  0.0f, 100.0f,  0.0f, 1.0f, 0.0f,		500.0f, 500.0f// top right
	};
		
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//lamp
	float verticesLamp[] = {
		//tyl
		-0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,
		0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,
		0.6f,  0.5f, -0.6f,  0.0f,  0.0f, -1.0f,
		0.6f,  0.5f, -0.6f,  0.0f,  0.0f, -1.0f,
		-0.6f,  0.5f, -0.6f,  0.0f,  0.0f, -1.0f,
		-0.25f, -0.5f, -0.25f,  0.0f,  0.0f, -1.0f,
		//przod
		-0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,
		0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,
		0.6f,  0.5f,  0.6f,  0.0f,  0.0f,  1.0f,
		0.6f,  0.5f,  0.6f,  0.0f,  0.0f,  1.0f,
		-0.6f,  0.5f,  0.6f,  0.0f,  0.0f,  1.0f,
		-0.25f, -0.5f,  0.25f,  0.0f,  0.0f,  1.0f,
		//lewa
		-0.6f,  0.5f,  0.6f, -1.0f,  0.0f,  0.0f,
		-0.6f,  0.5f, -0.6f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.5f, -0.25f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.5f, -0.25f, -1.0f,  0.0f,  0.0f,
		-0.25f, -0.5f,  0.25f, -1.0f,  0.0f,  0.0f,
		-0.6f,  0.5f,  0.6f, -1.0f,  0.0f,  0.0f,
		//prawa
		0.6f,  0.5f,  0.6f,  1.0f,  0.0f,  0.0f,
		0.6f,  0.5f, -0.6f,  1.0f,  0.0f,  0.0f,
		0.25f, -0.5f, -0.25f,  1.0f,  0.0f,  0.0f,
		0.25f, -0.5f, -0.25f,  1.0f,  0.0f,  0.0f,
		0.25f, -0.5f,  0.25f,  1.0f,  0.0f,  0.0f,
		0.6f,  0.5f,  0.6f,  1.0f,  0.0f,  0.0f,
		//dol
		-0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,
		0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,
		0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,
		0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.5f,  0.25f,  0.0f, -1.0f,  0.0f,
		-0.25f, -0.5f, -0.25f,  0.0f, -1.0f,  0.0f,
		//gora
		-0.6f,  0.5f, -0.6f,  0.0f,  1.0f,  0.0f,
		0.6f,  0.5f, -0.6f,  0.0f,  1.0f,  0.0f,
		0.6f,  0.5f,  0.6f,  0.0f,  1.0f,  0.0f,
		0.6f,  0.5f,  0.6f,  0.0f,  1.0f,  0.0f,
		-0.6f,  0.5f,  0.6f,  0.0f,  1.0f,  0.0f,
		-0.6f,  0.5f, -0.6f,  0.0f,  1.0f,  0.0f
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBOlamp;
	glGenBuffers(1, &VBOlamp);
	glBindBuffer(GL_ARRAY_BUFFER, VBOlamp);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLamp), verticesLamp, GL_STATIC_DRAW);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBOlamp);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(4);
	// build and compile shaders
	// -------------------------
	Shader ourShader("shader.vs", "shader.fs");
	Shader skyShader("skyShader.vs", "skyShader.fs");
	Shader lampShader("lampShader.vs", "lampShader.fs");
	Shader simpleDepthShader("depthShader.vs", "depthShader.fs");
	Shader debugDepthQuad("depthQuadShader.vs", "depthQuadSHader.fs");
	// load models
	// -----------
	Model upperModel = Model("Sprut/upperBody2.obj");
	Model lowerModel = Model("Sprut/lower.obj");
	Model towerModel = Model("Sprut/tower1.obj");
	Model treeModel = Model("Tree/Tree.obj");
	Model houseModel = Model("Farmhouse/farmhouse_obj.obj");
	Model streetLampModel = Model("streetlamp/Models/streetlamp.obj");
	Model bulletModel = Model("Bullet/Bullet.obj");
	//load sky
	unsigned int textureSky;
	create_cube_map("sky/nz.jpg", "sky/pz.jpg", "sky/py.jpg", "sky/ny.jpg", "sky/nx.jpg", "sky/px.jpg",&textureSky);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureSky);
	//load grass texture
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load("grass.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	//shadows
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024*5, SHADOW_HEIGHT = 1024*5;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//shader configuration
	ourShader.use();
	ourShader.setInt("diffuseTexture", 0);
	ourShader.setInt("shadowMap", 1);
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);
	// lighting info
	// -------------
	glm::vec3 lightPos(-100.0f, 100.0f, 100.0f);
	// render loop
	// -----------

	model = glm::scale(model, glm::vec3(0.2f));	// it's a bit too big for our scene, so scale it down
	modelTree = glm::translate(modelTree, glm::vec3(100.0f, 0.0f, 0.0f));
	modelTree= glm::scale(modelTree, glm::vec3(50.0f));
	modelHouse = glm::translate(modelHouse, glm::vec3(0.0f, 0.0f, -400.0f));
	modelHouse = glm::scale(modelHouse, glm::vec3(6.0f));
	modelStreetLamp = glm::translate(modelStreetLamp, glm::vec3(120.0f, 0.0f, -400.0f));
	modelStreetLamp = glm::scale(modelStreetLamp, glm::vec3(7.0f));
	modelSurface = glm::scale(modelSurface, glm::vec3(100.0f));	//grass has now width and height of 20.000'
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		processInput(window);

		// render
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 2000.0f;
		lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f,near_plane , far_plane);
		lightPos=camera.Position+glm::vec3(-500.0f, 500.0f, 500.0f);
		glm::vec3 target = lightPos + glm::vec3(1.0f, -1.0f, -1.0f);
		lightView = glm::lookAt(lightPos, target, glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		//1 first render to depthmap
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//renderScene(simpleDepthShader);
			simpleDepthShader.setMat4("model", modelSurface);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			simpleDepthShader.setMat4("model", modelUpper);
			upperModel.Draw(simpleDepthShader);
			simpleDepthShader.setMat4("model", model);
			lowerModel.Draw(simpleDepthShader);
			simpleDepthShader.setMat4("model", modelTower);
			towerModel.Draw(simpleDepthShader);

			for (int i = 0; i < BulletsNumber; i++) {
				Bullet& bullet = BulletContainer[i];
				if (bullet.exist)
				{
					simpleDepthShader.setMat4("model", bullet.model);
					bulletModel.Draw(simpleDepthShader);
				}
			}

			simpleDepthShader.setMat4("model", modelTree);
			treeModel.Draw(simpleDepthShader);
			simpleDepthShader.setMat4("model", modelHouse);
			houseModel.Draw(simpleDepthShader);
			simpleDepthShader.setMat4("model", modelStreetLamp);
			streetLampModel.Draw(simpleDepthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		//skybox
		modelSky[3] = glm::vec4(camera.Position, 1.0f);
		glDepthMask(GL_FALSE);
		skyShader.use();
		skyShader.setMat4("projection", projection);
		skyShader.setMat4("view", view);
		skyShader.setMat4("model", modelSky);
		glBindVertexArray(vaoSky);
		glActiveTexture(GL_TEXTURE0 + textureSky);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureSky);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);

		// set light uniforms
		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setVec3("lightPos", lightPos);
		ourShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		ourShader.use();
		ourShader.setInt("material.diffuse", 0);
		ourShader.setInt("material.specular", 1);
		ourShader.setFloat("material.shininess", 32.0f);
		ourShader.setVec3("viewPos", camera.Position);
		//point light
		ourShader.setVec3("pointLight.position", LightPos);
		ourShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLight.diffuse", 20.8f, 20.8f, 20.8f);
		ourShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLight.constant", 1.0f);
		ourShader.setFloat("pointLight.linear", 0.022);
		ourShader.setFloat("pointLight.quadratic", 0.0019);

			ourShader.setMat4("model", modelSurface);
			glBindVertexArray(VAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			ourShader.setMat4("model", modelUpper);
			upperModel.Draw(ourShader);
			ourShader.setMat4("model", model);
			lowerModel.Draw(ourShader);
			ourShader.setMat4("model", modelTower);
			towerModel.Draw(ourShader);

			ourShader.setMat4("model", modelTree);
			treeModel.Draw(ourShader);

			ourShader.setMat4("model", modelHouse);
			houseModel.Draw(ourShader);

			ourShader.setMat4("model", modelStreetLamp);
			streetLampModel.Draw(ourShader);
			
			for (int i = 0; i < BulletsNumber; i++) {
				Bullet& bullet = BulletContainer[i];
				if (bullet.exist)
				{
					ourShader.setMat4("model", bullet.model);
					bulletModel.Draw(ourShader);
					bullet.angle -= 35.0f*deltaTime;
					if (bullet.angle > -90.0f)
					{
						bullet.model = glm::rotate(bullet.model, glm::radians(-35.0f*deltaTime), glm::vec3(0.0f, 0.0f, 1.0f));
					}
					bullet.model = glm::translate(bullet.model, bullet.speed);
					bullet.speed += glm::vec3(0.0f, -9.81f, 0.0f)*0.4f*deltaTime;
					if (bullet.model[3].y < -5.0f) {
						bullet.exist= false;
					}
				}
			}

			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);
			modelLamp = glm::mat4();
			modelLamp = glm::translate(modelLamp, LightPos);
			modelLamp = glm::scale(modelLamp, glm::vec3(8.0f)); // a smaller cube
			lampShader.setMat4("model", modelLamp);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}
float angleVertical = 0.0f;
float angleHorizontal = 0.0f;
float bulletSpeed = 0.0f;
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	glm::mat4 unity;
	float velocity = MOV_SPEED * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -velocity));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, velocity));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		velocity *= 0.66f;
		angleHorizontal += velocity;
		model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f));
		towerDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(towerDirection, 1.0f);
		tankDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(tankDirection, 1.0f);
		cameraOffset = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraOffset, 1.0f);

	}if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		velocity *= 0.66f;
		angleHorizontal -= velocity;
		model = glm::rotate(model, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f));
		towerDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::vec4(towerDirection, 1.0f);
		tankDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::vec4(tankDirection, 1.0f);
		cameraOffset = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::vec4(cameraOffset, 1.0f);
	}

	modelUpper = unity;
	modelUpper = glm::scale(modelUpper, glm::vec3(0.2f));
	modelUpper[3] = model[3] + glm::vec4(0.0f, 28.0f, 0.0f, 0.0f);
	modelUpper = glm::rotate(modelUpper, glm::radians(angleHorizontal), glm::vec3(0.0f, 1.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.Position = glm::vec3(model[3]) - cameraOffset;
		camera.Center = tankDirection + camera.LookDown;
		cameraMouseOffset = cameraOffset;
		mouseDirection = tankDirection;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.Position = glm::vec3(model[3]) - glm::vec3(-1*cameraOffset.x,cameraOffset.y,-1*cameraOffset.z);
		camera.Center = glm::vec3(-1*tankDirection.x,tankDirection.y,-1*tankDirection.z) + camera.LookDown;
		cameraMouseOffset = glm::vec3(-1 * cameraOffset.x, cameraOffset.y, -1 * cameraOffset.z);
		mouseDirection = glm::vec3(-1 * tankDirection.x, tankDirection.y, -1 * tankDirection.z);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS) {
		velocity *= 0.66f;
		angleHorizontal += velocity;
		modelUpper = glm::rotate(modelUpper, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f));
		towerDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(towerDirection, 1.0f);

		modelTower = modelUpper;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS) {
		velocity *= 0.66f;
		angleHorizontal -= velocity;
		modelUpper = glm::rotate(modelUpper, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f));
		towerDirection = glm::rotate(unity, glm::radians(velocity), glm::vec3(0.0f, -1.0f, 0.0f)) * glm::vec4(towerDirection, 1.0f);

		modelTower = modelUpper;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS) {
		velocity *= 0.66f/3.0f;
		angleVertical += velocity;

	}
	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS) {
		velocity *= 0.66f / 3.0f;
		angleVertical -= velocity;

	}
	modelTower = unity;
	modelTower = glm::scale(modelTower, glm::vec3(0.2f));
	modelTower[3] = modelUpper[3];
	modelTower[3] += glm::vec4(towerDirection*22.0f, 0.0f);
	modelTower[3] += glm::vec4(0.0f, 1.5f, 0.0f, 0.0f);
	if (angleVertical < -2.5f) {
		angleVertical = -2.5f;
	}
	if (angleVertical > 40.0f) {
		angleVertical = 40.0f;
	}
	modelTower = glm::rotate(modelTower, glm::radians(angleHorizontal), glm::vec3(0.0f, 1.0f, 0.0f));
	modelTower = glm::rotate(modelTower, glm::radians(angleVertical), glm::vec3(1.0f, 0.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS) {
		bulletSpeed += 100.0f*deltaTime;
		if (bulletSpeed > 200.0f)
		{
			bulletSpeed = 200.0f;
		}
	}
	else {
		if(bulletSpeed>0) {
			AddBullet(modelTower[3], angleVertical, angleHorizontal,bulletSpeed+20.0f);
		}
		bulletSpeed = 0.0f;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;

	lastX = xpos;

	//camera.ProcessMouseMovement(xoffset, yoffset);
	xoffset *= 0.1f;
		glm::mat4 unity;
		glm::mat4 unity2;
		cameraMouseOffset = glm::rotate(unity2, glm::radians(xoffset), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(cameraMouseOffset, 1.0f);
		mouseDirection = glm::rotate(unity, glm::radians(xoffset), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(mouseDirection, 1.0f);
		camera.Position = glm::vec3(model[3]) - cameraMouseOffset;
		camera.Center = mouseDirection + camera.LookDown;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);

}
void create_cube_map(
	const char* front,
	const char* back,
	const char* top,
	const char* bottom,
	const char* left,
	const char* right,
	GLuint* tex_cube) {
	// generate a cube-map texture to hold all the sides
	//glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex_cube);

	// load each image and copy into a side of the cube-map texture
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, top);
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottom);
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	load_cube_map_side(*tex_cube, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	// format cube map texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
bool load_cube_map_side(GLuint texture, GLenum side_target, const char* file_name) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int x, y, n;
	int force_channels = 4;
	unsigned char*  image_data = stbi_load(
		file_name, &x, &y, &n, force_channels);
	if (!image_data) {
		fprintf(stderr, "ERROR: could not load %s\n", file_name);
		return false;
	}
	// non-power-of-2 dimensions check
	if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
		fprintf(stderr,
			"WARNING: image %s is not power-of-2 dimensions\n",
			file_name);
	}

	// copy image data into 'target' side of cube map
	glTexImage2D(
		side_target,
		0,
		GL_RGBA,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data);
	free(image_data);
	return true;
}
void AddBullet(glm::vec4 pos, float angleVertical,float angleHorizontal, float speed) {
	Bullet bullet;
	bullet.pos = pos;
	bullet.angle = angleVertical;
	bullet.speed = glm::vec3(speed, 0.0f, 0.0f);
	bullet.exist = true;
	bullet.timeCreated = lastFrame;
	bullet.model[3] = pos;
	bullet.model = glm::scale(bullet.model, glm::vec3(0.1f));
	bullet.model = glm::rotate(bullet.model, glm::radians(angleHorizontal), glm::vec3(0.0f, 1.0f, 0.0f));
	bullet.model = glm::rotate(bullet.model, glm::radians(angleVertical), glm::vec3(1.0f, 0.0f, 0.0f));
	bullet.model = glm::rotate(bullet.model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	bullet.model = glm::translate(bullet.model, glm::vec3(750.0f, 20.0f, 0.0f));

	BulletContainer[BulletsNumber] = bullet;
	BulletsNumber++;
}