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
        bool operator==(const TransferSizeDistribution& other) const {
            return _0_4kb == other._0_4kb &&
                    _4_64kb == other._4_64kb &&
                    _64kb_1mb == other._64kb_1mb &&
                    _1mb_16mb == other._1mb_16mb  &&
                    _16mb == other._16mb;
        }
    };
    struct MimirKey {
        size_t _id;
        MimirKey():_id(){}
        MimirKey(const MimirKey& key):_id(key._id) {}
        MimirKey(const MimirKey&& key):_id(key._id) {}

        bool operator==(const MimirKey& other) const {
            return _id == other._id;
        }
    };

    struct MimirHandler {
    public:
        size_t _id;
        AdviceType _type;
        MimirHandler(): _type(PrimaryAdviceType::ADVICE_NONE, OperationAdviceType::NO_OP),
                        _id() {
        }
        MimirHandler(const MimirHandler& other):_type(other._type),
                                                _id(other._id) {}
        MimirHandler(const MimirHandler&& other):_type(other._type),
                                                 _id(other._id) {}
        bool operator==(const MimirHandler& other) const {
            return _type == other._type && _id == other._id;
        }
        MimirHandler& operator=(const MimirHandler& other) {
            _type = other._type;
            _id = other._id;
            return *this;
        }
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
            return k._id;
        }
    };

}
#endif //MIMIR_DATA_STRUCTURE_H
