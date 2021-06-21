/*
 * Ivan Artyukh, 2021
 */

#ifndef _JSONGRAPHIO_H_
#define _JSONGRAPHIO_H_

#include <boost/version.hpp>
#if BOOST_VERSION < 107500
#error "Boost of version 1.75.0 or higher required"
#endif
#include <boost/json.hpp>
#include <string>
#include <fstream>
////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
////////////////////////////////////////////////////////////////////////////////////
#include "graphIO.h"

/** Class to load/save graphs from/to a .json file.


 */
template<class Graph>
class JSONGraphIO: public GraphIO<Graph>
{
public:
    JSONGraphIO(std::string filename): filename_(std::move(filename)) {};
    ~JSONGraphIO() = default;

    Graph load() override
    {
        boost::json::value json = load_json();
        // Try to build a graph from loaded json object
        try {
            const auto &root = json.as_object();
            // Set number of vertices same as number of root items in JSON
            Graph g(root.size());
            size_t i = 0;
            for (const auto &item : root) {
                // Try to interpret value of each item as a row of an adjacency matrix
                const auto &row = item.value().as_array();
                if (row.size() != root.size())
                    throw std::invalid_argument("JSON has inconsistent number of rows and columns");

                size_t j = 0;
                for (const auto &v : row) {
                    auto val = v.as_int64();
                    switch (val) {
                    case 0:
                        break;
                    case 1:
                        add_edge(i, j, g);
                        break;
                    default:
                        throw std::invalid_argument("Adjacency matrix must contain only 0 or 1");
                    }
                    ++j;
                }
                ++i;
            }
            return g;
        }
        catch (const std::invalid_argument &err) {
            throw GraphIOError(std::string("JSON is invalid: ") + err.what());
        }
    };

    void dump(const Graph &graph) override
    {};
private:
    std::string filename_;

    boost::json::value load_json()
    {
        using namespace std;
        // Try to open .json file
        fstream f(filename_, ios::in);
        if (!f.is_open())
            throw GraphIOError("Failed to open file " + filename_);

        // Try to parse it
        boost::json::stream_parser jp;
        string line;
        while(getline(f, line)) {
            jp.write_some(line);
        }
        jp.finish();
        if (!jp.done())
            throw GraphIOError("Failed to parse JSON structure");

        // File parsed, close it and get json value
        f.close();
        return jp.release();
    };
};

#endif // _JSONGRAPHIO_H_
