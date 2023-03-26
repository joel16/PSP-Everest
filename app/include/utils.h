#pragma once

namespace Utils {
    int UTF8ToUnicode(wchar_t *dest, char *src);
    void ASCIIToUnicode(char *unicode, const char *ascii);
    int Rand(int min, int max);
    int GetRegistryValue(const char *dir, const char *name, void *buf, int bufsize, int inttype);
}
