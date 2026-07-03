#pragma once

struct edge
{
    int u;
    int v;
    double d;
    double t;
    double w;
    double total_cost()
    {
        return d * t * w;
    }
};