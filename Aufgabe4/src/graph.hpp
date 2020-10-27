//
// Created by revol-xut on 15.10.20.
//

#ifndef BWINF2020_AUFGABE4_GRAPH_HPP
#define BWINF2020_AUFGABE4_GRAPH_HPP

#include <vector>

#include "vertex.hpp"

struct Alignment_Description{
    unsigned int rotation_;
    unsigned int overlaying_vertices_;
    bool mirage_x_;
    bool mirage_y_;
};

typedef std::pair<Vertex, Vertex> Edge;

class Graph {
public:
    Graph();
    ~Graph();

    void find_ideal_alignment(const std::shared_ptr<Graph>& other_graph);
    static auto find_ideal_rotation(std::vector<Vertex>& vertices_1, std::vector<Vertex>& vertices_2) -> std::pair<unsigned int, unsigned int>;

    void add_edge(const Edge& edge);
    void print();
    auto difference(const std::shared_ptr<Graph>& other_graph) -> std::shared_ptr<Graph>;
    auto size() -> unsigned int;
    auto valid_match_stick_graph() -> bool;
private:
    void transpose_graph(const Alignment_Description& align);

    static void mirror_x(std::vector<Vertex>& vertices);
    static void mirror_y(std::vector<Vertex>& vertices);
    static void align(std::vector<Vertex>& vertices, std::pair<float, float> min_values);
    auto extract_vertices() -> std::vector<Vertex>;
    std::vector<Edge> base_graph_;

};


#endif //BWINF2020_AUFGABE4_GRAPH_HPP
