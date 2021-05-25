#include "application.h"

#include <iostream>
#include <vector>

namespace Magnum
{
    
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
        ApplicationLayer* m_applicationLayer = nullptr;
        std::vector<Primitive*> spheres;       
};

Engine::Engine(int argc, char** argv){
    m_applicationLayer = new ApplicationLayer({argc, argv});
    spheres.reserve(2);
    spheres.push_back(m_applicationLayer->addCube({-1.f, -1.f, -1.f}, 35.f, nullptr));
    spheres.push_back(m_applicationLayer->addCube({2.f, 2.f, 2.f}, 75.f, spheres[0]));
    spheres.push_back(m_applicationLayer->addCube({2.f, 2.f, 2.f}, 75.f, spheres[1]));

}

static int i = 0;
void Engine::update(float frameDelta){
    Debug{} << "********";
    Debug{} << spheres[1]->transformation().translation();
    Debug{} << spheres[1]->absoluteTransformation().translation();
    Debug{} << spheres[1]->transformation().rotation();
    
    Vector3 velocity = {0.f, 0.f, -3.f};
    spheres[0]->translate(velocity*frameDelta);
    spheres[1]->rotateY(Rad(1.f)*frameDelta);
    spheres[1]->rotateZLocal(Rad(.5f)*frameDelta);
    spheres[2]->rotateX(Rad(2.f)*frameDelta);
    spheres[2]->rotateYLocal(Rad(2.f)*frameDelta);
}

bool Engine::mainLoopIteration() {
    //std::cout << m_applicationLayer->getFrameDelta() << std::endl;
    if (i++ > 2500)
        return false;
    update(m_applicationLayer->getFrameDelta());
    m_applicationLayer->redraw(); //ensure drawing of frame
    return m_applicationLayer->mainLoopIteration(); //application returns if is exited
}

void Engine::run(){
    while(mainLoopIteration()){}
}


} /* Magnum */ 

int main(int argc, char** argv)
{
    std::cout << "You have entered " << argc 
         << " arguments:" << "\n"; 
  
    for (int i = 0; i < argc; ++i) 
        std::cout << argv[i] << "\n";

    {
        Magnum::Engine engine = Magnum::Engine(argc, argv);
        engine.run();
    }
}

