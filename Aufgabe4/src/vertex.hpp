//
// Created by revol-xut on 20.10.20.
//

#ifndef BWINF2020_AUFGABE4_VERTEX_HPP
#define BWINF2020_AUFGABE4_VERTEX_HPP

#include <tuple>

class Vertex {
public:
    explicit Vertex(const std::pair<float, float>& coords);
    Vertex(float x, float y);
    ~Vertex();

    void rotate();
    void rotate_by_degree(unsigned int rot);
    void add_vector(const std::pair<float, float>& coords);
    void mirror_x();
    void mirror_y();

    auto get_coords() const -> std::pair<float, float>;
    auto get_x() const -> float;
    auto get_y() const -> float;
    auto operator==(const Vertex& other) const -> bool;
private:

    std::pair<float, float> coordinates_;
};


#endif //BWINF2020_AUFGABE4_VERTEX_HPP
