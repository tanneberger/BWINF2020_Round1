//
// Created by revol-xut on 20.10.20.
//

#include <limits>
#include <cmath>
#include "vertex.hpp"

Vertex::Vertex(const std::pair<float, float> &coords) {
    coordinates_ = coords;
}

Vertex::Vertex(float x, float y) {
    coordinates_ = std::make_pair(x, y);
}

Vertex::~Vertex() = default;

void Vertex::rotate() {
    rotate_by_degree(1);
}

void Vertex::rotate_by_degree(unsigned int rot) {
    // Does a vector space transformation with every individual vector stored in this matrix
    //  cos(r) -1 * sin(r)
    //  sin(r)  cos(r)
    // Afterwards it brings every vector into the first quadrant

    float radians = (M_PI * (rot % 12)) / 6;
    float x_0 = std::cos(radians), x_1 = std::sin(radians) , y_0 = -1.0 * std::sin(radians), y_1 = std::cos(radians);
    float temporary_x = coordinates_.first;

    coordinates_.first = x_0 * temporary_x + y_0 * coordinates_.second;
    coordinates_.second = x_1 * temporary_x + y_1 * coordinates_.second;
}

void Vertex::add_vector(const std::pair<float, float> &coords) {
    coordinates_.first += coords.first;
    coordinates_.second += coords.second;
}

auto Vertex::get_coords() const -> std::pair<float, float> {
    return coordinates_;
}

auto Vertex::operator==(const Vertex &other) const -> bool {
    auto round = [](float val) -> float{
        return std::round( val * 1000.0 ) / 1000.0;
    };
    return (round(coordinates_.first) == round(other.coordinates_.first)) and (round(coordinates_.second) == round(other.coordinates_.second));
}

void Vertex::mirror_x() {
    coordinates_.first -= 2 * coordinates_.first;
}

void Vertex::mirror_y() {
    coordinates_.second -= 2 * coordinates_.second;
}

auto Vertex::get_x() const -> float {
    return coordinates_.first;
}

auto Vertex::get_y() const -> float {
    return coordinates_.second;
}
