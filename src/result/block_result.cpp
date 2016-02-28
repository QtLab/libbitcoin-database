/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/database/result/block_result.hpp>

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin.hpp>

namespace libbitcoin {
namespace database {

////#include <boost/iostreams/stream.hpp>
////#include <bitcoin/database/pointer_array_source.hpp>
////chain::header deserialize_header(const uint8_t* begin, uint64_t length)
////{
////    boost::iostreams::stream<byte_pointer_array_source> istream(begin, length);
////    istream.exceptions(std::ios_base::failbit);
////    chain::header header;
////    header.from_data(istream);
////
//////    if (!istream)
//////        throw end_of_stream();
////
////    return header;
////}

using namespace bc::chain;

static constexpr size_t header_size = 80;
static constexpr size_t height_size = sizeof(uint32_t);
static constexpr size_t count_size = sizeof(uint32_t);

block_result::block_result(const uint8_t* slab)
  : slab_(slab)
{
}

block_result::operator bool() const
{
    return slab_ != nullptr;
}

chain::header block_result::header() const
{
    BITCOIN_ASSERT(slab_ != nullptr);
    chain::header header;
    auto deserial = make_deserializer_unsafe(slab_);
    header.from_data(deserial, false);
    return header;
    //// return deserialize_header(slab_, size_limit_);
}

size_t block_result::height() const
{
    BITCOIN_ASSERT(slab_ != nullptr);
    return from_little_endian_unsafe<uint32_t>(slab_ + header_size);
}

size_t block_result::transaction_count() const
{
    BITCOIN_ASSERT(slab_ != nullptr);
    const auto offset = header_size + height_size;
    return from_little_endian_unsafe<uint32_t>(slab_ + offset);
}

hash_digest block_result::transaction_hash(size_t index) const
{
    BITCOIN_ASSERT(slab_ != nullptr);
    BITCOIN_ASSERT(index < transaction_count());
    const auto offset = header_size + height_size + count_size;
    const uint8_t* first = slab_ + offset + index * hash_size;
    auto deserial = make_deserializer_unsafe(first);
    return deserial.read_hash();
}

} // namespace database
} // namespace libbitcoin
