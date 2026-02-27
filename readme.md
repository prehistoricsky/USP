The repository of USP
To compile, just "g++ main.cpp -o main"
To partition a graph, use the command "main path_to_graph_file k" in your shell, where k is the number of partition
The defalut file of graph is written in binary. The first four bytes represents the 32-bit number of vertices, and the 5-12 bytes represents the 64-bit number of edges, while the following bytes represent edges, each with two 32-bit number representing the two vertices' id of an edge. The vertices' id is from 0 to n-1, where n is the number of vertices.
To read graph with other format, please implement the class GraphScanner in GraphScanner.cpp and use it in main.cpp.