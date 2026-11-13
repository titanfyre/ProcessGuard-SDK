#include "SDK/ProcessGuard.h"
#include "WireTypes.h"
#include <map>
struct ProductSession{

std::unique_ptr<ProcessGuard::Session> session;
ProcessGuard::Session& Get(){if(!session)session=std::make_unique<ProcessGuard::Session>();return *session;}
std::vector<PG_RULE> Rules(const Json& a){auto array=a.at("rules");Require(array.is_array() && array.size()<=PG_RULES,"Too many rules");std::vector<PG_RULE> rules;for(const auto& r:array)rules.push_back(ProcessGuard::Rule(Number(r,"id"),Wide(r.at("path").get<std::string>()),static_cast<ProcessGuard::Action>(Number(r,"action",0,1))));return rules;}

Json Invoke(const std::string& op,const Json& a){

if(op=="check"){auto policy=FromWire<PG_POLICY>(a.at("policy"));return {{"valid",ProcessGuard::Valid(policy)},{"value",EncodeWire(policy)}};}
if(op=="make_rule")return EncodeWire(ProcessGuard::Rule(Number(a,"id"),Wide(a.at("path").get<std::string>()),static_cast<ProcessGuard::Action>(Number(a,"action",0,1))));
if(op=="start"){Get().Start(Rules(a));return Json::object();}
if(op=="set_rules"){Get().SetRules(Rules(a));return Json::object();}
if(op=="stop"){if(session)session->Stop();return Json::object();}
if(op=="status")return EncodeWire(Get().Status());
if(op=="read")return EncodeWire(Get().Read());
if(op=="event_path"){auto event=FromWire<PG_EVENT>(a.at("event"));Require(event.Chars<PG_PATH,"Event path exceeds capacity");return {{"path",Utf8(ProcessGuard::Path(event))}};}

throw SdkError(50,"Unknown operation");}
};
static Runtime<ProductSession> runtime;
extern "C" __declspec(dllexport) unsigned processguard_sdk_json_open(std::uint64_t* id)noexcept{return runtime.Open(id);}
extern "C" __declspec(dllexport) unsigned processguard_sdk_json_close(std::uint64_t id)noexcept{return runtime.Close(id);}
extern "C" __declspec(dllexport) char* processguard_sdk_json_call(std::uint64_t id,const char* request,std::uint32_t length)noexcept{return runtime.Call(id,request,length);}
extern "C" __declspec(dllexport) void processguard_sdk_json_free(char* response)noexcept{delete[] response;}
