#include "Model.h"
#include "FBO.h"
#include "Floor.h"
#include "Skybox.h"
#include "PerlinTexture.h"
#include "MarbleTexture.h"
#include "WoodTexture.h"
#include "CellularTexture.h"
#include "Cubemap.h"
#include <random>


GLuint width = 1440;
GLuint height = 810;
float prevTime = 0.0f;
float crntTime = 0.0f;
float dt = 0.0f;



int main(void) 
{
	// Initialize GLFW
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	// Tell GLFW what version of OpenGL we are using 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "Dolphin", NULL, NULL);
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	// Specify the viewport of OpenGL in the Window

	glViewport(0, 0, width, height);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	// TEXTURES
	
	// BLUE _ GREEN
	GLuint octaves = 6;
	double falloff = 0.5;
	const GLuint size = 256 / 1;
	double veinWidth = 0.5;
	double veinFrequency = 7.6;
	double distPower = 1.2;
	glm::vec3 baseColor(1.0, 0.6, 0.4);
	glm::vec3 veinColor(0.0, 0.7, 1.0);
	//MarbleTexture marbleTex(size, octaves, falloff, veinWidth, veinFrequency, distPower, baseColor, veinColor);
	//MarbleTexture blackGoldMarble(ssize, octaves, falloff, veinWidth, veinFrequency, distPower, glm::vec3(1.0, 0.8, 0.5), glm::vec3(0.1, 0.1, 0.1));
	MarbleTexture blueVeinsMarble(size, octaves, falloff, veinWidth, veinFrequency, distPower, 
		glm::vec3(0.20, 0.28, 0.22), glm::vec3(0.01, 0.18, 0.25));
	//MarbleTexture greenMarble(size, octaves, falloff, veinWidth, veinFrequency, distPower,
	//	glm::vec3(0.0, 0.3, 0.2), glm::vec3(0.0, 1.0, 0.5));
	
	
	// WHITE_RED
	octaves = 7; 
	falloff = 0.6;
	veinWidth = 1.6;
	veinFrequency = 1.8;
	distPower = 2.9;
	baseColor = glm::vec3(0.9, 0.7, 0.7);
	veinColor = glm::vec3(0.4, 0.0, 0.1);

	MarbleTexture redMarble(size, octaves, falloff, veinWidth, veinFrequency, distPower,
		baseColor, veinColor);
	
	
	// GOLD
	octaves = 7; 
	falloff = 0.6;
	veinWidth = 1.8;
	veinFrequency = 1.6;
	distPower = 2.9;
	baseColor = glm::vec3(0.8549, 0.6471, 0.1255);
	veinColor = glm::vec3(0.01, 0.01, 0.05);

	MarbleTexture goldMarble(size, octaves, falloff, veinWidth, veinFrequency, distPower,
		baseColor, veinColor);
	// WOOD
	double ringWidth = 0.5;
	double ringFrequency = 25.0;
	distPower = 0.3;
	falloff = 0.25;
	octaves = 6;
	WoodTexture woodTexture(size, octaves, falloff, ringWidth, ringFrequency, distPower,
		glm::vec3(0.5f, 0.35f, 0.15f), glm::vec3(0.26f, 0.15f, 0.0f));


	// backpack
	Model dolphin("dolphin/dolphin.obj");
	Floor floor(10.0f, "planks.png", "planksSpec.png");
	//Model model("cat/cat.obj");
	ShaderProgram shader("default.vert", "default.frag");
	ShaderProgram shader3D("model.vert", "model.frag");
	ShaderProgram dissolveShader("model.vert", "modelDissolve.frag");
	ShaderProgram goldShader("gold.vert", "gold.frag");
	ShaderProgram skyboxShader("skybox.vert", "skybox.frag");
	ShaderProgram dirShadowShader("directionalShadowMap.vert", "directionalShadowMap.frag");

	glm::vec3 floorPos = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::mat4 floorModel = glm::mat4(1.0f);
	floorModel = glm::translate(floorModel, floorPos);
	float radius = 2.0f;
	const int modelNumber = 5;
	float scale = 0.01f;
	glm::vec3 pos;
	std::vector<glm::mat4> model;
	std::vector<glm::mat3> transposeInv;
	float shininess[modelNumber];
	for (int i = 0; i < modelNumber; i++)
	{
		pos = glm::vec3(radius * cos(glm::radians(360.0 / modelNumber * float(i))), 0.0f, radius * sin(glm::radians(360.0 / modelNumber * float(i))));
		glm::mat4 m = glm::mat4(1.0f);
		model.push_back(glm::translate(m, pos));
		model[i] = glm::scale(model[i], glm::vec3(scale));
		model[i] = glm::rotate(model[i], glm::radians(45.0f + float(i) * 30.0f), glm::vec3(0.0, 1.0, 0.0));
		model[i] = glm::rotate(model[i], glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		
		transposeInv.push_back(transpose(inverse(glm::mat3(model[i]))));
	}
	shininess[0] = 0.9f;
	shininess[1] = 0.9f;
	shininess[2] = 0.9f;
	shininess[3] = 0.2f;
	shininess[4] = 0.2f;
	
	 
	Camera camera(width, height, glm::vec3(4.0f, 0.0f, 0.0f));
	camera.SetDir(glm::vec3(-1.0f, 0.0f, 0.0f));


	// SHADOW

	unsigned int shadowWidth = 1024 * 4, shadowHeight = 1024 * 4;

	FBO shadowFBO(shadowWidth, shadowHeight);
	GLuint shadowTexUnit = 2;
	shadowFBO.GenTexture();
	shadowFBO.AttachDepth();
	shadowFBO.DisableColor();

	glm::mat4 orthogonalProjection = glm::ortho(-6.0f, 6.0f, -6.0f, 6.0f, 0.2f, 10.0f);
	glm::mat4 lightView = glm::lookAt(8.0f * glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 dirLightVP = orthogonalProjection * lightView;
	
	shadowFBO.Bind();
	shadowFBO.Viewport();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthFunc(GL_LESS);

	dirShadowShader.Activate();
	dirShadowShader.setMat4("lightVP", dirLightVP);
	for (int i = 0; i < modelNumber; i++)
	{
		dirShadowShader.setMat4("model", model[i]);
		dolphin.Draw(dirShadowShader, camera);
	}
	
	// SKYBOX
	Skybox skybox("skybox", 0);
	// CUBEMAP
	GLuint cubemapSize = 2048;
	glm::vec3 position = glm::vec3(model[modelNumber - 2][3][0], 
		model[modelNumber - 2][3][1], model[modelNumber - 2][3][2]);
	Cubemap cubemap(GL_COLOR_ATTACHMENT0, cubemapSize, position);
	cubemap.GenRBO();
	cubemap.Bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < 6; i++)
	{
		cubemap.SetFaceToDraw(i);

		glDisable(GL_DEPTH_TEST);
		skyboxShader.Activate();
		skybox.Draw(skyboxShader, cubemap.GetCamera());
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		shader.Activate();
		shader.setVec3("camDir", camera.GetDirection());
		shader.setMat4("model", floorModel);
		shader.setBool("flashLight", camera.GetFlashlight());
		shader.setMat4("dirLightVP", dirLightVP);
		shader.setInt("shadowMap", 2);
		glActiveTexture(GL_TEXTURE2);
		shadowFBO.BindTex2D();
		floor.Draw(shader, cubemap.GetCamera());
		shader3D.Activate();
		shader3D.setMat4("camMatrix", camera.GetMatrix());
		shader3D.setInt("tex3D", 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		for (int i = 0; i < modelNumber - 2; i++)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			if (i == 0)
				blueVeinsMarble.Bind();
			else if (i == 1)
				woodTexture.Bind();
			else if(i == 2)
				redMarble.Bind();

			glm::mat3 transposeInverse = glm::transpose(glm::inverse(glm::mat3(model[i])));
			shader3D.setMat3("transposeInverse", transposeInverse);
			shader3D.setMat4("model", model[i]);
			shader3D.setFloat("shininess", shininess[i]);
			dolphin.Draw(shader3D, cubemap.GetCamera());
			GLcheckError();
		}
	}

	// MAIN LOOP
	int counter = 0;
	while (!glfwWindowShouldClose(window)) 
	{
		prevTime = crntTime;
		crntTime = glfwGetTime();
		dt = crntTime - prevTime;

		// SHADOW
		shadowFBO.Bind();
		shadowFBO.Viewport();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);

		dirShadowShader.Activate();
		dirShadowShader.setMat4("lightVP", dirLightVP);
		dirShadowShader.setBool("dissolve", false);
		dirShadowShader.setFloat("time", crntTime / 5.0);
		dirShadowShader.setInt("dissTex", 3);
		glActiveTexture(GL_TEXTURE0 + 3);
		blueVeinsMarble.Bind();

		for (int i = 0; i < modelNumber; i++)
		{
			if(i == modelNumber - 1)
				dirShadowShader.setBool("dissolve", true);
			dirShadowShader.setMat4("model", model[i]);
			dolphin.Draw(dirShadowShader, camera);
		}
		// PRIMARY
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		skyboxShader.Activate();
		skyboxShader.setInt("skybox", 0);
		skybox.Draw(skyboxShader, camera);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		

		
		camera.updateMatrix(45.0f, 0.1f, 100.0f);
		camera.Input(window, dt);
		float angle = (crntTime - prevTime) * 10.0;

		shader.Activate();
		shader.setVec3("camDir", camera.GetDirection());
		shader.setMat4("model", floorModel);
		shader.setBool("flashLight", camera.GetFlashlight());
		shader.setMat4("dirLightVP", dirLightVP);
		shader.setInt("shadowMap", 2);
		glActiveTexture(GL_TEXTURE2);
		shadowFBO.BindTex2D();
		floor.Draw(shader, camera);
		shader3D.Activate();
		shader3D.setMat4("camMatrix", camera.GetMatrix());
		shader3D.setInt("tex3D", 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		for (int i = 0; i < modelNumber - 2; i++)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			if (i == 0)
				blueVeinsMarble.Bind();
			else if (i == 1)
				woodTexture.Bind();
			else
				redMarble.Bind();
			
			model[i] = glm::rotate(model[i], glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
			glm::mat3 transposeInverse = glm::transpose(glm::inverse(glm::mat3(model[i])));
			shader3D.setMat3("transposeInverse", transposeInverse);
			shader3D.setMat4("model", model[i]);
			shader3D.setFloat("shininess", shininess[i]);
			dolphin.Draw(shader3D, camera);
			GLcheckError();
		}
		// Dissolve
		dissolveShader.Activate();
		dissolveShader.setMat4("camMatrix", camera.GetMatrix());
		dissolveShader.setInt("tex3D", 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		woodTexture.Bind();
		dissolveShader.setInt("dissTex", 3);
		glActiveTexture(GL_TEXTURE0 + 3);
		blueVeinsMarble.Bind();
		dissolveShader.setMat4("model", model[modelNumber - 1]);
		dissolveShader.setMat3("transposeInverse", transposeInv[modelNumber - 1]);
		dissolveShader.setVec3("camPos", camera.GetPosition());
		dissolveShader.setFloat("shininess", shininess[modelNumber - 1]);
		dissolveShader.setFloat("time", crntTime / 5.0);
		dolphin.Draw(dissolveShader, camera);
		// Gold
		goldShader.Activate();
		goldShader.setMat4("camMatrix", camera.GetMatrix());
		goldShader.setVec3("camPos", camera.GetPosition());
		model[modelNumber - 2] = glm::rotate(model[modelNumber - 2], glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
		glm::mat3 transposeInverse = glm::transpose(glm::inverse(glm::mat3(model[modelNumber - 2])));
		goldShader.setMat4("model", model[modelNumber - 2]);
		goldShader.setMat3("transposeInverse", transposeInverse);
		goldShader.setInt("tex3D", 2);
		glActiveTexture(GL_TEXTURE0 + 2);
		goldMarble.Bind();
		goldShader.setInt("cubemap", 3);
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.GetTexID());
		dolphin.Draw(goldShader, camera);
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}
	shader.Delete();
	dirShadowShader.Delete();
	shader3D.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}



