#include <GL/glew.h>
#include "renderer.hpp"
#include "GameObjects/gameObject.hpp"
#include "Logs/logs.hpp"
#include "openglErrors.hpp"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <iostream>

namespace ph {

void Renderer::init()
{
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK)
		PH_EXIT_GAME("GLEW wasn't initialized correctly!");

	/*GLCheck( const GLubyte* openglVersionInfo = glGetString(GL_VERSION) );
	std::cout << "OpenGL version: " << openglVersionInfo << std::endl;*/

	// set up blending
	GLCheck( glEnable(GL_BLEND) );
	GLCheck( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );

	// load default shaders
	auto& sl = ShaderLibrary::getInstance();
	sl.loadFromFile("perfectPixel", "resources/shaders/staticPixelPerfect.vs.glsl", "resources/shaders/texture.fs.glsl");
	sl.loadFromFile("dynamic", "resources/shaders/default.vs.glsl", "resources/shaders/texture.fs.glsl");
	mRendererData.mDefaultShader = sl.get("perfectPixel");
}

void Renderer::beginScene(Camera& camera)
{
	GLCheck( glClear(GL_COLOR_BUFFER_BIT) );

	mSceneData.mViewProjectionMatrix = camera.getViewProjectionMatrix4x4().getMatrix();
	
	const sf::Vector2f center = camera.getCenter();
	const sf::Vector2f size = camera.getSize();
	mSceneData.mScreenBounds = FloatRect(center.x - size.x / 2, center.y - size.y / 2, size.x, size.y);
}

void Renderer::submit(VertexArray& vao, Shader& shader, const sf::Transform& transform, const sf::Vector2i size, DrawPrimitive drawMode)
{
	if(!isInsideScreen(transform, size))
		return;

	vao.bind();

	shader.bind();
	shader.setUniformMatrix4x4("modelMatrix", transform.getMatrix());
	shader.setUniformMatrix4x4("viewProjectionMatrix", mSceneData.mViewProjectionMatrix);
	
	GLCheck( glDrawElements(toGLEnum(drawMode), vao.getIndexBuffer().mNumberOfIndices, GL_UNSIGNED_INT, 0) );
	
	++mRendererData.mNumberOfDrawCalls;
}

void Renderer::submit(VertexArray& vao, const sf::Transform& transform, const sf::Vector2i size, DrawPrimitive drawMode)
{
	submit(vao, *mRendererData.mDefaultShader, transform, size, drawMode);
}

void Renderer::submit(Sprite& sprite, Shader& shader, const sf::Transform& transform, DrawPrimitive drawMode)
{
	sprite.mTexture.bind();
	submit(sprite.mVertexArray, shader, transform, sprite.mSize, drawMode);
}

void Renderer::submit(Sprite& sprite, const sf::Transform& transform, DrawPrimitive drawMode)
{
	submit(sprite, *mRendererData.mDefaultShader, transform, drawMode);
}

bool Renderer::isInsideScreen(const sf::Transform& transform, const sf::Vector2i size)
{
	const FloatRect objectRect(transform.getMatrix()[12], transform.getMatrix()[13], static_cast<float>(size.x), static_cast<float>(size.y));
	return mSceneData.mScreenBounds.doPositiveRectsIntersect(objectRect);
}

void Renderer::endScene()
{
	std::cout << "DCPF: " << mRendererData.mNumberOfDrawCalls << std::endl;
	mRendererData.mNumberOfDrawCalls = 0;
}

void Renderer::onWindowResize(unsigned width, unsigned height)
{
	GLCheck( glViewport(0, 0, width, height) );
}

void Renderer::setClearColor(const sf::Color& color)
{
	GLCheck( glClearColor(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f) );
}

}
