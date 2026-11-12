#pragma once
#include <string.h>
typedef unsigned long PG_U32;
typedef unsigned __int64 PG_U64;
typedef unsigned short PG_U16;
#define PG_VERSION 1u
#define PG_PATH 512u
#define PG_RULES 16u
#define PG_RING 256u
#define PG_READ 16u
#define PG_AUDIT 0u
#define PG_DENY 1u
#define PG_UNKNOWN_NAME 1u
#define PG_TRUNCATED 2u
#define PG_PRIOR_FAILURE 4u
#define PG_IOCTL(n) ((0x8000u<<16)|(3u<<14)|((n)<<2))
#define PG_SET PG_IOCTL(0x810u)
#define PG_STOP PG_IOCTL(0x811u)
#define PG_STATUS PG_IOCTL(0x812u)
#define PG_RECEIVE PG_IOCTL(0x813u)
typedef struct PG_RULE { PG_U32 Id,Action,Chars,Reserved; PG_U16 Path[PG_PATH]; } PG_RULE;
typedef struct PG_POLICY { PG_U32 Version,Size,Count,Reserved; PG_RULE Rules[PG_RULES]; } PG_POLICY;
typedef struct PG_REQUEST { PG_U32 Version,Size,Reserved[2]; } PG_REQUEST;
typedef struct PG_STATS {
 PG_U32 Version,Size,Active,RuleCount;
 PG_U64 Generation,Frequency,Produced,Denied,Dropped,UnknownNames;
 PG_U32 Available,Capacity,Reserved[2];
} PG_STATS;
typedef struct PG_EVENT {
 PG_U64 Sequence,Qpc,ProcessKey,Generation;
 PG_U32 ProcessId,ParentId,Operation,RuleId,Decision,Flags,Status,Chars;
 PG_U16 Path[PG_PATH];
} PG_EVENT;
typedef struct PG_BATCH { PG_STATS Stats; PG_U32 Count,Reserved[3]; PG_EVENT Events[PG_READ]; } PG_BATCH;
static __inline int PG_Text(const PG_U16* s,PG_U32 n) {
 PG_U32 i;
 if(!n||n>=PG_PATH||s[n])return 0;
 for(i=0;i<n;++i){
  if(s[i]<32||s[i]=='*'||s[i]=='?'||s[i]=='/'||(!s[i]))return 0;
  if(s[i]>=0xD800&&s[i]<=0xDBFF){if(++i>=n||s[i]<0xDC00||s[i]>0xDFFF)return 0;}
  else if(s[i]>=0xDC00&&s[i]<=0xDFFF)return 0;
 }
 for(i=n+1;i<PG_PATH;++i)if(s[i])return 0;
 return s[n-1]!='\\';
}
static __inline int PG_ValidPolicy(const PG_POLICY* p) {
 PG_U32 i,j;
 if(p->Version!=PG_VERSION||p->Size!=sizeof(*p)||p->Reserved||p->Count>PG_RULES)return 0;
 for(i=0;i<p->Count;++i){const PG_RULE* r=&p->Rules[i];
  if(!r->Id||r->Action>PG_DENY||r->Reserved||!PG_Text(r->Path,r->Chars)||r->Path[0]!='\\')return 0;
  for(j=0;j<i;++j)if(p->Rules[j].Id==r->Id)return 0;
 }
 for(i=p->Count;i<PG_RULES;++i){const unsigned char* bytes=(const unsigned char*)&p->Rules[i];for(j=0;j<sizeof(PG_RULE);++j)if(bytes[j])return 0;}
 return 1;
}
#ifndef _KERNEL_MODE
#include <windows.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
static_assert(sizeof(PG_RULE)==1040&&sizeof(PG_POLICY)==16656&&sizeof(PG_STATS)==80&&sizeof(PG_EVENT)==1088&&sizeof(PG_BATCH)==17504,"Unexpected wire packing");
namespace ProcessGuard {
enum class Action:unsigned { Audit=PG_AUDIT,Deny=PG_DENY };
PG_RULE Rule(unsigned id,const std::wstring& path,Action action=Action::Audit);
bool Valid(const PG_POLICY&) noexcept;
bool Valid(const PG_STATS&) noexcept;
bool Valid(const PG_BATCH&) noexcept;
std::wstring Path(const PG_EVENT&);
std::string Utf8(const std::wstring&);
class Session {
 struct State;std::unique_ptr<State> state_;
 void Command(unsigned,const void*,DWORD,void*,DWORD);
public:
 Session();~Session();
 Session(const Session&)=delete;Session& operator=(const Session&)=delete;
 void Start(const std::vector<PG_RULE>& rules={});
 void SetRules(const std::vector<PG_RULE>&);
 PG_BATCH Read(); // Immediate bounded drain; no polling or waits inside the driver.
 PG_STATS Status();
 void Stop() noexcept; // Closing the device also clears the policy.
};
}
#endif
