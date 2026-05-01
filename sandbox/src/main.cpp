#include "SandboxGame.h"
#include "thengine/DebugLogger.h"

int main() {
  LOG_INFO() << "Starting thengine sandbox application...";

  SandboxGame game("thengine Sandbox", WINDOW_WIDTH, WINDOW_HEIGHT);
  int result = game.run();

  LOG_INFO() << "Sandbox exited with return code: " << result;
  return result;
}
