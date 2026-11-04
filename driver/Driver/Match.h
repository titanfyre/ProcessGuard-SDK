#pragma once
// No allocation or registry/filesystem calls while holding the event/policy lock.
static __inline PG_U16 Upper(PG_U16 c){return (PG_U16)RtlUpcaseUnicodeChar((WCHAR)c);}
static __inline BOOLEAN Equal(const PG_U16* a,const PG_U16* b,PG_U32 n){PG_U32 i;for(i=0;i<n;++i)if(Upper(a[i])!=Upper(b[i]))return FALSE;return TRUE;}
static __inline BOOLEAN Match(const PG_RULE* r,const PG_U16* name,PG_U32 chars,BOOLEAN subtree){
 if(chars<r->Chars||(!subtree&&chars!=r->Chars))return FALSE;
 return Equal(r->Path,name,r->Chars)&&(chars==r->Chars||(subtree&&name[r->Chars]=='\\'));
}
static __inline BOOLEAN Ancestor(const PG_RULE* r,const PG_U16* name,PG_U32 chars){return chars<r->Chars&&r->Path[chars]=='\\'&&Equal(r->Path,name,chars);}
static __inline BOOLEAN Starts(const PG_RULE* r,const WCHAR* prefix){PG_U32 n=(PG_U32)wcslen(prefix);return r->Chars>n&&Equal(r->Path,(const PG_U16*)prefix,n);}
static __inline BOOLEAN AllowedScope(const PG_RULE* r){ return Starts(r,L"\\Device\\")&&r->Chars>12; }
