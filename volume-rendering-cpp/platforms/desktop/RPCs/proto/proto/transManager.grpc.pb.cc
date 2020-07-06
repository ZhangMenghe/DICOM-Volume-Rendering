// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: proto/transManager.proto

#include "proto/transManager.pb.h"
#include "proto/transManager.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace helmsley {

static const char* dataTransfer_method_names[] = {
  "/helmsley.dataTransfer/getAvailableConfigs",
  "/helmsley.dataTransfer/exportConfigs",
  "/helmsley.dataTransfer/getAvailableDatasets",
  "/helmsley.dataTransfer/getVolumeFromDataset",
  "/helmsley.dataTransfer/Download",
  "/helmsley.dataTransfer/DownloadVolume",
  "/helmsley.dataTransfer/DownloadMasks",
  "/helmsley.dataTransfer/DownloadMasksVolume",
};

std::unique_ptr< dataTransfer::Stub> dataTransfer::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< dataTransfer::Stub> stub(new dataTransfer::Stub(channel));
  return stub;
}

dataTransfer::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_getAvailableConfigs_(dataTransfer_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_exportConfigs_(dataTransfer_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_getAvailableDatasets_(dataTransfer_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_getVolumeFromDataset_(dataTransfer_method_names[3], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_Download_(dataTransfer_method_names[4], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_DownloadVolume_(dataTransfer_method_names[5], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_DownloadMasks_(dataTransfer_method_names[6], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_DownloadMasksVolume_(dataTransfer_method_names[7], ::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  {}

::grpc::Status dataTransfer::Stub::getAvailableConfigs(::grpc::ClientContext* context, const ::Request& request, ::helmsley::configResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_getAvailableConfigs_, context, request, response);
}

void dataTransfer::Stub::experimental_async::getAvailableConfigs(::grpc::ClientContext* context, const ::Request* request, ::helmsley::configResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_getAvailableConfigs_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::getAvailableConfigs(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helmsley::configResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_getAvailableConfigs_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::getAvailableConfigs(::grpc::ClientContext* context, const ::Request* request, ::helmsley::configResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_getAvailableConfigs_, context, request, response, reactor);
}

void dataTransfer::Stub::experimental_async::getAvailableConfigs(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helmsley::configResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_getAvailableConfigs_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::helmsley::configResponse>* dataTransfer::Stub::AsyncgetAvailableConfigsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::helmsley::configResponse>::Create(channel_.get(), cq, rpcmethod_getAvailableConfigs_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::helmsley::configResponse>* dataTransfer::Stub::PrepareAsyncgetAvailableConfigsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::helmsley::configResponse>::Create(channel_.get(), cq, rpcmethod_getAvailableConfigs_, context, request, false);
}

::grpc::Status dataTransfer::Stub::exportConfigs(::grpc::ClientContext* context, const ::Request& request, ::commonResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_exportConfigs_, context, request, response);
}

void dataTransfer::Stub::experimental_async::exportConfigs(::grpc::ClientContext* context, const ::Request* request, ::commonResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_exportConfigs_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::exportConfigs(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::commonResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_exportConfigs_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::exportConfigs(::grpc::ClientContext* context, const ::Request* request, ::commonResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_exportConfigs_, context, request, response, reactor);
}

void dataTransfer::Stub::experimental_async::exportConfigs(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::commonResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_exportConfigs_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::commonResponse>* dataTransfer::Stub::AsyncexportConfigsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::commonResponse>::Create(channel_.get(), cq, rpcmethod_exportConfigs_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::commonResponse>* dataTransfer::Stub::PrepareAsyncexportConfigsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::commonResponse>::Create(channel_.get(), cq, rpcmethod_exportConfigs_, context, request, false);
}

::grpc::Status dataTransfer::Stub::getAvailableDatasets(::grpc::ClientContext* context, const ::Request& request, ::helmsley::datasetResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_getAvailableDatasets_, context, request, response);
}

void dataTransfer::Stub::experimental_async::getAvailableDatasets(::grpc::ClientContext* context, const ::Request* request, ::helmsley::datasetResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_getAvailableDatasets_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::getAvailableDatasets(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helmsley::datasetResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_getAvailableDatasets_, context, request, response, std::move(f));
}

void dataTransfer::Stub::experimental_async::getAvailableDatasets(::grpc::ClientContext* context, const ::Request* request, ::helmsley::datasetResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_getAvailableDatasets_, context, request, response, reactor);
}

void dataTransfer::Stub::experimental_async::getAvailableDatasets(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::helmsley::datasetResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_getAvailableDatasets_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::helmsley::datasetResponse>* dataTransfer::Stub::AsyncgetAvailableDatasetsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::helmsley::datasetResponse>::Create(channel_.get(), cq, rpcmethod_getAvailableDatasets_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::helmsley::datasetResponse>* dataTransfer::Stub::PrepareAsyncgetAvailableDatasetsRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::helmsley::datasetResponse>::Create(channel_.get(), cq, rpcmethod_getAvailableDatasets_, context, request, false);
}

::grpc::ClientReader< ::helmsley::volumeResponse>* dataTransfer::Stub::getVolumeFromDatasetRaw(::grpc::ClientContext* context, const ::Request& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::helmsley::volumeResponse>::Create(channel_.get(), rpcmethod_getVolumeFromDataset_, context, request);
}

void dataTransfer::Stub::experimental_async::getVolumeFromDataset(::grpc::ClientContext* context, ::Request* request, ::grpc::experimental::ClientReadReactor< ::helmsley::volumeResponse>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::helmsley::volumeResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_getVolumeFromDataset_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::helmsley::volumeResponse>* dataTransfer::Stub::AsyncgetVolumeFromDatasetRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeResponse>::Create(channel_.get(), cq, rpcmethod_getVolumeFromDataset_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::helmsley::volumeResponse>* dataTransfer::Stub::PrepareAsyncgetVolumeFromDatasetRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeResponse>::Create(channel_.get(), cq, rpcmethod_getVolumeFromDataset_, context, request, false, nullptr);
}

::grpc::ClientReader< ::helmsley::dcmImage>* dataTransfer::Stub::DownloadRaw(::grpc::ClientContext* context, const ::Request& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), rpcmethod_Download_, context, request);
}

void dataTransfer::Stub::experimental_async::Download(::grpc::ClientContext* context, ::Request* request, ::grpc::experimental::ClientReadReactor< ::helmsley::dcmImage>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::helmsley::dcmImage>::Create(stub_->channel_.get(), stub_->rpcmethod_Download_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::helmsley::dcmImage>* dataTransfer::Stub::AsyncDownloadRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), cq, rpcmethod_Download_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::helmsley::dcmImage>* dataTransfer::Stub::PrepareAsyncDownloadRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), cq, rpcmethod_Download_, context, request, false, nullptr);
}

::grpc::ClientReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::DownloadVolumeRaw(::grpc::ClientContext* context, const ::helmsley::RequestWholeVolume& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), rpcmethod_DownloadVolume_, context, request);
}

void dataTransfer::Stub::experimental_async::DownloadVolume(::grpc::ClientContext* context, ::helmsley::RequestWholeVolume* request, ::grpc::experimental::ClientReadReactor< ::helmsley::volumeWholeResponse>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::helmsley::volumeWholeResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_DownloadVolume_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::AsyncDownloadVolumeRaw(::grpc::ClientContext* context, const ::helmsley::RequestWholeVolume& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), cq, rpcmethod_DownloadVolume_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::PrepareAsyncDownloadVolumeRaw(::grpc::ClientContext* context, const ::helmsley::RequestWholeVolume& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), cq, rpcmethod_DownloadVolume_, context, request, false, nullptr);
}

::grpc::ClientReader< ::helmsley::dcmImage>* dataTransfer::Stub::DownloadMasksRaw(::grpc::ClientContext* context, const ::Request& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), rpcmethod_DownloadMasks_, context, request);
}

void dataTransfer::Stub::experimental_async::DownloadMasks(::grpc::ClientContext* context, ::Request* request, ::grpc::experimental::ClientReadReactor< ::helmsley::dcmImage>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::helmsley::dcmImage>::Create(stub_->channel_.get(), stub_->rpcmethod_DownloadMasks_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::helmsley::dcmImage>* dataTransfer::Stub::AsyncDownloadMasksRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), cq, rpcmethod_DownloadMasks_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::helmsley::dcmImage>* dataTransfer::Stub::PrepareAsyncDownloadMasksRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::dcmImage>::Create(channel_.get(), cq, rpcmethod_DownloadMasks_, context, request, false, nullptr);
}

::grpc::ClientReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::DownloadMasksVolumeRaw(::grpc::ClientContext* context, const ::Request& request) {
  return ::grpc_impl::internal::ClientReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), rpcmethod_DownloadMasksVolume_, context, request);
}

void dataTransfer::Stub::experimental_async::DownloadMasksVolume(::grpc::ClientContext* context, ::Request* request, ::grpc::experimental::ClientReadReactor< ::helmsley::volumeWholeResponse>* reactor) {
  ::grpc_impl::internal::ClientCallbackReaderFactory< ::helmsley::volumeWholeResponse>::Create(stub_->channel_.get(), stub_->rpcmethod_DownloadMasksVolume_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::AsyncDownloadMasksVolumeRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), cq, rpcmethod_DownloadMasksVolume_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::helmsley::volumeWholeResponse>* dataTransfer::Stub::PrepareAsyncDownloadMasksVolumeRaw(::grpc::ClientContext* context, const ::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncReaderFactory< ::helmsley::volumeWholeResponse>::Create(channel_.get(), cq, rpcmethod_DownloadMasksVolume_, context, request, false, nullptr);
}

dataTransfer::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< dataTransfer::Service, ::Request, ::helmsley::configResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::helmsley::configResponse* resp) {
               return service->getAvailableConfigs(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< dataTransfer::Service, ::Request, ::commonResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::commonResponse* resp) {
               return service->exportConfigs(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< dataTransfer::Service, ::Request, ::helmsley::datasetResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::helmsley::datasetResponse* resp) {
               return service->getAvailableDatasets(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[3],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< dataTransfer::Service, ::Request, ::helmsley::volumeResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::grpc_impl::ServerWriter<::helmsley::volumeResponse>* writer) {
               return service->getVolumeFromDataset(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[4],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< dataTransfer::Service, ::Request, ::helmsley::dcmImage>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::grpc_impl::ServerWriter<::helmsley::dcmImage>* writer) {
               return service->Download(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[5],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< dataTransfer::Service, ::helmsley::RequestWholeVolume, ::helmsley::volumeWholeResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::helmsley::RequestWholeVolume* req,
             ::grpc_impl::ServerWriter<::helmsley::volumeWholeResponse>* writer) {
               return service->DownloadVolume(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[6],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< dataTransfer::Service, ::Request, ::helmsley::dcmImage>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::grpc_impl::ServerWriter<::helmsley::dcmImage>* writer) {
               return service->DownloadMasks(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      dataTransfer_method_names[7],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< dataTransfer::Service, ::Request, ::helmsley::volumeWholeResponse>(
          [](dataTransfer::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::Request* req,
             ::grpc_impl::ServerWriter<::helmsley::volumeWholeResponse>* writer) {
               return service->DownloadMasksVolume(ctx, req, writer);
             }, this)));
}

dataTransfer::Service::~Service() {
}

::grpc::Status dataTransfer::Service::getAvailableConfigs(::grpc::ServerContext* context, const ::Request* request, ::helmsley::configResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::exportConfigs(::grpc::ServerContext* context, const ::Request* request, ::commonResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::getAvailableDatasets(::grpc::ServerContext* context, const ::Request* request, ::helmsley::datasetResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::getVolumeFromDataset(::grpc::ServerContext* context, const ::Request* request, ::grpc::ServerWriter< ::helmsley::volumeResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::Download(::grpc::ServerContext* context, const ::Request* request, ::grpc::ServerWriter< ::helmsley::dcmImage>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::DownloadVolume(::grpc::ServerContext* context, const ::helmsley::RequestWholeVolume* request, ::grpc::ServerWriter< ::helmsley::volumeWholeResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::DownloadMasks(::grpc::ServerContext* context, const ::Request* request, ::grpc::ServerWriter< ::helmsley::dcmImage>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status dataTransfer::Service::DownloadMasksVolume(::grpc::ServerContext* context, const ::Request* request, ::grpc::ServerWriter< ::helmsley::volumeWholeResponse>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace helmsley

