#pragma once
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Timeline.h>

namespace Magnum
{

typedef SceneGraph::Object<SceneGraph::MatrixTransformation3D> Object3D;
typedef SceneGraph::Scene<SceneGraph::MatrixTransformation3D> Scene3D;

class Primitive: public Object3D, SceneGraph::Drawable3D
{
public:
    explicit Primitive(Object3D& parent, 
            SceneGraph::DrawableGroup3D& group,
            const Trade::MeshData& primitive,
            Shaders::Phong& shader,
            Vector3 position,
            const Color4& color)
        : Object3D{&parent},
          SceneGraph::Drawable3D{*this, &group}, 
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

class ApplicationLayer: public Platform::Application {
public:
    explicit ApplicationLayer(const Arguments& arguments);

    Primitive* addCube(Vector3 initialPosition, float color);

    Float getFrameDelta();

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

    // stuff related to the scenegraph and thus rendering
    Scene3D m_scene;
    Object3D m_manipulator;
    Object3D* m_cameraObject;
    SceneGraph::Camera3D* m_camera;
    SceneGraph::DrawableGroup3D m_drawables;
    Vector3 m_previousPosition;

    // timing
    Timeline timeline;

    // Keep shaders and mesh as member variable to keep them alive
    Shaders::Phong m_shader;
    GL::Mesh m_mesh;

private:
    /* Camera and interaction */
    Matrix4 m_transformation, m_projection, m_view;
    Vector2i m_previousMousePosition;

    void mousePressEvent(MouseEvent& event) override;
    //void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

};

} /* Magnum */ 

