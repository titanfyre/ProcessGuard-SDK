#include "../SDK/ProcessGuard.h"
#include <iostream>
int wmain(int count,wchar_t** args){
 try{
  std::cout<<"Deny one executable launched directly by this example; other programs are unaffected.\n";
  if(count!=2){std::cout<<"Usage: ProcessGuardExample.exe \"C:\\Path\\child.exe\"\n";return 0;}
  ProcessGuard::Session session;
  session.Start({ProcessGuard::Rule(1,args[1],ProcessGuard::Action::Deny)});
  std::wstring command=L"\""+std::wstring(args[1])+L"\"";
  STARTUPINFOW startup{};startup.cb=sizeof(startup);PROCESS_INFORMATION child{};
  if(CreateProcessW(args[1],command.data(),nullptr,nullptr,FALSE,0,nullptr,nullptr,&startup,&child)){CloseHandle(child.hThread);CloseHandle(child.hProcess);std::cout<<"Launch continued. Inspect events and name flags.\n";}
  else std::cout<<"Launch returned Windows error "<<GetLastError()<<'\n';
  auto batch=session.Read();
  for(unsigned i=0;i<batch.Count;++i)std::cout<<ProcessGuard::Utf8(ProcessGuard::Path(batch.Events[i]))<<" : "<<(batch.Events[i].Decision?"denied":"observed")<<'\n';
  // The session destructor closes its handle and removes its rules automatically.
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}return 0;
}
