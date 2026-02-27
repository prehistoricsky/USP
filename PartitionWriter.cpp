#include<stdio.h>
#include<vector>
#include"bitmap.cpp"
#pragma once



class StreamEdgePartitionWriter{
    public:
        virtual ~StreamEdgePartitionWriter(){}
        virtual void next(const std::pair<int, int> &edge, int res) = 0;
};



class BinaryFileSEPW : public StreamEdgePartitionWriter{
    public:
        BinaryFileSEPW(char *output_file, int k){
            out_fp_ = fopen(output_file, "wb");
            fwrite(&k, sizeof(int), 1, out_fp_);
        }

        BinaryFileSEPW(BinaryFileSEPW &bfsepw) = delete;

        ~BinaryFileSEPW(){
            if(out_fp_ != NULL) fclose(out_fp_);
        }

        void next(const std::pair<int, int> &edge, int res) override {
            if(out_fp_ == NULL){
                throw "BinaryFileSEPW is Ended before writing\n";
            }
            fwrite(&res, sizeof(int), 1, out_fp_);
        }

        void end(){
            if(out_fp_ != NULL) fclose(out_fp_);
            out_fp_ = NULL;
        }

    private:
        FILE *out_fp_ = NULL;
};



class CalcRepSEPW : public StreamEdgePartitionWriter{
    public:
        CalcRepSEPW(int vertices_cnt, int k){
            vertices_cnt_ = vertices_cnt;
            k_ = k;
        }

        CalcRepSEPW(CalcRepSEPW &crsepw) = delete;

        ~CalcRepSEPW(){}

        void next(const std::pair<int, int> &edge, int res) override {
            if(!is_initialized_){
                vertice_in_partition_.resize(k_, BitMap(vertices_cnt_));
                is_initialized_ = true;
            }
            int a = edge.first;
            int b = edge.second;
            if(vertice_in_partition_[res].at(a) == false){
                replicates_++;
                vertice_in_partition_[res].set(a, true);
            }
            if(vertice_in_partition_[res].at(b) == false){
                replicates_++;
                vertice_in_partition_[res].set(b, true);
            }
        }

        double replicator(){
            return (double)replicates_ / vertices_cnt_;
        }

        void clear(){
            std::vector<BitMap>().swap(vertice_in_partition_);
            is_initialized_ = false;
            replicates_ = 0;
        }

    private:
        std::vector<BitMap> vertice_in_partition_;
        int vertices_cnt_;
        int k_;
        bool is_initialized_ = false;
        long long replicates_ = 0;
};


class OutputSEPW : public CalcRepSEPW{
    public:
        OutputSEPW(int vertices_cnt, int k, char *output_file) : CalcRepSEPW(vertices_cnt, k) {
            fp_ = fopen(output_file, "w");
        }

        OutputSEPW(OutputSEPW &osepw) = delete;

        ~OutputSEPW(){
            fclose(fp_);
        }

        void next(const std::pair<int, int> &edge, int res) override {
            CalcRepSEPW::next(edge, res);
            fprintf(fp_, "%d %d %d\n", edge.first, edge.second, res);
        }

        double replicator(){
            return CalcRepSEPW::replicator();
        }

        void clear(){
            CalcRepSEPW::clear();
        }

    private:
        FILE *fp_;
};