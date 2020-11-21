//
// Created by revol-xut on 15.10.20.
//

#ifndef BWINF2020_AUFGABE4_GRAPH_HPP
#define BWINF2020_AUFGABE4_GRAPH_HPP

#include <vector>
#include <iostream>

#include "vertex.hpp"

struct Alignment_Description{
    unsigned int rotation_;
    unsigned int overlaying_vertices_;
    bool mirage_x_;
    bool mirage_y_;
};

struct Edge {
    Vertex first;
    Vertex second;
};
auto operator==(const Edge& e1, const Edge& e2) -> bool;
auto operator<(const Edge& e1, const Edge& e2) -> bool;
auto operator<<(std::ostream& os, const Edge& e) -> std::ostream&;

namespace std
{
    template<>
    struct hash<Edge>
    {
        auto operator()(const Edge& obj) const -> size_t ;
    };
}
auto hash(const Edge&) -> std::size_t ;

auto make_edge(const Vertex& v1, const Vertex& v2) -> Edge;

auto make_edge(const Vertex& v1, const Vertex& v2) -> Edge;


class Graph {
public:
    Graph();
    ~Graph();

    void find_ideal_alignment(const std::shared_ptr<Graph>& other_graph);
    static auto find_ideal_rotation(std::vector<Edge>& edge_1, std::vector<Edge>& edge_2) -> std::pair<unsigned int, unsigned int>;

    void add_edge(const Edge& edge);
    void print();
    void difference(const std::shared_ptr<Graph>& other_graph);
    auto size() -> unsigned int;
    auto valid_match_stick_graph() -> bool;
    auto visualize() const -> std::string;

    auto get_edge(unsigned int index) -> Edge;
private:
    void transpose_graph(const Alignment_Description& align);
    auto cross(const Edge &e1, const Edge &e2) -> bool;
    static void mirror_x(std::vector<Vertex>& vertices);
    static void mirror_y(std::vector<Vertex>& vertices);
    static void align(std::vector<Vertex>& vertices, std::pair<float, float> min_values);
    auto extract_vertices() -> std::vector<Vertex>;
    std::vector<Edge> base_graph_;

};


#endif //BWINF2020_AUFGABE4_GRAPH_HPP
