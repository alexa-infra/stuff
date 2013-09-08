#include "smartbuffer.h"

namespace base
{
    void hex_to_base64(const u8* data, u32 size, char*& base64, u32& outSize);
    void base64_to_hex(const char* data, u32 size, u8*& hex, u32& outSize);
    void str_to_hex(const char* data, u32 size, u8*& hex, u32& outSize);
    void hex_to_str(const u8* data, u32 size, char*& str, u32& outSize);

    SmartBuffer::SmartBuffer(const char* str, bool base64)
    {
        if (!base64)
        {
            str_to_hex(str, strlen(str), data_, dataLen_);
        }
        else
        {
            base64_to_hex(str, strlen(str), data_, dataLen_);
        }
    }

    SmartBuffer::SmartBuffer(const char* str, u32 len, bool base64)
    {
        if (!base64)
        {
            str_to_hex(str, len, data_, dataLen_);
        }
        else
        {
            base64_to_hex(str, len, data_, dataLen_);
        }
    }

    SmartBuffer::SmartBuffer(const u8* data, u32 len)
    {
        dataLen_ = len;
        data_ = new u8[dataLen_];
        memcpy(data_, data, dataLen_);
    }

    SmartBuffer::SmartBuffer(const SmartBuffer& buf)
    {
        dataLen_ = buf.dataLen_;
        data_ = new u8[dataLen_];
        memcpy(data_, buf.data_, dataLen_);
    }

    SmartBuffer& SmartBuffer::operator=(const SmartBuffer& buf)
    {
        delete[] data_;

        dataLen_ = buf.dataLen_;
        data_ = new u8[dataLen_];
        memcpy(data_, buf.data_, dataLen_);

        return *this;
    }

    SmartBuffer::~SmartBuffer()
    {
        delete[] data_;
    }

    std::string SmartBuffer::hex() const
    {
        char* str;
        u32 strLen;
        hex_to_str(data_, dataLen_, str, strLen);
        std::string ret(str, strLen);
        delete[] str;
        return ret;
    }

    std::string SmartBuffer::base64() const
    {
        char* str;
        u32 strLen;
        hex_to_base64(data_, dataLen_, str, strLen);
        std::string ret(str, strLen);
        delete[] str;
        return ret;
    }

    std::string SmartBuffer::str() const
    {
        return std::string((char*)data_, dataLen_);
    }

    bool SmartBuffer::operator==(const SmartBuffer& buf) const
    {
        if (dataLen_ != buf.dataLen_) return false;
        return memcmp(data_, buf.data_, dataLen_) == 0;
    }

    bool SmartBuffer::operator!=(const SmartBuffer& buf) const
    {
        if (dataLen_ != buf.dataLen_) return true;
        return memcmp(data_, buf.data_, dataLen_) != 0;
    }

    SmartBuffer SmartBuffer::xor(u8 ch) const
    {
        u8* temp = new u8[dataLen_];
        for(u32 i=0; i<dataLen_; i++)
            temp[i] = data_[i] ^ ch;
        SmartBuffer buf(temp, dataLen_);
        delete[] temp;
        return buf;
    }

    SmartBuffer SmartBuffer::xor(const char* key) const
    {
        char* buf = new char[dataLen_];
        u32 keysize = strlen(key);
        for(u32 i=0; i<dataLen_; i+=keysize)
        {
            u32 count = std::min(dataLen_-i, keysize);
            for(u32 j=0; j<count; j++)
                buf[i + j] = data_[i + j] ^ key[j];
        }
        SmartBuffer buffer((u8*)buf, dataLen_);
        delete[] buf;
        return buffer;
    }

    SmartBuffer SmartBuffer::xor(const SmartBuffer& buf) const
    {
        u8* temp = new u8[dataLen_];
        for(u32 i=0; i<dataLen_; i++)
            temp[i] = data_[i] ^ buf.data_[i];
        SmartBuffer ret(temp, dataLen_);
        delete[] temp;
        return ret;
    }

    char encode64(u32 d)
    {
        if (d < 26) return 'A' + d;
        if (d < 52) return 'a' + (d - 26);
        if (d < 62) return '0' + (d - 52);
        if (d == 62) return '+';
        if (d == 63) return '/';
        return ' ';
    }

    u32 decode64(char ch)
    {
        if ((ch >= 'A') && (ch <= 'Z')) return ch - 'A';
        if ((ch >= 'a') && (ch <= 'z')) return 26 + ch - 'a';
        if ((ch >= '0') && (ch <= '9')) return 52 + ch - '0';
        if (ch == '+') return 62;
        if (ch == '/') return 63;
        return 64;
    }

    u8 decodeHex1(char ch)
    {
        if ((ch >= '0') && (ch <= '9')) return ch - '0';
        if ((ch >= 'a') && (ch <= 'f')) return 10 + ch - 'a';
        if ((ch >= 'A') && (ch <= 'F')) return 10 + ch - 'A';
        return 16;
    }

    u8 decodeHex(const char* str)
    {
        u8 ret = 0;
        ret += decodeHex1(str[0]) << 4;
        ret += decodeHex1(str[1]) << 0;
        return ret;
    }

    char encodeHex1(u8 d)
    {
        if (d < 10) return '0' + d;
        return 'a' + (d - 10);
    }

    void encodeHex(u8 d, char* str)
    {
        str[0] = encodeHex1((d >> 4) & 0xf);
        str[1] = encodeHex1((d >> 0) & 0xf);
    }

    void str_to_hex(const char* data, u32 size, u8*& hex, u32& outSize)
    {
        outSize = size / 2;
        hex = new u8[outSize+1];
        u32 idx = 0;
        for(u32 i=0; i<size; i+=2)
            hex[idx++] = decodeHex(data + i);
        hex[outSize] = 0;
    }

    void hex_to_str(const u8* data, u32 size, char*& str, u32& outSize)
    {
        outSize = size * 2;
        str = new char[outSize+1];
        
        for(u32 i=0; i<size; i++)
            encodeHex(data[i], str + i*2);

        str[outSize] = 0;
    }

    void hex_to_base64(const u8* data, u32 size, char*& base64, u32& outSize)
    {
        outSize = 4 * ((size + 2) / 3);
        base64 = new char[outSize+1];

        u32 idx = 0;

        u32 unstableSize = size % 3;
        u32 stableSize = size - unstableSize;
        for (u32 pos = 0; pos < stableSize; pos+=3)
        {
            u32 triple = 0;
            for (u32 i = 0; i < 3; i++)
                triple += data[pos + i] << (16 - i*8);
            for (u32 i = 0; i < 4; i++)
                base64[idx++] = encode64((triple >> (18 - i*6)) & 0x3f);
        }

        if (unstableSize > 0)
        {
            u32 triple = 0;
            for (u32 i = 0; i < unstableSize; i++)
                triple += data[stableSize + i] << (16 - i*8);
            u32 count = unstableSize + 1;
            for (u32 i = 0; i < count; i++)
                base64[idx++] = encode64((triple >> (18 - i*6)) & 0x3f);
            for (u32 i = count; i < 4; i++)
                base64[idx++] = '=';
        }
        base64[outSize] = 0;
    }

    void base64_to_hex(const char* data, u32 size, u8*& hex, u32& outSize)
    {
        u32 padding = 0;
        if (data[size-1] == '=')
            padding++;
        if (data[size-2] == '=')
            padding++;
        u32 stableSize = padding == 0 ? size : size - 4;
        u32 unstableSize = padding == 0 ? 0 : 4 - padding;

        outSize = (stableSize / 4) * 3;
        if (unstableSize == 2)
            outSize += 1;
        if (unstableSize == 3)
            outSize += 2;

        hex = new u8[outSize+1];

        u32 idx = 0;
        for(u32 pos = 0; pos < stableSize; pos += 4)
        {
            u32 triple = 0;
            for(u32 i = 0; i < 4; i++)
                triple += decode64(data[pos + i]) << (18 - i*6);
            for(u32 i = 0; i < 3; i++)
                hex[idx++] = triple >> (16 - i*8);
        }
        if (unstableSize > 0)
        {
            u32 triple = 0;
            for(u32 i = 0; i < unstableSize; i++)
                triple += decode64(data[stableSize + i]) << (18 - i*6);
            for(u32 i = 0; i < 3 - padding; i++)
                hex[idx++] = triple >> (16 - i*8);
        }
        hex[outSize] = 0;
    }

}