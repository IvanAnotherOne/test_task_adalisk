/*
 * Ivan Artyukh, 2021
 */

#ifndef _STDGRAPHIO_H_
#define _STDGRAPHIO_H_

#include <string>
#include <iostream>

#include "graphIO.h"

/** Input/print graph as adjacency matrix from/to std streams.
 */
template<class Graph>
class StdGraphIO: public GraphIO<Graph>
{
public:
    StdGraphIO() = default;
    ~StdGraphIO() = default;

    Graph load() override
    {
        throw GraphIOError("Not implemented");
    }

    void dump(const Graph &g) override
    {
        using namespace std;
        cout << "Adjacency matrix:" << endl;
        for (auto [ui, ui_end] = vertices(g); ui != ui_end; ++ui) {
            for (auto [vi, vi_end] = vertices(g); vi != vi_end; ++vi) {
                auto [ei, isPresent] = boost::edge(*ui, *vi, g);
                cout << (isPresent ? 1 : 0) << ' ';
            }
            cout << endl;
        }
    }
};

#endif // _STDGRAPHIO_H_
