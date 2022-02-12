//
// Created by haridev on 2/11/22.
//

#ifndef MIMIR_DATA_STRUCTURE_H
#define MIMIR_DATA_STRUCTURE_H
namespace mimir {
    struct Storage {
        uint8 _index;
        std::string _mount_point;
    };

    struct Node {
        uint32 _unique_hash;
    };

    struct Application : public Node {
        std::string _name;
        uint32 _argument_hash;
    };

    struct File : public Node {
        std::string _name;
        uint32 _full_path_hash;
    };

    template<typename SOURCE, typename DESTINATION>
    struct Edge {
        SOURCE source;
        DESTINATION destination;
    };

    struct ApplicationFileDAG {
        std::vector <Application> applications;
        std::vector <File> files;
        std::vector <Edge<Node, Node>> edges;
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
}
#endif //MIMIR_DATA_STRUCTURE_H
