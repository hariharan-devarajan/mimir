//
// Created by hariharan on 2/21/22.
//

#ifndef ATHENA_POSIX_IO_H
#define ATHENA_POSIX_IO_H

#include <cstdio>
#include <memory>
#include <rpc/msgpack.hpp>
#include <vector>
#include <boost/interprocess/containers/string.hpp>
namespace bip = boost::interprocess;
namespace clmdep_msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
  namespace adaptor {
  namespace mv1 = clmdep_msgpack::v1;
  template <>
  struct convert<bip::string> {
    clmdep_msgpack::object const &operator()(clmdep_msgpack::object const &o,
                                             bip::string &v) const {
      switch (o.type) {
        case clmdep_msgpack::type::BIN:
          v.assign(o.via.bin.ptr, o.via.bin.size);
          break;
        case clmdep_msgpack::type::STR:
          v.assign(o.via.str.ptr, o.via.str.size);
          break;
        default:
          throw clmdep_msgpack::type_error();
          break;
      }
      return o;
    }
  };

  template <>
  struct pack<bip::string> {
    template <typename Stream>
    clmdep_msgpack::packer<Stream> &operator()(
        clmdep_msgpack::packer<Stream> &o, const bip::string &v) const {
      uint32_t size = checked_get_container_size(v.size());
      o.pack_str(size);
      o.pack_str_body(v.data(), size);
      return o;
    }
  };

  template <>
  struct object<bip::string> {
    void operator()(clmdep_msgpack::object &o, const bip::string &v) const {
      uint32_t size = checked_get_container_size(v.size());
      o.type = clmdep_msgpack::type::STR;
      o.via.str.ptr = v.data();
      o.via.str.size = size;
    }
  };

  template <>
  struct object_with_zone<bip::string> {
    void operator()(clmdep_msgpack::object::with_zone &o,
                    const bip::string &v) const {
      uint32_t size = checked_get_container_size(v.size());
      o.type = clmdep_msgpack::type::STR;
      char *ptr = static_cast<char *>(
          o.zone.allocate_align(size, MSGPACK_ZONE_ALIGNOF(char)));
      o.via.str.ptr = ptr;
      o.via.str.size = size;
      std::memcpy(ptr, v.data(), v.size());
    }
  };
  }  // namespace adaptor
}
}  // namespace clmdep_msgpack

typedef bip::string DATA;

namespace athena {
int posix_open(DATA filename, int mode, int flags);
int posix_close(int fd);
off_t posix_lseek(int fd, off_t offset, int whence);
ssize_t posix_write(int fd, DATA buf, size_t count);
DATA posix_read(int fd, size_t count);
bool posix_prefetch(DATA filename);

}  // namespace athena

#endif  // ATHENA_POSIX_IO_H
