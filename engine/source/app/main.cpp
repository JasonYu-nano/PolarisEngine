#include "app/application.hpp"
#include "foundation/smart_ptr.hpp"


int main(void* args, int argv)
{
    Engine::UniquePtr<Engine::Application> app = Engine::MakeUniquePtr<Engine::Application>();
    app->Init();
    app->Shutdown();
    return 0;
}