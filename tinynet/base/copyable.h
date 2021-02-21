/*
 * @Date: 2021-02-19 08:46:32
 * @LastEditors: Kevin
 * @LastEditTime: 2021-02-19 08:48:08
 * @FilePath: /tinynet/tinynet/base/copyable.h
 */

#ifndef TINYNET_BASE_COPYABLE_H
#define TINYNET_BASE_COPYABLE_H

namespace tinynet
{

    class copyable
    {
    protected:
        copyable() = default;
        ~copyable() = default;
    };

} // namespace tinynet

#endif // !TINYNET_BASE_COPYABLE_H