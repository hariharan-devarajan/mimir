//
// Created by hariharan on 2/28/22.
//

#ifndef MIMIR_POSIX_ATHENA_CLIENT_H
#define MIMIR_POSIX_ATHENA_CLIENT_H
#include "athena_client.h"
namespace athena {
class PosixClient : public Client<int> {
 private:
  static std::shared_ptr<athena::PosixClient>  instance;

 public:
  PosixClient(bool is_mpi) : Client<int>(is_mpi) {
      mimir::Logger::Instance("ATHENA")->log(mimir::LOG_INFO,
                                            "Constructing PosixClient");
  }
  static std::shared_ptr<athena::PosixClient>  Instance() {
    if (instance == nullptr) {
      instance = std::make_shared<athena::PosixClient>(is_mpi());
    }
    return instance;
  }
};
}  // namespace athena
#endif  // MIMIR_POSIX_ATHENA_CLIENT_H
