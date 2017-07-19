/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : iostackbuffer.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/04/10 22:23:29
*  @Last Modified  : 2017/06/28 01:29:42
********************************************************************************
*/

#ifndef IOSTACKBUFFER_H
#define IOSTACKBUFFER_H

#include <stdint.h>
#include <string.h>

enum byte_order {little_endian, big_endian};

template<int32_t SIZE, byte_order ORDER = little_endian>
class iostackbuffer
{
private:
    enum seek_dir {seek_set, seek_cur, seek_end};

    //重载类的new操作符，同时把它声明为私有函数，则该类就不能动态生成，只能静态
    //作为全局变量使用，或者在栈中生成使用。
    noexcept void *operator new(size_t){return (void *)0;};
    void operator delete(void *){};

    //声明和定义iostl.cpp所引用的io_err函数
    //模版类的友元函数的声明和定义要放在一起，否则无法自动生成该函数的实例
    friend bool io_err(iostackbuffer<SIZE, ORDER> &io)
    {
        return io.mError;
    }

private:
    char mData[SIZE];
    int32_t mPos;
    byte_order mLocalByteOrder;
    bool mError;

public:
    iostackbuffer(void);
    int32_t write(const char *buff, int32_t len);
    int32_t read(char *buff, int32_t len);
    const char *data(void);
    int32_t size(void);
    int32_t tell(void);
    void seek(int32_t off, seek_dir dir = seek_set);
    bool error(void);
    void reset(void);

    iostackbuffer &operator<<(const char &val);
    iostackbuffer &operator<<(const unsigned char &val);
    iostackbuffer &operator<<(const short &val);
    iostackbuffer &operator<<(const unsigned short &val);
    iostackbuffer &operator<<(const int &val);
    iostackbuffer &operator<<(const unsigned int &val);
    iostackbuffer &operator<<(const long int &val);
    iostackbuffer &operator<<(const unsigned long int &val);
    iostackbuffer &operator<<(const long long &val);
    iostackbuffer &operator<<(const unsigned long long &val);
    iostackbuffer &operator<<(const float &val);
    iostackbuffer &operator<<(const double &val);
    iostackbuffer &operator<<(const bool &val);

    iostackbuffer &operator>>(char &val);
    iostackbuffer &operator>>(unsigned char &val);
    iostackbuffer &operator>>(short &val);
    iostackbuffer &operator>>(unsigned short &val);
    iostackbuffer &operator>>(int &val);
    iostackbuffer &operator>>(unsigned int &val);
    iostackbuffer &operator>>(long int &val);
    iostackbuffer &operator>>(unsigned long int &val);
    iostackbuffer &operator>>(long long &val);
    iostackbuffer &operator>>(unsigned long long &val);
    iostackbuffer &operator>>(float &val);
    iostackbuffer &operator>>(double &val);
    iostackbuffer &operator>>(bool &val);
};

template<int32_t SIZE, byte_order ORDER>
iostackbuffer<SIZE, ORDER>::iostackbuffer(void)
    :mPos(0)
    ,mError(false)
{
    int32_t i = 1;
    mLocalByteOrder = *((int8_t *)&i) == 1 ? little_endian : big_endian;
}

template<int32_t SIZE, byte_order ORDER>
inline int32_t iostackbuffer<SIZE, ORDER>::write(const char *buff, int32_t len)
{
    int32_t remain;
    if(mError)
    {
        return 0;
    }
    remain = SIZE - mPos; 
    if(remain < len)
    {
        mError = true;
        return 0;
    }
    memcpy(mData + mPos, buff, len);
    mPos += len;
    return len;
}

template<int32_t SIZE, byte_order ORDER>
inline int32_t iostackbuffer<SIZE, ORDER>::read(char *buff, int32_t len)
{
    int32_t remain;
    if(mError)
    {
        return 0;
    }
    remain = SIZE - mPos; 
    memset(buff, 0, len);
    if(remain < len)
    {
        len = remain;
        mError = true;
    }
    memcpy(buff, mData + mPos, len);
    mPos += len;
    return len;
}

template<int32_t SIZE, byte_order ORDER>
inline const char *iostackbuffer<SIZE, ORDER>::data(void)
{
    return mData;
}

template<int32_t SIZE, byte_order ORDER>
inline int32_t iostackbuffer<SIZE, ORDER>::size(void)
{
    return SIZE;
}

template<int32_t SIZE, byte_order ORDER>
inline int32_t iostackbuffer<SIZE, ORDER>::tell(void)
{
    return mPos;
}

template<int32_t SIZE, byte_order ORDER>
void iostackbuffer<SIZE, ORDER>::seek(int32_t off, iostackbuffer<SIZE, ORDER>::seek_dir dir)
{
    int32_t newPos = 0;
    switch(dir)
    {
        case seek_cur:
            newPos = mPos + off;
            break;
        case seek_end:
            newPos = SIZE + off;
            break;
        default:
            newPos = off;
    }

    if(newPos >= 0 && newPos <= SIZE)
    {
        mPos = newPos;
    }
}

template<int32_t SIZE, byte_order ORDER>
bool iostackbuffer<SIZE, ORDER>::error(void)
{
    return mError;
}

template<int32_t SIZE, byte_order ORDER>
void iostackbuffer<SIZE, ORDER>::reset(void)
{
    mPos = 0;
    mError = false;
}

#define iostackbuffer_operator_write(type) \
    template<int32_t SIZE, byte_order ORDER> \
    iostackbuffer<SIZE, ORDER> &iostackbuffer<SIZE, ORDER>::operator<<(const type &val) \
    {   \
        type tmp = 0; \
        unsigned char *ptr1, *ptr2; \
        if(mError) \
        { \
            return *this; \
        } \
        if(mLocalByteOrder != ORDER) \
        { \
            ptr1 = (unsigned char *)&val; \
            ptr2 = (unsigned char *)&tmp + sizeof(type) - 1; \
            for(unsigned i = 0; i < sizeof(type); ++i) \
            { \
                *ptr2-- = *ptr1++; \
            } \
            write((const char *)&tmp, sizeof(type)); \
        } \
        else \
        { \
            write((const char *)&val, sizeof(type)); \
        } \
        return *this; \
    }

#define iostackbuffer_operator_read(type) \
    template<int32_t SIZE, byte_order ORDER> \
    iostackbuffer<SIZE, ORDER> &iostackbuffer<SIZE, ORDER>::operator>>(type &val) \
    {   \
        type tmp = 0; \
        unsigned char *ptr1, *ptr2; \
        if(mError) \
        { \
            return *this; \
        } \
        read((char *)&val, sizeof(type)); \
        if(mLocalByteOrder != ORDER) \
        { \
            ptr1 = (unsigned char *)&val; \
            ptr2 = (unsigned char *)&tmp + sizeof(type) - 1; \
            for(unsigned i = 0; i < sizeof(type); ++i) \
            { \
                *ptr2-- = *ptr1++; \
            } \
            val = tmp; \
        } \
        return *this; \
    }

iostackbuffer_operator_write(char);
iostackbuffer_operator_write(unsigned char);
iostackbuffer_operator_write(short);
iostackbuffer_operator_write(unsigned short);
iostackbuffer_operator_write(int);
iostackbuffer_operator_write(unsigned int);
iostackbuffer_operator_write(long int);
iostackbuffer_operator_write(unsigned long int);
iostackbuffer_operator_write(long long);
iostackbuffer_operator_write(unsigned long long);
iostackbuffer_operator_write(float);
iostackbuffer_operator_write(double);
iostackbuffer_operator_write(bool);

iostackbuffer_operator_read(char);
iostackbuffer_operator_read(unsigned char);
iostackbuffer_operator_read(short);
iostackbuffer_operator_read(unsigned short);
iostackbuffer_operator_read(int);
iostackbuffer_operator_read(unsigned int);
iostackbuffer_operator_read(long int);
iostackbuffer_operator_read(unsigned long int);
iostackbuffer_operator_read(long long);
iostackbuffer_operator_read(unsigned long long);
iostackbuffer_operator_read(float);
iostackbuffer_operator_read(double);
iostackbuffer_operator_read(bool);

#include "iostl.h"

#endif /* end of include guard: IOSTACKBUFFER_H */
