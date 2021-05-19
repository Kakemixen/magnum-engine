#include "application.h"
#include <Corrade/Containers/GrowableArray.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Timeline.h>

namespace Magnum
{

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

void Primitive::draw(const Matrix4& transformationMatrix, SceneGraph::Camera3D& camera)
{
    m_shader
        .setDiffuseColor(m_color)
        .setLightPositions({{13.0f, 2.0f, 5.0f, 0.0f}})
        .setTransformationMatrix(transformationMatrix)
        .setNormalMatrix(transformationMatrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .draw(m_mesh);
}

Primitive& Primitive::updatePosition(Vector3 newPosition){
    m_position = newPosition;
    return (*this); // method chaining
}

const Vector3& Primitive::getPosition() const {
    return m_position;
}

ApplicationLayer::ApplicationLayer(const Arguments& arguments):
    Platform::Application(arguments, Configuration{}
            .setTitle("basic engine"))
{
	using namespace Math::Literals;
    m_cameraObject = new Object3D{&m_scene};
    (*m_cameraObject) //All scenes need camera
        .setParent(&m_scene)
        .translate(Vector3::zAxis(5.0f));
    m_camera  = new SceneGraph::Camera3D{*m_cameraObject};
    (*m_camera)
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    // base object for easy manipulation
    m_manipulator.setParent(&m_scene);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    m_shader
        .setAmbientColor(0x111111_rgbf)
        .setSpecularColor(0xffffff_rgbf)
        .setShininess(80.0f);

    timeline.start();
}

Float ApplicationLayer::getFrameDelta(){
    return timeline.previousFrameDuration();
}

Primitive* ApplicationLayer::addCube(Vector3 initialPosition, float color){
    using namespace Math::Literals;
    Trade::MeshData cube = Primitives::cubeSolid();
    Primitive* p = new Primitive(m_manipulator, m_drawables, cube, m_shader, initialPosition, 
            Magnum::Color3::fromHsv({Deg(color), 1.0f, 1.0f}));
    p->translate(initialPosition);
    return p;
}

void ApplicationLayer::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    m_camera->draw(m_drawables); // draw the list of drawables

    swapBuffers();
    timeline.nextFrame();
}

void ApplicationLayer::mousePressEvent(MouseEvent& event) {
    if(event.button() != MouseEvent::Button::Left) return;

    m_previousMousePosition = event.position();
    event.setAccepted();
}

void ApplicationLayer::mouseMoveEvent(MouseMoveEvent& event) {
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    /* We have to take window size, not framebuffer size, since the position is
       in window coordinates and the two can be different on HiDPI systems */
    const Vector2 delta = 3.0f*
        Vector2{event.position() - m_previousMousePosition}/
        Vector2{windowSize()};

    (*m_cameraObject)
        .rotate(Rad{-delta.y()}, m_cameraObject->transformation().right().normalized())
        .rotateY(Rad{-delta.x()});

    m_previousMousePosition = event.position();
    event.setAccepted();
}

void ApplicationLayer::keyPressEvent(KeyEvent& event) {
    if(event.key() == KeyEvent::Key::D) {
        m_cameraObject->translate({.0f, .0f, -0.1f});
    } else if(event.key() == KeyEvent::Key::S) {
        m_cameraObject->translate({.0f, .0f, 0.1f});
    } else if (event.key() == KeyEvent::Key::R) {
        m_cameraObject->translate({-0.1f, .0f, .0f});
    } else if (event.key() == KeyEvent::Key::T) {
        m_cameraObject->translate({0.1f, .0f, .0f});
    } else if (event.key() == KeyEvent::Key::W) {
        m_cameraObject->translate({.0f, -0.1f, .0f});
    } else if (event.key() == KeyEvent::Key::B) {
        m_cameraObject->translate({.0f, 0.1f, .0f});
    }
}

void ApplicationLayer::keyReleaseEvent(KeyEvent& event) {
    if(event.key() == KeyEvent::Key::W || event.key() == KeyEvent::Key::S ||
       event.key() == KeyEvent::Key::A || event.key() == KeyEvent::Key::D)
        return;
}

} /* Magnum */ 

