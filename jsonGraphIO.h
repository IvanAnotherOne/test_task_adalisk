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
#include "graphIO.h"

/** Load/save graphs from/to a .json file

    JSON must contain adjacency matrix of a graph and must be an object with set of key-value pairs
    where key is string of [A-Z] symbols like "A", "B", ... and value is an array representing a
    row of the adjacency matrix.

    Example:
    {
      "A" : [1, 0],
      "B" : [1, 1]
    }
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

    void dump(const Graph &g) override
    {
        using namespace boost;
        std::string key = "A";
        json::object json;
        // Generate JSON structure from a graph
        for (auto [ui, ui_end] = vertices(g); ui != ui_end; ++ui) {
            json::array row;
            for (auto [vi, vi_end] = vertices(g); vi != vi_end; ++vi) {
                auto [ei, isPresent] = edge(*ui, *vi, g);
                row.emplace_back(isPresent ? 1 : 0);
            }
            json.emplace(key, row);
            key = next_key(key);
        }
        // Try to save generated JSON to a file
        save_json(json);
    };
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

    void save_json(boost::json::object json)
    {
        using namespace std;
        // Try to open .json file for write
        fstream f(filename_, ios::out|ios::trunc);
        if (!f.is_open())
            throw GraphIOError("Failed to open file " + filename_);
        // Manually print JSON row by row for better view
        char lastSym = '{'; // start of JSON object
        for (const auto &item : json) {
            f << lastSym << endl; // finish previous line
            f << "    \"" << item.key() << "\" : " << item.value(); // format print
            lastSym = ',';
        }
        f << endl << '}' << endl;
    };

    /** Get next key for JSON row

        Account key as a string of symbols [A-Z]. Next key would be
        string with increased last symbol by lexicographical order.
     */
    std::string next_key(std::string key)
    {
        bool cf = true;
        for (auto it = key.rbegin(); cf && it != key.rend(); ++it) {
            assert(*it >= 'A' && *it <= 'Z');
            if (*it == 'Z') {
                *it = 'A';
                cf = true;
            }
            else {
                *it += 1; // Set to next symbol
                cf = false;
            }
        }
        if (cf) {
            return 'A' + key;
        }
        else {
            return key;
        }
    }
};

#endif // _JSONGRAPHIO_H_
