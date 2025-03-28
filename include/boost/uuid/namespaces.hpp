#ifndef BOOST_UUID_NAMESPACES_HPP_INCLUDED
#define BOOST_UUID_NAMESPACES_HPP_INCLUDED

// Copyright 2010 Andy Tompkins
// Copyright 2024 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/uuid/uuid.hpp>

namespace boost {
namespace uuids {
namespace ns {

inline uuid dns() noexcept
{
    uuid result = {{
        0x6b, 0xa7, 0xb8, 0x10, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }};
    return result;
}

inline uuid url() noexcept
{
    uuid result = {{
        0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }};
    return result;
}

inline uuid oid() noexcept
{
    uuid result = {{
        0x6b, 0xa7, 0xb8, 0x12, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }};
    return result;
}

inline uuid x500dn() noexcept
{
    uuid result = {{
        0x6b, 0xa7, 0xb8, 0x14, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }};
    return result;
}

}}} // namespace boost::uuids::ns

#endif // BOOST_UUID_NAMESPACES_HPP_INCLUDED
