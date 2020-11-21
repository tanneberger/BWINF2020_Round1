//
// Created by revol-xut on 15.10.20.
//

#include <math.h>
#include <memory>
#include <iostream>
#include <unordered_set>

#include "graph.hpp"

auto operator==(const Edge& e1, const Edge& e2) -> bool{
    return (e1.first == e2.first and e1.second == e2.second) or (e1.first == e2.second and e1.second == e2.first);
}
auto operator<(const Edge& e1, const Edge& e2) -> bool{
    return true;
}
auto operator<<(std::ostream& os, const Edge& e) -> std::ostream&{
    auto round = [](float val) -> float {
        return std::round(val * 100.0) / 100.0;
    };
    os << "x_0: " << round(e.first.get_x()) << " y_0: " << round(e.first.get_y()) << " x_1: " << round(e.second.get_x()) << " y_1: " << round(e.second.get_y());
    return os;
}

namespace std{
    auto hash<Edge>::operator()(const Edge &e) const -> size_t {
        return (std::min(e.first.get_x(), e.second.get_x()) * 10 * std::max(e.first.get_y(), e.second.get_y()) + std::max(e.first.get_x(), e.second.get_x())) * ( std::min(e.first.get_y(), e.second.get_y()) + 3);
    }
}

auto make_edge(const Vertex& v1, const Vertex& v2) -> Edge{
    return Edge{v1, v2};
}

Graph::Graph() {
    base_graph_ = {};
}

Graph::~Graph() = default;

auto Graph::extract_vertices() -> std::vector<Vertex> {
    // Iterates through the set of edges and copies vertices that are not found in the vertices vector into it
    std::vector<Vertex> vertices;
    vertices.reserve(20);
    for (const auto &edge : base_graph_) {
        if (std::find(vertices.begin(), vertices.end(), edge.first) == vertices.end()) {
            vertices.push_back(edge.first);
        }
        if (std::find(vertices.begin(), vertices.end(), edge.second) == vertices.end()) {
            vertices.push_back(edge.second);
        }
    }
    return vertices;
}


auto Graph::find_ideal_rotation(std::vector<Edge> &edge_1,
                                std::vector<Edge> &edge_2) -> std::pair<unsigned int, unsigned int> {
    unsigned int ideal_rotation = 0;
    unsigned int overlaying_vertex_count = 0;
    std::pair<float, float> min_values;
    auto smallest = [](const Vertex& vertex, auto& min_values){
        if (vertex.get_coords().first < min_values.first) {
            min_values.first = vertex.get_coords().first;
        }
        if (vertex.get_coords().second < min_values.second) {
            min_values.second = vertex.get_coords().second;
        }
    };

    for (unsigned int i = 0; i < 12; i++) {
        // 1.) Rotate
        min_values.first = std::numeric_limits<float>::max();
        min_values.second = std::numeric_limits<float>::max();

        for (auto &edge : edge_1) {
            edge.first.rotate();
            edge.second.rotate();

            smallest(edge.first, min_values);
            smallest(edge.second, min_values);
        }

        min_values.first = -1 * min_values.first;
        min_values.second = -1 * min_values.second;

        //2.) Align to (0|0)
        for (auto &edge : edge_1) {
            edge.first.add_vector(min_values);
            edge.second.add_vector(min_values);
        }

        //3.) Count Edges with same coordinates
        unsigned int counter = 0;
        for (const auto &e_1 : edge_1) {
            for (const auto &e_2 : edge_2) {
                if (e_1 == e_2) {
                    counter++;
                }
            }
        }

        //4.) save best rotation
        if (counter > overlaying_vertex_count) {
            overlaying_vertex_count = counter;
            // We have to do this because we first rotate and than count overlapping vertices
            ideal_rotation = (i + 1) % 12;
        }
    }

    return std::make_pair(overlaying_vertex_count, ideal_rotation);
}

void Graph::add_edge(const Edge &edge) {
    base_graph_.push_back(edge);
}

void Graph::find_ideal_alignment(const std::shared_ptr<Graph> &other_graph) {
    bool mirage_x, mirage_y;

    Alignment_Description best_align{};
    auto vertices = extract_vertices();
    auto other_vertices = other_graph->extract_vertices();
    auto best_solution = std::min(vertices.size(), other_vertices.size());
    best_align.overlaying_vertices_ = 0;

    unsigned int array[4] = {0, 1, 3, 2};
    for (unsigned int i = 0; i < 4; i++) {
        mirage_x = (array[i] & 1) == 1;
        mirage_y = (array[i] & 2) == 2;
        // 0 0  => 0 1 => 1 1 => 1 0 So only one bit flips every iteration

        if (i > 0 and i % 2 == 0) {
            mirror_x(vertices);
        } else if (i > 0 and i % 2 == 1) {
            mirror_y(vertices);
        }

        auto pair = find_ideal_rotation(base_graph_, other_graph->base_graph_);

        if (pair.first > best_align.overlaying_vertices_) {
            best_align.overlaying_vertices_ = pair.first;
            best_align.mirage_y_ = mirage_y;
            best_align.mirage_x_ = mirage_x;
            best_align.rotation_ = pair.second;
        }

        // No need to search further we already found an optimal alignment
        if (best_align.overlaying_vertices_ == best_solution) {
            break;
        }
    }

    std::cout << "Eine Spiegelung an der X Achse: " << best_align.mirage_x_ << " Eine Spiegelung an der Y Achse: " << best_align.mirage_y_ << std::endl;
    std::cout << "Rotation um " << best_align.rotation_ * 30 << "° entgegen des Uhrzeigersinn " << std::endl;
    std::cout << "Übereinanderliegende Kanten: " << best_align.overlaying_vertices_ << std::endl;
    // This loop mirrors two times y and x
    transpose_graph(best_align);
}

void Graph::transpose_graph(const Alignment_Description &align) {
    std::pair<float, float> move = std::make_pair(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    for (auto &edge : base_graph_) {
        if (align.mirage_x_) {
            edge.first.mirror_x();
            edge.second.mirror_x();
        }
        if (align.mirage_y_) {
            edge.first.mirror_y();
            edge.second.mirror_y();
        }
        edge.first.rotate_by_degree(align.rotation_);
        edge.second.rotate_by_degree(align.rotation_);

        // little bit ugly but there is no way around it
        if (edge.first.get_coords().first < move.first) {
            move.first = edge.first.get_coords().first;
        }
        if (edge.second.get_coords().first < move.first) {
            move.first = edge.second.get_coords().first;
        }
        if (edge.first.get_coords().second < move.second) {
            move.second = edge.first.get_coords().second;
        }
        if (edge.second.get_coords().second < move.second) {
            move.second = edge.second.get_coords().second;
        }
    }
    move.first = -1 * move.first;
    move.second = -1 * move.second;

    for (auto &edge : base_graph_) {
        edge.first.add_vector(move);
        edge.second.add_vector(move);
    }
}

void Graph::mirror_x(std::vector<Vertex> &vertices) {
    float max_value = std::numeric_limits<float>::min();
    for (auto &vertex : vertices) {
        if (vertex.get_coords().first > max_value) {
            max_value = vertex.get_coords().first;
        }
        vertex.add_vector(std::make_pair(-2 * vertex.get_coords().first, 0));
    }
    align(vertices, std::make_pair(max_value, 0));
}

void Graph::mirror_y(std::vector<Vertex> &vertices) {
    float max_value = std::numeric_limits<float>::min();
    for (auto &vertex : vertices) {
        if (vertex.get_coords().second > max_value) {
            max_value = vertex.get_coords().second;
        }
        vertex.add_vector(std::make_pair(0, -2 * vertex.get_coords().second));
    }
    align(vertices, std::make_pair(0, max_value));
}

void Graph::align(std::vector<Vertex> &vertices, std::pair<float, float> min_values) {
    for (auto &vertex : vertices) {
        vertex.add_vector(min_values);
    }
}

void Graph::difference(const std::shared_ptr<Graph> &other_graph){
    std::unordered_multiset<Edge> edge_set;
    edge_set.insert(base_graph_.begin(), base_graph_.end());
    edge_set.insert(other_graph->base_graph_.begin(), other_graph->base_graph_.end());
    auto predicate = [&edge_set](const Edge& k){
        return edge_set.count(k) > 1;
    };

    base_graph_.erase(std::remove_if(base_graph_.begin(), base_graph_.end(), predicate), base_graph_.end());
    other_graph->base_graph_.erase(std::remove_if(other_graph->base_graph_.begin(), other_graph->base_graph_.end(), predicate), other_graph->base_graph_.end());
}

auto Graph::size() -> unsigned int {
    return base_graph_.size();
}

auto Graph::valid_match_stick_graph() -> bool {
    /* Need to check that every edge has the length 1 and that the dont cross

     Checking the length is straight forward we just use the pythagorean theorem and calculate the length of every edge

     Checking if edges cross is a little bit more complicated. We check the intervals and look if they overlap
     e.g. [0, 5] and [1, 3] 1 \in [0, 5] and 3 \in [0,5] for x and y
     If this is the case take those to linear function (the two edges) and see where they intersect
    */
    auto round = [](float val) -> float {
        return std::round(val * 100.0) / 100.0;
    };

    auto intersecting_intervals = [](const Edge &e1, const Edge &e2) -> bool {
        float max_x = std::max(e2.first.get_x(), e2.second.get_x());
        float min_x = std::max(e2.first.get_x(), e2.second.get_x());
        float max_y = std::max(e2.first.get_y(), e2.second.get_y());
        float min_y = std::max(e2.first.get_y(), e2.second.get_y());

        return ((e1.first.get_x() == std::clamp(e1.first.get_x(), min_x, max_x)) or
                (e1.second.get_x() == std::clamp(e1.second.get_x(), min_x, max_x))) and
               ((e1.first.get_y() == std::clamp(e1.first.get_y(), min_y, max_y)) or
                (e1.second.get_y() == std::clamp(e1.second.get_y(), min_y, max_y)));

    };

    for (const auto &edge : base_graph_) {
        if (round(sqrt(pow(edge.first.get_coords().first - edge.second.get_coords().first, 2) +
                       pow(edge.first.get_coords().second - edge.second.get_coords().second, 2))) != 1) {
            return false;
        }
    }
    for (unsigned int i = 0; i < base_graph_.size(); i++) {
        for (unsigned int j = i; j < base_graph_.size(); j++) {
            if (intersecting_intervals(base_graph_.at(i), base_graph_.at(j)) and cross(base_graph_.at(i), base_graph_.at(j))) {
                return false;
            }
        }
    }
    return true;
}

auto Graph::cross(const Edge &e1, const Edge &e2) -> bool {
    float min_x, max_x, min_y, max_y;

    if(std::min(e1.first.get_x(), e1.second.get_x()) < std::min(e2.first.get_x(), e2.first.get_x())){
        min_x = std::min(e2.first.get_x(), e2.second.get_x());
        max_x = std::max(e1.first.get_x(), e1.second.get_x());
    }else{
        min_x = std::min(e1.first.get_x(), e1.second.get_x());
        max_x = std::max(e2.first.get_x(), e2.second.get_x());
    }
    if(std::min(e1.first.get_y(), e1.second.get_y()) < std::min(e2.first.get_y(), e2.first.get_y())){
        min_y = std::min(e2.first.get_y(), e2.second.get_y());
        max_y = std::max(e1.first.get_y(), e1.second.get_y());
    }else{
        min_y = std::min(e1.first.get_y(), e1.second.get_y());
        max_y = std::max(e2.first.get_y(), e2.second.get_y());
    }
    auto round = [](float val) -> float {
        return std::round(val * 100.0) / 100.0;
    };

    // In this interval the two edges have to cross each other
    // Now we use the points to generate the eqations for the two lines
    // Linear Equations to define

    auto m_0 = std::make_pair(e1.second.get_x() - e1.first.get_x(), e1.second.get_y() - e1.first.get_y());
    auto m_1 = std::make_pair(e2.second.get_x() - e2.first.get_x(), e2.second.get_y() - e2.first.get_y());
    auto n_0 = e1.first;
    auto n_1 = e2.first;


    float div = m_0.first * m_1.second - m_0.second * m_1.first;
    if (round(div) == 0) {
        return false;
    }
    float n = (m_1.first * (n_0.get_y() - n_1.get_y()) - m_1.second * (n_0.get_x() - n_1.get_x())) / div;
    float k = (m_0.first * (n_0.get_y() - n_1.get_y()) - m_0.second * (n_0.get_x() - n_1.get_x())) / div;

    float intersec_x = m_0.first * n + n_0.get_x();
    float intersec_y = m_0.second * n + n_0.get_y();

    bool value = min_x < intersec_x and max_x > intersec_x and min_y < intersec_y and max_y > intersec_y;

    return value;
}


auto Graph::visualize() const -> std::string {
    std::string tikz_text;

    for (const Edge &edge : base_graph_) {
        tikz_text += "\\filldraw[color=green!80, draw] (" + std::to_string(edge.first.get_x()) + ", " +
                     std::to_string(edge.first.get_y()) + ") circle (3pt);\n";
        tikz_text += "\\filldraw[color=green!80, draw] (" + std::to_string(edge.second.get_x()) + ", " +
                     std::to_string(edge.second.get_y()) + ") circle (3pt);\n";
        tikz_text +=
                "\\draw (" + std::to_string(edge.first.get_x()) + ", " + std::to_string(edge.first.get_y()) + ") -> (" +
                std::to_string(edge.second.get_x()) + ", " + std::to_string(edge.second.get_y()) + ");\n";
    }

    return tikz_text;
}

auto Graph::get_edge(unsigned int index) -> Edge {
    return base_graph_.at(index);
}
