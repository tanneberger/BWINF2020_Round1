//
// Created by revol-xut on 07.10.20.
//

#include <iostream>
#include <string>
#include <algorithm>
#include <optional>
#include <memory>
#include <unordered_set>
#include "src/graph.hpp"

auto generate_unsigned_int() -> unsigned int{
    std::string num_text;
    while (true){
        getline(std::cin, num_text);
        std::string::const_iterator it = num_text.begin();
        while (it != num_text.end() && std::isdigit(*it)) ++it;
        if (!num_text.empty() && it == num_text.end()){
            break;
        }
    }

    return std::stoi(num_text);
}

auto generate_float() -> std::optional<float>{
    std::string num_text;

    getline(std::cin, num_text);
    std::string::const_iterator it = num_text.begin();
    while (it != num_text.end() && (std::isdigit(*it) || *it == '.')) ++it;
    if (!num_text.empty() && it == num_text.end()){
        return std::stof(num_text);
    }
    return std::nullopt;
}


void construct_graph(const std::shared_ptr<Graph>& graph, unsigned int edge_count, const std::vector<Vertex>& vertices){
    unsigned int i = 0;
    while (i < edge_count){
        std::cout << "Edge: " << i << std::endl;
        unsigned int vertex_0 = generate_unsigned_int();
        unsigned int vertex_1 = generate_unsigned_int();

        if (vertex_0 < vertices.size() or vertex_1 < vertices.size()){
            graph->add_edge(make_edge(vertices.at(vertex_0), vertices.at(vertex_1)));
            i++;
        }else{
            std::cout << "Invalid Ids !" << std::endl;
        }
    }
}

void input_vertices(std::vector<Vertex>& vertices){
    unsigned int i = 0;
    while (true){
        std::cout << "Vertex: " << i << std::endl;
        std::cout << "x:";
        auto x_0 = generate_float();
        if (not x_0.has_value()){
            return;
        }
        std::cout << "y:";
        auto y_0 = generate_float();
        if (not y_0.has_value()){
            return;
        }

        vertices.emplace_back(x_0.value(), y_0.value());
        i++;
    }
}


int main(){
    std::cout << "Aufgabe 4 des Bundeswettbewerbs Informatik 2020" << std::endl;
    std::cout << "===============================================\n" << std::endl << std::endl;
    std::cout << "Geben sie ein in wie vielen Umformungsschritten der Graph transformiert werden soll" << std::endl;

    auto move_counter = generate_unsigned_int();

    auto graph1 = std::make_shared<Graph>();
    auto graph2 = std::make_shared<Graph>();

    std::cout << "Nun geben sie die Knoten der beiden Graphen ein. Diese Knoten bekommen jeweils eine ID die später für die Eingabe der Kanten genutzt wird." << std::endl;
    std::cout << "Es reicht eine präzision von 3 Nachkomma Stellen aus." << std::endl;
    std::cout << "Kleine Hilfe:" << std::endl;
    std::cout << "cos(30°) = 0.8661 cos(60°) = 0.5 \nsin(30°) = 0.5 sin(60°) = 0.8661" << std::endl;
    std::vector<Vertex> vertices;
    input_vertices(vertices);

    std::cout << "Die eingegebenen Knoten:" << std::endl;
    unsigned int i = 0;
    for (const auto& vertex: vertices){
        std::cout << "Vertex: " << i << " x = " << vertex.get_x() << " y = " << vertex.get_y() << std::endl;
        i++;
    }

    std::cout << "Eingabe der Kanten mit hilfe der IDs die gerade für die Knoten vergeben wurden" << std::endl;

    std::cout << "Eingabe für Graph1" << std::endl;
    std::cout << "Wie viele Kanten soll der Graph1 besitzen ?" << std::endl;
    auto edge_count_1 = generate_unsigned_int();
    construct_graph(graph1, edge_count_1, vertices);
    std::cout << "Eingabe für Graph2" << std::endl;
    std::cout << "Wie viele Kanten soll der Graph2 besitzen ?" << std::endl;
    auto edge_count_2 = generate_unsigned_int();
    construct_graph(graph2, edge_count_2, vertices);

    // Example from the documentation hard coded
   /* graph1->add_edge(make_edge(Vertex(0, 0), Vertex(1, 0)));
    graph1->add_edge(make_edge(Vertex(0, 0), Vertex(0, 1)));
    graph1->add_edge(make_edge(Vertex(0, 1), Vertex(0, 2)));
    graph1->add_edge(make_edge(Vertex(1, 0), Vertex(0.5, 0.8661)));
    graph1->add_edge(make_edge(Vertex(0.8661, 1.5), Vertex(0, 1)));
    graph1->add_edge(make_edge(Vertex(0.8661, 1.5), Vertex(0, 2)));

    graph2->add_edge(make_edge(Vertex(0, 0), Vertex(0, 1)));
    graph2->add_edge(make_edge(Vertex(0, 0), Vertex(0.8661, 0.5)));
    graph2->add_edge(make_edge(Vertex(0, 1), Vertex(0.8661, 0.5)));
    graph2->add_edge(make_edge(Vertex(0, 1), Vertex(0.8661, 1.5)));
    graph2->add_edge(make_edge(Vertex(0.8661, 1.5), Vertex(0, 2)));
    graph2->add_edge(make_edge(Vertex(0.8661, 0.5), Vertex(0.8661, 1.5))); */


    if( graph1->size() != graph2->size()){
        std::cout << "Graph1 und Graph2 haben unterschiedlich viele Kanten es ist somit nicht möglich G1 in G2 umzuwandeln !" << std::endl;
        std::cout << "Wollen sind sie forfahren und die ideale ausrichtung für eine Umformung finden ? [J/n]" << std::endl;
        std::string continue_algorithm;
        getline(std::cin, continue_algorithm);

        if(std::unordered_set<std::string>({"N", "n", "no", "No", "nein", "Nein"}).contains(continue_algorithm)){
            return 0;
        }
    }

    if(not graph1->valid_match_stick_graph() ){
        std::cout << "Graph 1 ist kein valider Streichholzgraph !" << std::endl;
       // return 0;
    }
    if(not graph2->valid_match_stick_graph() ){
        std::cout << "Graph 2 ist kein valider Streichholzgraph !" << std::endl;
    //    return 0;
    }

    graph1->find_ideal_alignment(graph2);
    graph1->difference(graph2);

    //graph_1 are edges that need to be moved
    //graph_2 are edges that need to be placed
    if(graph1->size() < graph2->size() or graph1->size() > move_counter or graph2->size() > move_counter){
        std::cout << "Es ist nicht möglich den Streichholzgraphen 1 in den Streichholzgraphen 2 umzuwandeln !" << std::endl;
        return 0;
    }

    for (unsigned int i = 0; i < std::min(graph1->size(), graph2->size()); i++){
        std::cout << "Edge:" << i << " / " << graph1->get_edge(i) << " => " << graph2->get_edge(i) << std::endl;
    }


    return 0;
}