#include "../headers/SpriteRenderer.h"

SpriteRenderer* SpriteRenderer::instance = 0;

/**
 * Get the instance of the renderer. If the renderer is yet to be used the object is initialized.
 *
 * @return instance a pointer to the SpriteRenderer instance.
 */
SpriteRenderer* SpriteRenderer::getInstance()
{
	if (instance == 0)
	{
		instance = new SpriteRenderer();
	}

	return instance;
}

void SpriteRenderer::moveCamera(glm::vec2 position) {
	//remove arbitrary numbers
	cameraPosition.x = position.x - globals::width/2;
	cameraPosition.y = position.y - globals::height/2;
}

/**
 * Constructor fot the spreiteRenderer used to initialize a new sprite shader on the GPU.
 *
 */
SpriteRenderer::SpriteRenderer()
{
	spriteShader.initialize("Shaders/Shader.vert", "Shaders/Shader.frag");
	cameraPosition = glm::vec2(0.0f, 0.0f);
	spriteShader.SetMatrix(glm::ortho(0.0f, globals::width,globals::height, 0.0f, 0.1f, 100.0f), "projection");

	GLfloat vertices[] = {
-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //2nd 2 on each line are for texture co-ords
1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
-1.0f,1.0f,0.0f, 1.0f,0.0f,
	};

	GLuint indices[] = {
		0,3,1,
		1,2,3
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

/**
 * Render a sprite object to screen.
 *
 * @param sprite A pointer to the sprites Texture object.
 * @param position A 3 value vector for the world position of the sprite.
 * @param size A 3 value vector for the world size of the sprite.
 * @param rotation the rotation in radians of the sprite.
 * @param rotationOffset this is used to alter the pivot point of the object used in rotations.
 */
void SpriteRenderer::Render()
{
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// Model used for manipulation of positioning of object on screen
	glm::mat4 model = glm::mat4(1.0f);
	while (!messageQueue.empty())
	{
		drawCall currentSprite = messageQueue.front();
		messageQueue.erase(messageQueue.begin());

		model = glm::translate(model, glm::vec3(currentSprite.position.x - cameraPosition.x + currentSprite.rotationOffset.y, +currentSprite.position.y - cameraPosition.y, -1.0));

		model = glm::rotate(model, currentSprite.rotation, glm::vec3(0.0f, 0.0f, 1.0f));

		model = glm::translate(model, glm::vec3(currentSprite.rotationOffset, -1.0));

		model = glm::scale(model, glm::vec3(currentSprite.size.x, currentSprite.size.y, 1));

		spriteShader.SetMatrix(model, "model");

		currentSprite.sprite->UseTexture();

		GLuint lastSprite = 0;
		if (spriteShader.shaderID != lastSprite)
		{
			glUseProgram(spriteShader.shaderID);
			lastSprite = spriteShader.shaderID;
		}

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//reset the model
		model = glm::mat4(1.0f);
	}


}

void SpriteRenderer::DrawMessage(Texture * sprite, glm::vec2 position, glm::vec2 size, float rotation, glm::vec2 rotationOffset) {
	drawCall newMsg = { sprite,position,size,rotation,rotationOffset };
	messageQueue.push_back(newMsg);
}



SpriteRenderer::~SpriteRenderer()
{}
