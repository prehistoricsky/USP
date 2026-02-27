#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include"GraphScanner.cpp"
#include"PartitionWriter.cpp"
#include"USPPartitioner.cpp"



int main(int argc, char *argv[]){
    auto start = clock();
    char *input_file_path = argv[1];
    int k = std::stoi(argv[2]);
    char *output_file = NULL;
    if(argc > 3) output_file = argv[3];
    printf("input file: %s\nk = %d\n", input_file_path, k);
    
    BinaryBufferStreamGSWIOD bsgswd(input_file_path);
    // std::vector<int> map;
    // map.resize(bsgswd.verticesCnt());
    // for(int i = 0; i < map.size(); i++){
    //     map[i] = i;
    // }
    // std::shuffle(map.begin(), map.end(), std::default_random_engine());
    // std::vector<std::pair<int, int>> edges;
    // int source, target;
    // while(bsgswd.next(source, target)){
    //     edges.emplace_back(map[source], map[target]);
    // }
    // struct PairFirstComp{
    //     bool operator()(const std::pair<int, int> &a, const std::pair<int, int> &b){
    //         return a.first < b.first || (a.first == b.first && a.second < b.second);
    //     }
    // };
    // std::sort(edges.begin(), edges.end(), PairFirstComp());
    // char buf[1024];
    // sprintf(buf, "%s.shuffle", input_file_path);
    // FILE *fp = fopen(buf, "wb");
    // int vcnt = bsgswd.verticesCnt();
    // long long ecnt = bsgswd.edgesCnt();
    // fwrite(&vcnt, sizeof(int), 1, fp);
    // fwrite(&ecnt, sizeof(long long), 1, fp);
    // for(auto e : edges){
    //     fwrite(&(e.first), sizeof(int), 1, fp);
    //     fwrite(&(e.second), sizeof(int), 1, fp);
    // }
    // fclose(fp);
    printf("vertices: %d, edges: %lld\n", bsgswd.verticesCnt(), bsgswd.edgesCnt());
    CalcRepSEPW *crsepw;
    if(output_file == NULL){
        crsepw = new CalcRepSEPW(bsgswd.verticesCnt(), k);
    } else {
        crsepw = new OutputSEPW(bsgswd.verticesCnt(), k, output_file);
    }
    USPPartitioner partitioner{&bsgswd, crsepw, k, 1};
    partitioner.run();
    printf("calc complete!\n");
    printf("%f\n", crsepw->replicator());
    delete crsepw;
    auto end = clock();
    printf("using time: %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
    return 0;
}