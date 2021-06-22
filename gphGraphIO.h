/*
 * Ivan Artyukh, 2021
 */

#ifndef _GPHGRAPHIO_H_
#define _GPHGRAPHIO_H_

#include <string>
#include <fstream>
#include <cstdint>

#include "graphIO.h"

/** Load/save graphs from/to a .gph file

    gph is a custom binary file format to store a graph. Data is stored by 4-byte words.
    First two words of a file are the number of vertices and the number of edges of a graph.
    Next data is a list of edges stored as a pairs of values; the 1st value in the pair is
    the index of a source vertex and the second is the index of a target vertex of the edge.
 */
template<class Graph>
class GphGraphIO: public GraphIO<Graph>
{
public:
    GphGraphIO(std::string filename): filename_(std::move(filename)) {};
    ~GphGraphIO() = default;

    Graph load() override
    {
        using namespace std;
        // Try to open .gph file
        fstream f(filename_, ios::in|ios::binary);
        if (!f.is_open())
            throw GraphIOError("Failed to open file " + filename_);
        // Read data
        uint32_t vertices, edges;
        read_uint32(f, vertices);
        read_uint32(f, edges);

        Graph g(vertices);
        uint32_t v0, v1;
        while (read_uint32(f, v0) && read_uint32(f, v1)) {
            if (v0 >= vertices || v1 >= vertices)
                throw GraphIOError("Vertex index must be less than total number of vertices");
            add_edge(v0, v1, g);
        }

        return g;
    }

    void dump(const Graph &g) override
    {
        using namespace std;
        // Try to open .gph file for write
        fstream f(filename_, ios::out|ios::trunc|ios::binary);
        if (!f.is_open())
            throw GraphIOError("Failed to open file " + filename_);
        // Write data
        write_uint32(f, static_cast<uint32_t>(num_vertices(g)));
        write_uint32(f, static_cast<uint32_t>(num_edges(g)));
        for (auto [ei, ei_end] = edges(g); ei != ei_end; ++ei) {
            write_uint32(f, static_cast<uint32_t>(source(*ei, g)));
            write_uint32(f, static_cast<uint32_t>(target(*ei, g)));
        }
    }
private:
    std::string filename_;

    static std::istream & read_uint32(std::istream &s, std::uint32_t &val)
    {
        return s.read(reinterpret_cast<char *>(&val), sizeof(std::uint32_t));
    };

    static std::ostream & write_uint32(std::ostream &s, const std::uint32_t &val)
    {
        return s.write(reinterpret_cast<const char *>(&val), sizeof(std::uint32_t));
    };
};

#endif // _GPHGRAPHIO_H_
