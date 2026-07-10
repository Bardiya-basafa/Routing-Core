#pragma once

/**
 * @brief Represents an undirected edge with multiplicative weight factors.
 *
 * The raw cost of traversing the edge is:
 *      cost = distance × traffic × weather
 * The final path weight also includes the destination node's delay.
 */
struct edge
{
    int u;    ///< Source node ID
    int v;    ///< Target node ID
    double d; ///< Distance multiplier
    double t; ///< Traffic multiplier
    double w; ///< Weather multiplier

    /**
     * @brief Returns the raw edge weight (without node delay).
     */
    double total_cost() const
    {
        return d * t * w;
    }
};