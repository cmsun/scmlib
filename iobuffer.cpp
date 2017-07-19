/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : iobuffer.cpp
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/04/09 20:55:06
*  @Last Modified  : 2017/04/11 02:26:59
********************************************************************************
*/

#include "iobuffer.h"

iobuffer::iobuffer(void)
    :mPos(0)
    ,mError(false)
{
    int32_t i = 1;
    mLocalByteOrder = *((int8_t *)&i) == 1 ? little_endian : big_endian;
    mTargetByteOrder = mLocalByteOrder;
}

iobuffer::iobuffer(size_type size)
    :mPos(0)
    ,mError(false)
{
    int32_t i = 1;
    mLocalByteOrder = *((int8_t *)&i) == 1 ? little_endian : big_endian;
    mTargetByteOrder = mLocalByteOrder;
    mData.resize(size);
}

void iobuffer::seek(iobuffer::pos_type off, iobuffer::seek_dir dir)
{
    pos_type newPos = 0;
    switch(dir)
    {
        case seek_cur:
            newPos = mPos + off;
            break;
        case seek_end:
            newPos = (pos_type)mData.size() + off;
            break;
        default:
            newPos = off;
    }

    if(newPos >= 0 && newPos <= (pos_type)mData.size())
    {
        mPos = newPos;
    }
}

#define iobuffer_operator_write(type) \
    iobuffer &iobuffer::operator<<(const type &val) \
    {   \
        type tmp = 0; \
        unsigned char *ptr1, *ptr2; \
        if(mError) \
        { \
            return *this; \
        } \
        if(mLocalByteOrder != mTargetByteOrder) \
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

#define iobuffer_operator_read(type) \
    iobuffer &iobuffer::operator>>(type &val) \
    {   \
        type tmp = 0; \
        unsigned char *ptr1, *ptr2; \
        if(mError) \
        { \
            return *this; \
        } \
        read((char *)&val, sizeof(type)); \
        if(mLocalByteOrder != mTargetByteOrder) \
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

iobuffer_operator_write(char);
iobuffer_operator_write(unsigned char);
iobuffer_operator_write(short);
iobuffer_operator_write(unsigned short);
iobuffer_operator_write(int);
iobuffer_operator_write(unsigned int);
iobuffer_operator_write(long int);
iobuffer_operator_write(unsigned long int);
iobuffer_operator_write(long long);
iobuffer_operator_write(unsigned long long);
iobuffer_operator_write(float);
iobuffer_operator_write(double);
iobuffer_operator_write(bool);

iobuffer_operator_read(char);
iobuffer_operator_read(unsigned char);
iobuffer_operator_read(short);
iobuffer_operator_read(unsigned short);
iobuffer_operator_read(int);
iobuffer_operator_read(unsigned int);
iobuffer_operator_read(long int);
iobuffer_operator_read(unsigned long int);
iobuffer_operator_read(long long);
iobuffer_operator_read(unsigned long long);
iobuffer_operator_read(float);
iobuffer_operator_read(double);
iobuffer_operator_read(bool);

