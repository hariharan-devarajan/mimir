//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATA_STRUCTURE_H
#define MIMIR_DATA_STRUCTURE_H

#include <stdint-gcc.h>
#include <string>
#include <utility>
#include <vector>
#include <mimir/advice/advice_type.h>
#include <mimir/advice/advice.h>
#include <memory>
#include "typedef.h"
#include <assert.h>

namespace mimir {
    struct Storage {
        std::string _mount_point;
        uint32_t _capacity_mb;
        Storage():_mount_point(), _capacity_mb(0) {}
        Storage(std::string mount_point, uint32_t capacity_mb):
                                _mount_point(std::move(mount_point)), _capacity_mb(capacity_mb) {}
        bool operator==(const Storage& other) const {
            return _mount_point == other._mount_point && _capacity_mb == other._capacity_mb;
        }
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
        TransferSizeDistribution(float ts_0_4kb, float ts_4_64kb, float ts_64kb_1mb, float ts_1mb_16mb, float ts_16mb):
                                _0_4kb(ts_0_4kb), _4_64kb(ts_4_64kb), _64kb_1mb(ts_64kb_1mb), _1mb_16mb(ts_1mb_16mb), _16mb(ts_16mb) {
            assert(ts_0_4kb + ts_4_64kb + ts_64kb_1mb + ts_1mb_16mb + ts_16mb <=1.0 &&
                           ts_0_4kb + ts_4_64kb + ts_64kb_1mb + ts_1mb_16mb + ts_16mb >=0 );
        }
        TransferSizeDistribution(): TransferSizeDistribution(0.0,0.0,0.0,0.0,0.0){ }
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
