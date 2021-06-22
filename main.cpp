/*
 * Ivan Artyukh, 2021
 */

#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_matrix.hpp>
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

    std::unique_ptr<GraphIO<Graph>> input, output;
    try {
        // Get input
        input = get_io(argv[1]);
        // Get output
        if (argc > 2) {
            output = get_io(argv[1]);
        }
        else {
            output = get_std_io();
        }
    }
    catch (const std::runtime_error &err) {
        std::cout << "Failed to set up graph input/output: " << err.what() << std::endl;
    }

    Graph g = input->load();
    output->dump(g);

    return 0;
}
