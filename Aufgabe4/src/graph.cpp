//
// Created by revol-xut on 15.10.20.
//

#include <math.h>
#include <memory>
#include <iostream>

#include "graph.hpp"

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


auto Graph::find_ideal_rotation(std::vector<Vertex> &vertices_1,
                                std::vector<Vertex> &vertices_2) -> std::pair<unsigned int, unsigned int> {
    unsigned int ideal_rotation = 0;
    unsigned int overlaying_vertex_count = 0;

    for (unsigned int i = 0; i < 4; i++) {
        // 1.) Rotate
        std::pair<float, float> min_values;
        min_values.first = std::numeric_limits<float>::max();
        min_values.second = std::numeric_limits<float>::max();

        for (auto &vertex : vertices_1) {
            vertex.rotate();

            if (vertex.get_coords().first < min_values.first) {
                min_values.first = vertex.get_coords().first;
            }
            if (vertex.get_coords().second < min_values.second) {
                min_values.second = vertex.get_coords().second;
            }
        }

        min_values.first = -1 * min_values.first;
        min_values.second = -1 * min_values.second;

        //2.) Align to (0|0)
        for (auto &vertex : vertices_1) {
            vertex.add_vector(min_values);
        }

        //3.) Count Vertices with same coordinates
        unsigned int counter = 0;
        for (const auto &vertex_1 : vertices_1) {
            for (const auto &vertex_2 : vertices_2) {
                if (vertex_1 == vertex_2) {
                    counter++;
                    break;
                }
            }
        }

        //4.) save best rotation
        if (counter > overlaying_vertex_count) {
            overlaying_vertex_count = counter;
            // We have to do this because we first rotate and than count overlapping vertices
            ideal_rotation = (i + 1) % 4;
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

        auto pair = find_ideal_rotation(vertices, other_vertices);
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

void Graph::print() {
    std::cout << "=======================\n";

    unsigned int i = 0;
    for (const auto &edge : base_graph_) {
        std::cout << "Edge: " << i << " -> x_0:" << edge.first.get_coords().first << " y_0:"
                  << edge.first.get_coords().second << " " << "x_1:" << edge.second.get_coords().first << " y_1:"
                  << edge.second.get_coords().second << "\n";
        i++;
    }
}

auto Graph::difference(const std::shared_ptr<Graph> &other_graph) -> std::shared_ptr<Graph> {
    auto vertices = extract_vertices().size();
    auto other_vertices = other_graph->extract_vertices().size();
    std::vector<Edge> new_graph;

    // Normally not needed because both graphs would have the same amount of edges
    if (vertices > other_vertices) {
        new_graph = base_graph_;
        for (const auto &ver : other_graph->base_graph_) {
            new_graph.erase(std::remove(new_graph.begin(), new_graph.begin(), ver));
        }
    } else {
        new_graph = other_graph->base_graph_;
        for (const auto &ver : base_graph_) {
            new_graph.erase(std::remove(new_graph.begin(), new_graph.begin(), ver));
        }
    }
    auto graph = std::make_shared<Graph>();
    graph->base_graph_ = new_graph;

    return graph;
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

    auto cross = [](const Edge &e1, const Edge &e2) -> bool {
        float min_x = std::max(e1.first.get_x(), e2.first.get_x());
        float max_x = std::min(e1.second.get_x(), e2.second.get_x());
        // In this interval the two edges have to cross each other
        // Now we use the points to generate the linear function and calculate a possible intersection

        // Linear Functions
        float m_0 = (e1.second.get_y() - e1.first.get_y()) / (e1.second.get_x() - e1.first.get_x());
        float n_0 = e1.first.get_y() - m_0 * e1.first.get_x();
        float m_1 = (e2.second.get_y() - e2.first.get_y()) / (e2.second.get_x() - e2.first.get_x());
        float n_1 = e2.first.get_y() - m_0 * e2.first.get_x();


        return not((m_0 * min_x + n_0 > m_1 * min_x + n_1 and m_0 * max_x + n_0 > m_1 * max_x + n_1) or
                   (m_0 * min_x + n_0 < m_1 * min_x + n_1 and m_0 * max_x + n_0 < m_1 * max_x + n_1));

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
