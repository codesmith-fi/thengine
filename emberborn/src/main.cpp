#include "EmberbornGame.h"
#include "thengine/DebugLogger.h"

int main() {
  LOG_INFO() << "Starting Project Emberborn...";

  EmberbornGame game("Project Emberborn", 1280, 768);
  int result = game.run();

  LOG_INFO() << "Emberborn exited with return code: " << result;
  return result;
}
