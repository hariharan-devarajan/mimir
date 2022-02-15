//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATA_STRUCTURE_H
#define MIMIR_DATA_STRUCTURE_H

#include <stdint-gcc.h>
#include <string>
#include <vector>
#include <mimir/advice/advice_type.h>
#include <mimir/advice/advice.h>
#include <memory>
#include "typedef.h"

namespace mimir {
    struct Storage {
        uint8_t _index;
        std::string _mount_point;
    };

    struct Node {
        uint32_t _unique_hash;
    };

    struct Application : public Node {
        std::string _name;
        uint32_t _argument_hash;
    };

    struct File : public Node {
        std::string _name;
        uint32_t _full_path_hash;
    };

    template<typename SOURCE, typename DESTINATION>
    struct Edge {
        SOURCE source;
        DESTINATION destination;
    };

    struct ApplicationFileDAG {
        std::vector<Application> applications;
        std::vector<File> files;
        std::vector<Edge<Node, Node>> edges;
    };

    struct RankFileDAG {
        std::vector <Rank> ranks;
        std::vector <File> files;
        std::vector <Edge<Node, Node>> edges;
    };

    struct TransferSizeDistribution {
        float _0_4kb;
        float _4_64kb;
        float _64kb_1mb;
        float _1mb_16mb;
        float _16mb;
    };
    struct MimirKey {
        MimirKey(){}
        MimirKey(const MimirKey& key) {}
        MimirKey(const MimirKey&& key){}

        bool operator==(const MimirKey& other) const {
            return true;
        }
    };

    struct POSIXMimirKey: public MimirKey {
        int _fd;
        POSIXMimirKey():MimirKey(), _fd(-1){}
        POSIXMimirKey(const POSIXMimirKey& key):MimirKey(key), _fd(key._fd) {}
        POSIXMimirKey(const POSIXMimirKey&& key):MimirKey(key), _fd(key._fd){}
        bool operator==(const POSIXMimirKey& l) {
            return _fd == l._fd;
        }
    };

    struct STDIOMimirKey: public MimirKey {
        FILE* _fh;
        STDIOMimirKey():MimirKey(), _fh(NULL){}
        STDIOMimirKey(const STDIOMimirKey& key):MimirKey(key), _fh(key._fh) {}
        STDIOMimirKey(const STDIOMimirKey&& key):MimirKey(key), _fh(key._fh){}
        bool operator==(const STDIOMimirKey& l) {
            return _fh == l._fh;
        }
    };

    struct MimirHandler {
        AdviceType _type;
        std::shared_ptr<Advice> _payload;
        uint32_t _index;
        MimirHandler(): _type(PrimaryAdviceType::ADVICE_NONE, OperationAdviceType::NO_OP),
                        _payload(), _index() {}
    };
}
namespace std {
    template<>
    struct hash<mimir::File> {
        size_t operator()(const mimir::File &k) const {
            size_t hash_val = hash<uint32_t>()(k._full_path_hash);
            hash_val ^= hash<uint32_t>()(k._unique_hash);
            hash_val ^= hash<std::string>()(k._name);
            return hash_val;
        }
    };
    template<>
    struct hash<mimir::MimirKey> {
        size_t operator()(const mimir::MimirKey &k) const {
            return 0;
        }
    };
}
#endif //MIMIR_DATA_STRUCTURE_H
