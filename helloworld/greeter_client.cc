#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "protos/helloworld.grpc.pb.h"

#include <glog/logging.h>

using namespace helloworld;

int main(int argc, char**argv) {
  (void)argc;
  (void)argv;

  // 创建 Channel
  std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:5005",
                                                grpc::InsecureChannelCredentials());

  // 创建 stub
  // 所有的RPC都是通过 stub发送与接收
  std::unique_ptr<Greeter::Stub> stub = Greeter::NewStub(channel);

  grpc::ClientContext context;
  HelloRequest request;
  request.set_name("wqx");
  HelloReply reply;
  
  // 执行 RPC(同步)
  grpc::Status status = stub->SayHello(&context, request, &reply);

  if (status.ok()) {
    LOG(INFO) << reply.message();
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message();
  }
  return 0;
}
