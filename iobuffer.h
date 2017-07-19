/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : iobuffer.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/04/08 23:13:49
*  @Last Modified  : 2017/06/20 11:17:02
********************************************************************************
*/

#ifndef IOBUFFER_H
#define IOBUFFER_H

#include <stdint.h>
#include <string.h>
#include <vector>

class iobuffer
{
public:
    typedef int32_t size_type;
    typedef int32_t pos_type;
    enum seek_dir {seek_set, seek_cur, seek_end};
    enum byte_order {little_endian, big_endian};

private:
    std::vector<char> mData;
    pos_type mPos;
    byte_order mLocalByteOrder;
    byte_order mTargetByteOrder;
    bool mError;

public:
    iobuffer(void);
    iobuffer(size_type size);
    void resize(size_type size);
    void setByteOrder(byte_order order);
    size_type write(const char *buff, size_type len);
    size_type read(char *buff, size_type len);
    const char *data(void);
    size_type size(void);
    pos_type tell(void);
    void seek(pos_type off, seek_dir dir = seek_set);
    bool error(void);
    void reset(void);

    iobuffer &operator<<(const char &val);
    iobuffer &operator<<(const unsigned char &val);
    iobuffer &operator<<(const short &val);
    iobuffer &operator<<(const unsigned short &val);
    iobuffer &operator<<(const int &val);
    iobuffer &operator<<(const unsigned int &val);
    iobuffer &operator<<(const long int &val);
    iobuffer &operator<<(const unsigned long int &val);
    iobuffer &operator<<(const long long &val);
    iobuffer &operator<<(const unsigned long long &val);
    iobuffer &operator<<(const float &val);
    iobuffer &operator<<(const double &val);
    iobuffer &operator<<(const bool &val);

    iobuffer &operator>>(char &val);
    iobuffer &operator>>(unsigned char &val);
    iobuffer &operator>>(short &val);
    iobuffer &operator>>(unsigned short &val);
    iobuffer &operator>>(int &val);
    iobuffer &operator>>(unsigned int &val);
    iobuffer &operator>>(long int &val);
    iobuffer &operator>>(unsigned long int &val);
    iobuffer &operator>>(long long &val);
    iobuffer &operator>>(unsigned long long &val);
    iobuffer &operator>>(float &val);
    iobuffer &operator>>(double &val);
    iobuffer &operator>>(bool &val);
};

inline void iobuffer::resize(size_type size)
{
    mData.resize(size);
    if(mPos > size)
    {
        mPos = size;
    }
}

inline void iobuffer::setByteOrder(byte_order order)
{
    mTargetByteOrder = order;
}

inline iobuffer::size_type iobuffer::write(const char *buff, size_type len)
{
    size_type remain;
    if(mError)
    {
        return 0;
    }
    remain = (size_type)mData.size() - mPos; 
    if(remain < len)
    {
        mError = true;
        return 0;
    }
    memcpy(mData.data() + mPos, buff, len);
    mPos += len;
    return len;
}

inline iobuffer::size_type iobuffer::read(char *buff, size_type len)
{
    size_type remain;
    if(mError)
    {
        return 0;
    }
    remain = (size_type)mData.size() - mPos; 
    memset(buff, 0, len);
    if(remain < len)
    {
        len = remain;
        mError = true;
    }
    memcpy(buff, mData.data() + mPos, len);
    mPos += len;
    return len;
}

inline const char *iobuffer::data(void)
{
    return mData.data();
}

inline iobuffer::size_type iobuffer::size(void)
{
    return (size_type)mData.size();
}

inline iobuffer::pos_type iobuffer::tell(void)
{
    return mPos;
}

inline bool iobuffer::error(void)
{
    return mError;
}

inline void iobuffer::reset(void)
{
    mPos = 0;
    mError = false;
}

inline bool io_err(iobuffer &io)
{
    return  io.error();
}

#include "iostl.h"

#endif /* end of include guard: IOBUFFER_H */
