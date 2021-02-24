/*
 * @Date: 2021-02-24 10:06:24
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-24 12:49:41
 * @FilePath: /tinynet/tinynet/net/Buffer.h
 */
#ifndef TINYNET_NET_BUFFER_H
#define TINYNET_NET_BUFFER_H

#include "tinynet/base/utils.h"
#include "tinynet/net/endian.h"

#include <cmath>
#include <vector>

namespace tinynet
{
    namespace net
    {
        class Buffer : noncopyable
        {
            static const size_t __CheapPrepend = 8;
            static const size_t __InitialSize = 1024;

        public:
            explicit Buffer(size_t initialSize = __InitialSize)
                : _buffer(__CheapPrepend + initialSize),
                  _readerIndex(__CheapPrepend),
                  _writerIndex(__CheapPrepend)
            {
            }

            void swap(Buffer &rhs)
            {
                _buffer.swap(rhs._buffer);
                std::swap(_readerIndex, rhs._readerIndex);
                std::swap(_writerIndex, rhs._writerIndex);
            }

            size_t readableBytes() const { return _writerIndex - _readerIndex; }
            size_t writableBytes() const { return _buffer.size() - _writerIndex; }
            size_t prependableBytes() const { return _readerIndex; }

            const char *peek() const { return begin() + _readerIndex; }
            char *beginWrite() { return begin() + _writerIndex; }
            const char *beginWrite() const { return begin() + _writerIndex; }

            void retrieve(size_t len)
            {
                assert(len <= readableBytes());
                _readerIndex += len;
            }

            void retrieveUntil(const char *end)
            {
                assert(peek() <= end && end <= beginWrite());
                retrieve(end - peek());
            }

            void retrieveAll()
            {
                _readerIndex = __CheapPrepend;
                _writerIndex = __CheapPrepend;
            }

            void retrieveInt8() { retrieve(sizeof(int8_t)); }
            void retrieveInt16() { retrieve(sizeof(int16_t)); }
            void retrieveInt32() { retrieve(sizeof(int32_t)); }
            void retrieveInt64() { retrieve(sizeof(int64_t)); }

            string extract(size_t len)
            {
                assert(len <= readableBytes());
                string result(peek(), len);
                retrieve(len);
                return result;
            }

            string extractAll()
            {
                return extract(readableBytes());
            }

            void ensureWriteableBytes(size_t len)
            {
                if (writableBytes() < len)
                {
                    makeSpace(len);
                }
                assert(writableBytes() >= len);
            }

            void append(const char *str, size_t len)
            {
                ensureWriteableBytes(len);
                std::copy(str, str + len, beginWrite());
                _writerIndex += len;
            }

            void append(const void *data, size_t len)
            {
                append(static_cast<const char *>(data), len);
            }

            void append(const string &str)
            {
                append(str.c_str(), str.size());
            }

            void appendInt8(int8_t x)
            {
                append(&x, sizeof(int8_t));
            }

            void appendInt16(int16_t x)
            {
                int16_t beX = sockets::hostToNetwork16(x);
                append(&beX, sizeof beX);
            }

            void appendInt32(int32_t x)
            {
                int32_t beX = sockets::hostToNetwork32(x);
                append(&beX, sizeof beX);
            }

            void appendInt64(int64_t x)
            {
                int64_t beX = sockets::hostToNetwork64(x);
                append(&beX, sizeof beX);
            }

            int8_t peekInt8() const
            {
                assert(readableBytes() >= sizeof(int8_t));
                int8_t x = *peek();
                return x;
            }

            int16_t peekInt16() const
            {
                assert(readableBytes() >= sizeof(int16_t));
                int16_t beX = 0;
                ::memcpy(&beX, peek(), sizeof beX);
                return sockets::networkToHost16(beX);
            }

            int32_t peekInt32() const
            {
                assert(readableBytes() >= sizeof(int32_t));
                int32_t beX = 0;
                ::memcpy(&beX, peek(), sizeof beX);
                return sockets::networkToHost32(beX);
            }

            int64_t peekInt64() const
            {
                assert(readableBytes() >= sizeof(int64_t));
                int64_t beX = 0;
                ::memcpy(&beX, peek(), sizeof beX);
                return sockets::networkToHost64(beX);
            }

            int8_t readInt8()
            {
                int8_t result = peekInt8();
                retrieveInt8();
                return result;
            }

            int16_t readInt16()
            {
                int16_t result = peekInt16();
                retrieveInt16();
                return result;
            }

            int32_t readInt32()
            {
                int32_t result = peekInt32();
                retrieveInt32();
                return result;
            }

            int64_t readInt64()
            {
                int64_t result = peekInt64();
                retrieveInt64();
                return result;
            }

            void prepend(const void *data, size_t len)
            {
                assert(len <= prependableBytes());
                _readerIndex -= len;
                const char *d = static_cast<const char *>(data);
                std::copy(d, d + len, begin() + _readerIndex);
            }

            void shrink(size_t reserve)
            {
                Buffer other;
                other.ensureWriteableBytes(readableBytes() + reserve);
                other.append(extractAll());
                swap(other);
            }

            size_t readFd(int fd, int *savedErrno);

        private:
            void makeSpace(size_t len)
            {
                if (writableBytes() + prependableBytes() < len + __CheapPrepend)
                {
                    _buffer.resize(_writerIndex + len);
                }
                else
                {
                    size_t readable = readableBytes();
                    std::copy(begin() + _readerIndex,
                              begin() + _writerIndex,
                              begin() + __CheapPrepend);
                    _readerIndex = __CheapPrepend;
                    _writerIndex = _readerIndex + readable;
                }
            }

            char *begin() { return &*_buffer.begin(); }
            const char *begin() const { return &*_buffer.begin(); }

        private:
            std::vector<char> _buffer;
            size_t _readerIndex;
            size_t _writerIndex;
        };

    } // namespace net

} // namespace tinynet

#endif // !TINYNET_NET_BUFFER_H