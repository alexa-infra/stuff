#pragma once

#include "types.h"
#include <string>

namespace base
{
    class SmartBuffer
    {
    public:
        SmartBuffer(const char* str, bool base64);
        SmartBuffer(const char* str, u32 len, bool base64);
        SmartBuffer(const u8* data, u32 len);

        SmartBuffer(const SmartBuffer& buf);
        SmartBuffer& operator=(const SmartBuffer& buf);

        ~SmartBuffer();

        std::string hex() const;
        std::string base64() const;
        std::string str() const;

        bool operator==(const SmartBuffer& buf) const;
        bool operator!=(const SmartBuffer& buf) const;

        SmartBuffer xor(u8 ch) const;
        SmartBuffer xor(const char* key) const;
        SmartBuffer xor(const SmartBuffer& buf) const;

        u8* data() const { return data_; }
        u32 dataLen() const { return dataLen_; }
    private:
        u8* data_;
        u32 dataLen_;
    };
}