#include "../SDK/ProcessGuard.h"
#include <iostream>
#include <random>
void Check(bool b){if(!b)throw std::runtime_error("Offline protocol check failed.");}
int main(){try{
 PG_POLICY p{};p.Version=PG_VERSION;p.Size=sizeof(p);Check(ProcessGuard::Valid(p));
 p.Count=1;p.Rules[0]=ProcessGuard::Rule(1,L"\\Device\\HarddiskVolume1\\Demo\\child.exe",ProcessGuard::Action::Deny);Check(ProcessGuard::Valid(p));
 auto base=p;p.Rules[0].Chars=PG_PATH;Check(!ProcessGuard::Valid(p));p=base;p.Rules[0].Path[10]=0;Check(!ProcessGuard::Valid(p));
 p=base;p.Count=2;p.Rules[1]=p.Rules[0];Check(!ProcessGuard::Valid(p));p=base;p.Rules[15].Reserved=1;Check(!ProcessGuard::Valid(p));
 p=base;p.Rules[0].Path[1]='*';Check(!ProcessGuard::Valid(p));p=base;p.Reserved=1;Check(!ProcessGuard::Valid(p));
 PG_STATS stats{};stats.Version=PG_VERSION;stats.Size=sizeof(stats);stats.Capacity=PG_RING;stats.Frequency=10000000;Check(ProcessGuard::Valid(stats));
 PG_BATCH b{};b.Stats=stats;Check(ProcessGuard::Valid(b));b.Count=PG_READ+1;Check(!ProcessGuard::Valid(b));
 std::mt19937 rng(7731);unsigned valid=0;for(unsigned i=0;i<1000000;++i){p=base;auto bytes=reinterpret_cast<unsigned char*>(&p);for(int j=0;j<3;++j)bytes[rng()%sizeof(p)]=static_cast<unsigned char>(rng());if(ProcessGuard::Valid(p))++valid;}
 bool missing=false;try{ProcessGuard::Session client;client.Status();}catch(const std::exception&){missing=true;}Check(missing);
 std::cout<<"Offline checks passed; one million malformed-policy mutations checked (accepted: "<<valid<<"). No driver loaded.\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
