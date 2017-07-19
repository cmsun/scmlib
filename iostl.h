/*
********************************************************************************
*                              COPYRIGHT NOTICE
*                             Copyright (c) 2016
*                             All rights reserved
*
*  @FileName       : iostl.h
*  @Author         : scm 351721714@qq.com
*  @Create         : 2017/04/06 21:26:02
*  @Last Modified  : 2017/04/16 16:03:11
********************************************************************************
*/

#ifndef IOSTL_H
#define IOSTL_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>

namespace iostl {

template<typename Stream>
Stream &operator<<(Stream &s, const char *str);

template<typename Stream>
Stream &operator<<(Stream &s, const std::string &str);
template<typename Stream>
Stream &operator>>(Stream &s, std::string &str);

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::vector<T> &v);
template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::vector<T> &v);

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::list<T> &l);
template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::list<T> &l);

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::deque<T> &d);
template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::deque<T> &d);

template<typename Stream, typename Key, typename T>
Stream &operator<<(Stream &s, const std::map<Key, T> &m);
template<typename Stream, typename Key, typename T>
Stream &operator>>(Stream &s, std::map<Key, T> &m);

template<typename Stream>
bool io_err(Stream &s); //需要用户程序去实现

template<typename Stream>
Stream &operator<<(Stream &s, const char *str)
{
    uint32_t length;
    length = strlen(str);
    s << length;
    for(uint32_t i = 0; i < length; ++i)
    {
        s << str[i];
    }
    return s;
}

template<typename Stream>
Stream &operator<<(Stream &s, const std::string &str)
{
    uint32_t length;
    length = str.length();
    s << length;
    for(uint32_t i = 0; i < length; ++i)
    {
        s << str.at(i);
    }
    return s;
}

template<typename Stream>
Stream &operator>>(Stream &s, std::string &str)
{
    uint32_t i, length;
    char *buff;
    s >> length;
    if(io_err(s))
    {
        return s;
    }
    buff = new char[length];
    for(i = 0; i < length; ++i)
    {
        if(io_err(s))
        {
            break;
        }
        s >> buff[i];
    }
    str.assign(buff, buff + i);
    delete []buff;
    return s;
}

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::vector<T> &v)
{
    uint32_t size;
    size = v.size();
    s << size;
    for(uint32_t i = 0; i < size; ++i)
    {
        s << v.at(i);
    }
    return s;
}

template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::vector<T> &v)
{
    uint32_t size;
    s >> size;
    if(io_err(s))
    {
        return s;
    }
    v.clear();
    v.reserve(size);
    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        if(io_err(s))
        {
            break;
        }
        s >> t;
        v.push_back(t);
    }
    return s;
}

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::list<T> &l)
{
    uint32_t size;
    size = l.size();
    s << size;
    for(typename std::list<T>::const_iterator itr = l.begin(); itr != l.end(); ++itr)
    {
        s << *itr;
    }
    return s;
}

template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::list<T> &l)
{
    uint32_t size;
    s >> size;
    if(io_err(s))
    {
        return s;
    }
    l.clear();
    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        if(io_err(s))
        {
            break;
        }
        s >> t;
        l.push_back(t);
    }
    return s;
}

template<typename Stream, typename T>
Stream &operator<<(Stream &s, const std::deque<T> &d)
{
    uint32_t size;
    size = d.size();
    s << size;
    for(typename std::deque<T>::const_iterator itr = d.begin(); itr != d.end(); ++itr)
    {
        s << *itr;
    }
    return s;
}

template<typename Stream, typename T>
Stream &operator>>(Stream &s, std::deque<T> &d)
{
    uint32_t size;
    s >> size;
    if(io_err(s))
    {
        return s;
    }
    d.clear();
    d.reserve(size);
    for(uint32_t i = 0; i < size; ++i)
    {
        T t;
        if(io_err(s))
        {
            break;
        }
        s >> t;
        d.push_back(t);
    }
    return s;
}

template<typename Stream, typename Key, typename T>
Stream &operator<<(Stream &s, const std::map<Key, T> &m)
{
    uint32_t size;
    size = m.size();
    s << size;
    typename std::map<Key, T>::const_iterator itr = m.begin();
    typename std::map<Key, T>::const_iterator end = m.end();
    while (itr != end)
    {
        s << itr.key() << itr.value();
        ++itr;
    }
    return s;
}

template<typename Stream, typename Key, typename T>
Stream &operator>>(Stream &s, std::map<Key, T> &m)
{
    uint32_t size;
    s >> size;
    if(io_err(s))
    {
        return s;
    }
    m.clear();
    for(uint32_t i = 0; i < size; ++i)
    {
        Key k;
        T t;
        if(io_err(s))
        {
            break;
        }
        s >> k >> t;
        m[k] = t;
    }
    return s;
}

} /* end of namespace iostl */

#endif /* end of include guard: IOSTL_H */
