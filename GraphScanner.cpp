#include<stdio.h>
#include<vector>
#include<memory>
#pragma once



class GraphScanner{
    public:
        virtual ~GraphScanner(){};
        virtual bool next(int &a, int &b) = 0;
        virtual int verticesCnt() = 0;
        virtual long long edgesCnt() = 0;
        virtual void resetStream() = 0;
};



class GraphScannerWithDegree : public GraphScanner{
    public:
        virtual ~GraphScannerWithDegree(){};
        virtual int degree(int point) = 0;
};



class GraphScannerWithInAndOutDegree : public GraphScannerWithDegree{
    public:
        virtual ~GraphScannerWithInAndOutDegree(){};
        virtual int inDegree(int point) = 0;
        virtual int outDegree(int point) = 0;
};



class StreamGraphScanner : public GraphScanner{
    public:
        StreamGraphScanner(char *input_file){
            fp_ = fopen(input_file, "r");
            fscanf(fp_, "%d %lld", &vertices_cnt_, &edges_cnt_);
        }

        StreamGraphScanner(StreamGraphScanner &sgs) = delete;

        ~StreamGraphScanner(){
            if(fp_ != NULL) fclose(fp_);
        }

        bool next(int &a, int &b) override {
            return fscanf(fp_, "%d %d", &a, &b) != EOF;
        }

        int verticesCnt() override {
            return vertices_cnt_;
        }

        long long edgesCnt() override {
            return edges_cnt_;
        }

        void resetStream() override {
            rewind(fp_);
            fscanf(fp_, "%d %lld", &vertices_cnt_, &edges_cnt_);
        }

    private:
        int vertices_cnt_;
        long long edges_cnt_;
        FILE *fp_ = NULL;
};



class StreamGraphScannerWithDegree : public GraphScannerWithDegree{
    public:
        StreamGraphScannerWithDegree(char *input_file, char *degree_file){
            sgs_ = new StreamGraphScanner(input_file);
            FILE *fp = fopen(degree_file, "r");
            int points_cnt;
            fscanf(fp, "%d", &points_cnt);
            degree_.resize(points_cnt);
            for(int i = 0; i < points_cnt; i++){
                fscanf(fp, "%d", &degree_[i]);
            }
            fclose(fp);
        }

        StreamGraphScannerWithDegree(StreamGraphScannerWithDegree &sgswd) = delete;

        ~StreamGraphScannerWithDegree(){
            if(sgs_ != NULL) delete sgs_;
        }

        bool next(int &a, int &b) override {
            return sgs_->next(a, b);
        }

        int verticesCnt() override {
            return sgs_->verticesCnt();
        }

        long long edgesCnt() override {
            return sgs_->edgesCnt();
        }

        void resetStream() override {
            sgs_->resetStream();
        }

        int degree(int point) override {
            return degree_.at(point);
        }

    private:
        StreamGraphScanner *sgs_ = NULL;
        std::vector<int> degree_;
};



class BinaryStreamGS : public GraphScanner{
    public:
        BinaryStreamGS(char *input_file){
            fp_ = fopen(input_file, "rb");
            // buffer_ = (char*)malloc(buffer_size_);
            // setvbuf(fp_, buffer_, _IOFBF, buffer_size_);
            fread(&vertices_cnt_, sizeof(int), 1, fp_);
            fread(&edges_cnt_, sizeof(long long), 1, fp_);
        }

        BinaryStreamGS(BinaryStreamGS &bsgs) = delete;

        ~BinaryStreamGS(){
            if(fp_ != NULL) fclose(fp_);
        }

        bool next(int &a, int &b) override {
            size_t res_a = fread(&a, sizeof(int), 1, fp_);
            size_t res_b = fread(&b, sizeof(int), 1, fp_);
            return (res_a == 1) && (res_b == 1);
        }

        int verticesCnt() override {
            return vertices_cnt_;
        }

        long long edgesCnt() override{
            return edges_cnt_;
        }

        void resetStream() override {
            rewind(fp_);
            fread(&vertices_cnt_, sizeof(int), 1, fp_);
            fread(&edges_cnt_, sizeof(long long), 1, fp_);
        }

    private:
        int vertices_cnt_;
        long long edges_cnt_;
        FILE *fp_ = NULL;
        // const static int buffer_size_ = 1024 * 1024 * 32;
        // char *buffer_;
};



class BinaryBufferStreamGS : public GraphScanner{
    public:
        BinaryBufferStreamGS(char *input_file, int buf_size){
            fp_ = fopen(input_file, "rb");
            size_ = buf_size;
            buf_ = std::make_unique<char[]>(size_);
            end_ = fread(buf_.get(), sizeof(char), size_, fp_);
            pr_ = 0;
            nextInt(vertices_cnt_);
            nextLongLong(edges_cnt_);
        }

        BinaryBufferStreamGS(char *input_file){
            // default 32mb buffer
            new(this)BinaryBufferStreamGS(input_file, 1024 * 1024 * 32);
        }

        BinaryBufferStreamGS(BinaryBufferStreamGS &bsgs) = delete;

        ~BinaryBufferStreamGS(){
            if(fp_ != NULL) fclose(fp_);
        }

        bool next(int &a, int &b) override {
            return (nextInt(a)) && (nextInt(b));
        }

        int verticesCnt() override {
            return vertices_cnt_;
        }

        long long edgesCnt() override{
            return edges_cnt_;
        }

        void resetStream() override {
            rewind(fp_);
            end_ = fread(buf_.get(), sizeof(char), size_, fp_);
            pr_ = 0;
            nextInt(vertices_cnt_);
            nextLongLong(edges_cnt_);
        }

    private:
        int vertices_cnt_;
        long long edges_cnt_;
        FILE *fp_ = NULL;
        std::unique_ptr<char[]> buf_;
        int size_;
        int pr_;
        int end_;


        bool nextInt(int &a){
            if(pr_ + sizeof(int) > end_){
                if(end_ < size_) return false;
                int remain = end_ - pr_;
                for(int i = pr_; i < end_; i++){
                    buf_[i - pr_] = buf_[i];
                }
                end_ = fread(buf_.get() + remain, sizeof(char), pr_, fp_);
                end_ += remain;
                pr_ = 0;
                if(sizeof(int) > end_) return false;
            }
            char *a_pr = (char*)&a;
            for(int i = 0; i < sizeof(int); i++){
                a_pr[i] = buf_[i + pr_];
            }
            pr_ += sizeof(int);
            return true;
        }


        bool nextLongLong(long long &a){
            if(pr_ + sizeof(long long) > end_){
                if(end_ < size_) return false;
                int remain = end_ - pr_;
                for(int i = pr_; i < end_; i++){
                    buf_[i - pr_] = buf_[i];
                }
                end_ = fread(buf_.get() + remain, sizeof(char), pr_, fp_);
                end_ += remain;
                pr_ = 0;
                if(sizeof(long long) > end_) return false;
            }
            char *a_pr = (char*)&a;
            for(int i = 0; i < sizeof(long long); i++){
                a_pr[i] = buf_[i + pr_];
            }
            pr_ += sizeof(long long);
            return true;
        }
};



class BinaryStreamGSWD : public GraphScannerWithDegree{
    public:
        BinaryStreamGSWD(char *input_file, char *degree_file){
            bsgs_ = new BinaryStreamGS(input_file);
            FILE *fp = fopen(degree_file, "rb");
            int points_cnt = bsgs_->verticesCnt();
            degree_.resize(points_cnt);
            for(int i = 0; i < points_cnt; i++){
                fread(&degree_[i], sizeof(int), 1, fp);
            }
            fclose(fp);
        }

        BinaryStreamGSWD(char *input_file){
            bsgs_ = new BinaryStreamGS(input_file);
            int points_cnt = bsgs_->verticesCnt();
            degree_.resize(points_cnt);
            int source, target;
            while(bsgs_->next(source, target)){
                degree_[source]++;
                degree_[target]++;
            }
            bsgs_->resetStream();
        }

        BinaryStreamGSWD(BinaryStreamGSWD &sgswd) = delete;

        ~BinaryStreamGSWD(){
            if(bsgs_ != NULL) delete bsgs_;
        }

        bool next(int &a, int &b) override {
            return bsgs_->next(a, b);
        }

        int verticesCnt() override {
            return bsgs_->verticesCnt();
        }

        long long edgesCnt() override {
            return bsgs_->edgesCnt();
        }

        void resetStream() override {
            bsgs_->resetStream();
        }

        int degree(int point) override {
            return degree_.at(point);
        }

    private:
        BinaryStreamGS *bsgs_ = NULL;
        std::vector<int> degree_;
};



class BinaryBufferStreamGSWD : public GraphScannerWithDegree{
    public:
        BinaryBufferStreamGSWD(char *input_file, char *degree_file){
            bsgs_ = new BinaryBufferStreamGS(input_file);
            FILE *fp = fopen(degree_file, "rb");
            int points_cnt = bsgs_->verticesCnt();
            degree_.resize(points_cnt);
            for(int i = 0; i < points_cnt; i++){
                fread(&degree_[i], sizeof(int), 1, fp);
            }
            fclose(fp);
        }

        BinaryBufferStreamGSWD(char *input_file){
            bsgs_ = new BinaryBufferStreamGS(input_file);
            int points_cnt = bsgs_->verticesCnt();
            degree_.resize(points_cnt);
            int source, target;
            while(bsgs_->next(source, target)){
                degree_[source]++;
                degree_[target]++;
            }
            bsgs_->resetStream();
        }

        BinaryBufferStreamGSWD(BinaryBufferStreamGSWD &sgswd) = delete;

        ~BinaryBufferStreamGSWD(){
            if(bsgs_ != NULL) delete bsgs_;
        }

        bool next(int &a, int &b) override {
            return bsgs_->next(a, b);
        }

        int verticesCnt() override {
            return bsgs_->verticesCnt();
        }

        long long edgesCnt() override {
            return bsgs_->edgesCnt();
        }

        void resetStream() override {
            bsgs_->resetStream();
        }

        int degree(int point) override {
            return degree_.at(point);
        }

    private:
        BinaryBufferStreamGS *bsgs_ = NULL;
        std::vector<int> degree_;
};



class BinaryBufferStreamGSWIOD : public GraphScannerWithInAndOutDegree{
    public:
        BinaryBufferStreamGSWIOD(char *input_file){
            bsgs_ = new BinaryBufferStreamGS(input_file);
            int points_cnt = bsgs_->verticesCnt();
            in_degree_.resize(points_cnt);
            out_degree_.resize(points_cnt);
            int source, target;
            while(bsgs_->next(source, target)){
                out_degree_[source]++;
                in_degree_[target]++;
            }
            bsgs_->resetStream();
        }

        BinaryBufferStreamGSWIOD(BinaryBufferStreamGSWIOD &sgswd) = delete;

        ~BinaryBufferStreamGSWIOD(){
            if(bsgs_ != NULL) delete bsgs_;
        }

        bool next(int &a, int &b) override {
            return bsgs_->next(a, b);
        }

        int verticesCnt() override {
            return bsgs_->verticesCnt();
        }

        long long edgesCnt() override {
            return bsgs_->edgesCnt();
        }

        void resetStream() override {
            bsgs_->resetStream();
        }

        int degree(int point) override {
            return in_degree_.at(point) + out_degree_.at(point);
        }

        int inDegree(int point) override {
            return in_degree_.at(point);
        }

        int outDegree(int point) override {
            return out_degree_.at(point);
        }

    private:
        BinaryBufferStreamGS *bsgs_ = NULL;
        std::vector<int> in_degree_;
        std::vector<int> out_degree_;
};
