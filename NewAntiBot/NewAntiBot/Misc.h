int len(const char* data);
int cmp(const char* data1, const char* data2);
int cmpi (const char * dst, const char * src);
BOOL QueryReg(char* data, char const *KeyName, char const *Name);
BOOL SetReg(char const *KeyName, char const *Name, char const *Value);
int __cdecl memcmp (const void * buf1, const void * buf2, size_t count);
void * __cdecl memcpy (void * dst, const void * src, size_t count);
void * __cdecl memset (void *dst, int val, size_t count);
int itoa(unsigned val, char *buf);
void SetD2WindowText(char* Text);
void KillMe(int reason);
int file_exists (const char *path);