#include "application.h"

#include <iostream>
#include <vector>

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
    //std::cout << m_applicationLayer->getFrameDelta() << std::endl;
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
