#include "SandboxGame.h"
#include "thengine/DebugLogger.h"

int main() {
    LOG_INFO() << "Starting thengine sandbox application...";
    
    SandboxGame game("thengine Sandbox", 1280, 720);
    int result = game.run();
    
    LOG_INFO() << "Sandbox exited with return code: " << result;
    return result;
}
