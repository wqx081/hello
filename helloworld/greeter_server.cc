#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "protos/helloworld.grpc.pb.h"

using namespace helloworld;

class GreeterServiceImpl final : public Greeter::Service {
  // Private??
  grpc::Status SayHello(grpc::ServerContext* context,
                        const HelloRequest* request,
                        HelloReply* reply) override {
    (void) context;
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return grpc::Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:5005");
  GreeterServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address,
                           grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server Listening on " << server_address << std::endl;
  
  server->Wait();
}

int main() {
  RunServer();

  return 0;
}
