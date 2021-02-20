#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Primitives/Icosphere.h>
#include <iostream>

namespace Magnum
{
class ApplicationLayer: public Platform::Application {
    public:
        explicit ApplicationLayer(const Arguments& arguments);

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
         *      - the main engine class callc the mainLoopIteration() of the application layer in each loop
         * */

        // variables for dummy test
        Vector3 m_staticSpherePosition;
        Vector3 m_movingSpherePosition;
        Vector3 m_movingSphereVelocity;
        Float m_sphereRadius = 1;
};

ApplicationLayer::ApplicationLayer(const Arguments& arguments):
    Platform::Application(arguments, Configuration{}
            .setTitle("basic engine")
            .setWindowFlags(Configuration::WindowFlag::Resizable))
{
	using namespace Math::Literals;
}

void ApplicationLayer::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);

    /* TODO: Add your drawing code here */

    swapBuffers();
}

} /* Magnum */ 

class Engine {
    public:
        explicit Engine();
        explicit Engine(int argc, char** argv);
        ~Engine();

        void run();

    private:
        bool mainLoopIteration();

        // Need pointer because of creating and such
        Magnum::ApplicationLayer* m_applicationLayer = nullptr;
        
};

Engine::Engine(int argc, char** argv){
    m_applicationLayer = new Magnum::ApplicationLayer({argc, argv});
}

bool Engine::mainLoopIteration() {
    m_applicationLayer->mainLoopIteration(); 
    m_applicationLayer->redraw();
    return true;
}

void Engine::run(){
    //while(mainLoopIteration()){}
    for(int i = 0; i < 100; i++) {
        std::cout << i << std::endl;
        mainLoopIteration();
    }
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
