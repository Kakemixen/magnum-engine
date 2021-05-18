#include "application.h"
#include <Corrade/Containers/GrowableArray.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Primitives/Icosphere.h>
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
    Matrix4 thisTransform = transformationMatrix;
    thisTransform.translation() = m_position;
    m_shader
        .setDiffuseColor(m_color)
        .setLightPositions({
                {camera.cameraMatrix().transformPoint({-3.0f, 10.0f, 10.0f}), 0.0f}
        })
        //.setTransformationMatrix(transformationMatrix)
        //.setNormalMatrix(transformationMatrix.normalMatrix())
        .setTransformationMatrix(thisTransform)
        .setNormalMatrix(thisTransform.normalMatrix())
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
    m_cameraObject //All scenes need camera
        .setParent(&m_scene)
        .translate(Vector3::zAxis(5.0f));
    (*(m_camera = new SceneGraph::Camera3D{m_cameraObject}))
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

    /*
    new Primitive(m_manipulator, m_drawables, sphere, m_shader, {0.f, 0.f, -4.f}, 
            Magnum::Color3::fromHsv({35.0_degf, 1.0f, 1.0f}));
    new Primitive(m_manipulator, m_drawables, sphere, m_shader, {2.f, 1.f, -6.f}, 
            Magnum::Color3::fromHsv({75.0_degf, 1.0f, 1.0f}));
    */

    timeline.start();
}

Float ApplicationLayer::getFrameDelta(){
    return timeline.previousFrameDuration();
}

Primitive* ApplicationLayer::addSphere(Vector3 initialPosition, float color){
    using namespace Math::Literals;
    Trade::MeshData sphere = Primitives::icosphereSolid(2);
    return new Primitive(m_manipulator, m_drawables, sphere, m_shader, initialPosition, 
            Magnum::Color3::fromHsv({Deg(color), 1.0f, 1.0f}));
}

void ApplicationLayer::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    m_camera->draw(m_drawables); // draw the list of drawables

    swapBuffers();
    timeline.nextFrame();
}

} /* Magnum */ 

