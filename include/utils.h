#ifndef ___UTILS_H___
#define ___UTILS_H___

int GetRegistryValue(const char *dir, const char *name, void *buf, int bufsize, int inttype);
int Random(int min, int max);
int utf82unicode(wchar_t *dest, char *src);
VlfText pspEverestPrintf(int x, int y, const char *text, ...);

#endif
