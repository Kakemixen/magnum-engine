#include <Corrade/Containers/GrowableArray.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Primitives/Icosphere.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Timeline.h>

#include <iostream>
#include <vector>

namespace Magnum
{

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class Primitive: public SceneGraph::Drawable3D
{
public:
    explicit Primitive(Object3D& object, 
            SceneGraph::DrawableGroup3D& group,
            const Trade::MeshData& primitive,
            Shaders::Phong& shader,
            Vector3 position,
            const Color4& color)
        : SceneGraph::Drawable3D{object, &group}, 
          m_mesh(MeshTools::compile(primitive)), 
          m_shader(shader),
          m_position(position),
          m_color{color} 
    {}

    Primitive& updatePosition(Vector3 newPosition);
    const Vector3& getPosition() const;

private:
    void draw(const Matrix4& transformationMatrixm, SceneGraph::Camera3D& camera) override;

    GL::Mesh m_mesh;
    Shaders::Phong& m_shader;
    Vector3 m_position;
    Color4 m_color;
};

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


class ApplicationLayer: public Platform::Application {
public:
    explicit ApplicationLayer(const Arguments& arguments);

    Primitive* addSphere(Vector3 initialPosition, float color);

    double getFrameDelta();

protected:
    void drawEvent() override; // Called everytime the screen is (re)drawn
    //void exitEvent(ExitEvent &event) override; // Called when <C-C>
    // bool mainLoopIteration(); // Called in a loop until program should close
    /* So let's talk a bit about the mainLoopIteration() function.
     * By default, it calls drawEvent() and glfwPollEvents() within some sensible calls to ensure stuff works.
     * Of importance, only call drawEvent() when the draw flag is set,
     * and check if it should close by checking glfwWindowShouldClose(m_window).
     * While this technically allows me to treat the virtual drawEvent(),
     * it required me to set draw for each frame and forces game-logic into a "draw" function
     * However, it is not virtual, and uses private values to check whether the window was initialized correctly
     * A solution, then, is to separate those two things into separate classes:
     *  - An application layer that is responsible for rendering and inputs and such.
     *      - essentially the layer/medium between the player of the game and the engine/game itself
     *  - The actual engine / game
     * */

    // variables for dummy test
    Vector3 m_staticSpherePosition;
    Vector3 m_movingSpherePosition;
    Vector3 m_movingSphereVelocity;
    Float m_sphereRadius = 1;

    // stuff related to the scenegraph and thus rendering
    Scene3D m_scene;
    Object3D m_manipulator, m_cameraObject;
    SceneGraph::Camera3D* m_camera;
    SceneGraph::DrawableGroup3D m_drawables;
    Vector3 m_previousPosition;

    // timing
    Timeline timeline;

    // Keep shaders and mesh as member variable to keep them alive
    Shaders::Phong m_shader;
    GL::Mesh m_mesh;
};

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

double ApplicationLayer::getFrameDelta(){
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

class Engine {
    public:
        explicit Engine();
        explicit Engine(int argc, char** argv);
        //~Engine();

        void run();
        void update(float frameDelta);

    private:
        bool mainLoopIteration();

        // Need pointer because of creating and such
        Magnum::ApplicationLayer* m_applicationLayer = nullptr;
        std::vector<Magnum::Primitive*> spheres;       
};

Engine::Engine(int argc, char** argv){
    m_applicationLayer = new Magnum::ApplicationLayer({argc, argv});
    spheres.reserve(2);
    spheres.push_back(m_applicationLayer->addSphere({0.f, 0.f, -8.f}, 35.f));
    spheres.push_back(m_applicationLayer->addSphere({2.f, 1.f, -6.f}, 75.f));
}

void Engine::update(float frameDelta){
    using namespace Magnum;
    Vector3 oldPosition = spheres[1]->getPosition();
    Vector3 velocity = {0.f, 0.f, -1.f};
    Vector3 newPosition = oldPosition + (velocity * frameDelta);
    spheres[1]->updatePosition(newPosition);
}

bool Engine::mainLoopIteration() {
    std::cout << m_applicationLayer->getFrameDelta() << std::endl;
    update(m_applicationLayer->getFrameDelta());
    m_applicationLayer->redraw(); //ensure drawing of frame
    return m_applicationLayer->mainLoopIteration(); //application returns if is exited
}

void Engine::run(){
    while(mainLoopIteration()){}
}



int main(int argc, char** argv)
{
    std::cout << "You have entered " << argc 
         << " arguments:" << "\n"; 
  
    for (int i = 0; i < argc; ++i) 
        std::cout << argv[i] << "\n";

    {
        Engine engine = Engine(argc, argv);
        engine.run();
    }
}
