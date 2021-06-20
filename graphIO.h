/*
 * Ivan Artyukh, 2021
 */

#ifndef _GRAPHIO_H_
#define _GRAPHIO_H_

#include <exception>
#include <boost/graph/graph_traits.hpp>

/**
 * Should be thrown by GraphKeeper class on errors.
 */
class GraphIOError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/**
 * Interface to load/save graph
 */
template<class AdjacencyGraph>
class GraphIO
{
public:
    virtual ~GraphIO() = default;

    virtual void load(AdjacencyGraph &graph) = 0;
    virtual void dump(const AdjacencyGraph &graph) = 0;
};

#endif // _GRAPHIO_H_
