/*
 * Ivan Artyukh, 2021
 */

#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/compute/iterator/transform_iterator.hpp>
#include "jsonGraphIO.h"
#include "stdGraphIO.h"
#include "gphGraphIO.h"

typedef boost::adjacency_matrix<> Graph;

/** Get appropriate graph IO depending on the filename extension */
std::unique_ptr<GraphIO<Graph>> get_io(std::string_view filename)
{
    using namespace std;
    auto ext = filename.substr(filename.find_last_of('.') + 1);
    if (ext == "json") {
        return std::make_unique<JSONGraphIO<Graph>>(string(filename));
    }
    else if (ext == "gph") {
        return std::make_unique<GphGraphIO<Graph>>(string(filename));
    }
    else {
        throw std::runtime_error("Unexpected file format: " + string(ext));
    }
}

/** To print graph to std stream */
std::unique_ptr<GraphIO<Graph>> get_std_io()
{
    return std::make_unique<StdGraphIO<Graph>>();
}

int main(int argc, char *argv[])
{
    using namespace boost;

    if (argc < 2) {
        std::cout << "At least 1 argument must be given: input graph file" << std::endl;
        return -1;
    }

    // Get input/output graph files from arguments
    std::unique_ptr<GraphIO<Graph>> input, output;
    try {
        // Get input
        input = get_io(argv[1]);
        // Get output
        if (argc > 2) {
            output = get_io(argv[2]);
        }
        else {
            output = get_std_io();
        }
    }
    catch (const std::runtime_error &err) {
        std::cout << "Failed to set up graph input/output: " << err.what() << std::endl;
        return -1;
    }

    // Now load graph
    Graph g = input->load();

    // Apply Cuthill-McKee ordering algorithm
    typedef graph_traits<Graph>::vertex_descriptor Vertex;
    std::vector<Vertex> perm(num_vertices(g));
    cuthill_mckee_ordering(g, perm.begin());

    // Create new graph with reordered vertices
    typedef property_map<Graph, vertex_index_t>::const_type VertexIndexMap;
    typedef graph_traits<Graph>::edge_descriptor Edge;
    VertexIndexMap index_map = get(vertex_index, g);
    iterator_property_map <Vertex *, VertexIndexMap> perm_map(perm.data(), index_map);
    auto g_edges = edges(g);
    const auto perm_edge = [&g, &perm_map](const Edge &e) {
        return std::make_pair( get(perm_map, source(e, g)), get(perm_map, target(e, g)) );
    };
    Graph g_permuted(
              make_transform_iterator(g_edges.first, perm_edge),
              make_transform_iterator(g_edges.second, perm_edge),
              num_vertices(g));
    // And finally save it
    output->dump(g_permuted);

    return 0;
}
