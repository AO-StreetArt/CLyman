//This defines some of the global variables that need to be accessed
//Across different files.  These are either numeric constants or
//singletons which form the backbone of the service.

#include "aossl/factory/response_interface.h"
#include "Response.pb.h"

#ifndef CLYMAN_RESPONSE
#define CLYMAN_RESPONSE

inline std::string response_to_protobuffer(ApplicationResponseInterface *resp)
{

//Transfer the response interface elements to a protocol buffer object
pResponse::ProtoResponse resp_buffer;
resp_buffer.set_error_code(resp->get_error_code());
resp_buffer.set_error_message(resp->get_error_message());
resp_buffer.set_transaction_id(resp->get_transaction_id());
resp_buffer.set_object_id(resp->get_object_id());

//Serialize the object to a string and return it
std::string wstr;
resp_buffer.SerializeToString(&wstr);
return wstr;
}

#endif
