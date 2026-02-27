#include<stdio.h>
#include<stdlib.h>
#include<memory>
#pragma once


class BitMap{
    public:
        BitMap(){
            data_ = NULL;
            size_ = 0;
        }

        BitMap(int size) : size_(size){
            int bits = (size + 7) / 8;
            data_ = std::make_unique<char[]>(bits);
            for(int i = 0; i < bits; i++) data_[i] = 0;
        }

        BitMap(const BitMap &bm){
            size_ = bm.size_;
            int bits = (size_ + 7) / 8;
            data_ = std::make_unique<char[]>(bits);
            for(int i = 0; i < bits; i++) data_[i] = bm.data_[i];
        }

        void operator=(const BitMap &bm){
            size_ = bm.size_;
            int bits = (size_ + 7) / 8;
            data_ = std::make_unique<char[]>(bits);
            for(int i = 0; i < bits; i++) data_[i] = bm.data_[i];
        }

        bool at(int loc) const {
            if(loc >= size_){
                printf("BitMap::at: loc %d >= size %d!\n", loc, size_);
                exit(-1);
            }
            int block_id = loc / 8;
            int offset = loc - block_id * 8;
            char block = data_[block_id];
            char res = (block >> offset) & 1;
            return res;
        }

        void set(int loc, bool val){
            if(loc >= size_){
                printf("BitMap::set: loc %d >= size %d!\n", loc, size_);
                exit(-1);
            }
            int block_id = loc / 8;
            int offset = loc - block_id * 8;
            char mask = 1 << offset;
            if(val){
                data_[block_id] |= mask;
            } else {
                data_[block_id] &= ~mask;
            }
        }

        char get_char(int block_id) const {
            if(block_id >= (size_ + 7) / 8){
                printf("BitMap::get_char: block %d out of range!\n", block_id);
                exit(-1);
            }
            return data_[block_id];
        }

        void set_char(int block_id, char val){
            if(block_id >= (size_ + 7) / 8){
                printf("BitMap::set_char: block %d out of range!\n", block_id);
                exit(-1);
            }
            data_[block_id] = val;
        }

        int get_map_size() const {
            return size_;
        }

        bool is_empty() const {
            for(int i = 0; i < (size_ + 7) / 8; i++){
                if(data_[i]) return false;
            }
            return true;
        }

        BitMap intersect(const BitMap &bm){
            if(bm.get_map_size() != size_){
                printf("BitMap::intersect: map_size aren't equal!\nthe size are %d %d\n", size_, bm.get_map_size());
                exit(-1);
            }
            BitMap res(size_);
            int bits = (size_ + 7) / 8;
            for(int i = 0; i < bits; i++){
                res.set_char(i, data_[i] & bm.get_char(i));
            }
            return res;
        }

        BitMap unionsect(const BitMap &bm){
            if(bm.get_map_size() != size_){
                printf("BitMap::unionsect: map_size aren't equal!\nthe size are %d %d\n", size_, bm.get_map_size());
                exit(-1);
            }
            BitMap res(size_);
            int bits = (size_ + 7) / 8;
            for(int i = 0; i < bits; i++){
                res.set_char(i, data_[i] | bm.get_char(i));
            }
            return res;
        }

    private:
        std::unique_ptr<char[]> data_;
        int size_;
};



