#include<stdio.h>
#include<vector>
#include<algorithm>
#include<unordered_map>

#pragma once



template<typename _Elem, typename _Comp>
class ArrayWithHeap{
    public:
        ArrayWithHeap(){}

        ArrayWithHeap(int size){
            size_ = size;
            arr_.resize(size_);
            heapid_to_arrid_.resize(size_);
            arrid_to_heapid_.resize(size_);
            for(int i = 0; i < size_; i++){
                heapid_to_arrid_[i] = i;
                arrid_to_heapid_[i] = i;
            }
        }

        ArrayWithHeap(std::vector<_Elem> &arr){
            size_ = arr.size();
            // arr_.resize(size_);
            arr_.swap(arr);
            heapid_to_arrid_.resize(size_);
            arrid_to_heapid_.resize(size_);
            for(int i = 0; i < size_; i++){
                heapid_to_arrid_[i] = i;
                arrid_to_heapid_[i] = i;
            }
            int mid = size_ / 2;
            for(int i = mid; i >= 0; i--){
                push_down(i);
            }
        }

        const _Elem at(int id){
            if(id >= size_ || id < 0){
                printf("ArrayWithHeap::at: id out of range\nid = %d, size = %d\n", id, size_);
                exit(-1);
            }
            return arr_.at(id);
        }

        void modify(int id, _Elem elem){
            if(id >= size_ || id < 0){
                printf("ArrayWithHeap::modify: id out of range\nid = %d, size = %d\n", id, size_);
                exit(-1);
            }
            arr_[id] = elem;
            int heap_id = arrid_to_heapid_.at(id);
            push_up(heap_id);
            push_down(heap_id);
        }

        const int get_top_id(){
            if(size_ == 0){
                printf("ArrayWithHeap::get_top_id: heap is empty\n");
                exit(-1);
            }
            return heapid_to_arrid_.at(0);
        }

        const _Elem get_top_elem(){
            if(size_ == 0){
                printf("ArrayWithHeap::get_top_elem: heap is empty\n");
                exit(-1);
            }
            return arr_.at(get_top_id());
        }

        void push_back(_Elem elem){
            arr_.emplace_back(elem);
            heapid_to_arrid_.emplace_back(size_);
            arrid_to_heapid_.emplace_back(size_);
            size_++;
            push_up(size_ - 1);
        }
        
    private:
        std::vector<_Elem> arr_;
        std::vector<int> heapid_to_arrid_;
        std::vector<int> arrid_to_heapid_;
        int size_;

        void swap_in_arr(int id1, int id2){
            _Elem e1 = arr_.at(id1);
            _Elem e2 = arr_.at(id2);
            arr_[id1] = e2;
            arr_[id2] = e1;
            int heap_id1 = arrid_to_heapid_.at(id1);
            int heap_id2 = arrid_to_heapid_.at(id2);
            arrid_to_heapid_[id1] = heap_id2;
            arrid_to_heapid_[id2] = heap_id1;
            heapid_to_arrid_[heap_id1] = id2;
            heapid_to_arrid_[heap_id2] = id1;
        }

        void swap_in_heap(int id1, int id2){
            int arr_id1 = heapid_to_arrid_.at(id1);
            int arr_id2 = heapid_to_arrid_.at(id2);
            heapid_to_arrid_[id1] = arr_id2;
            heapid_to_arrid_[id2] = arr_id1;
            arrid_to_heapid_[arr_id1] = id2;
            arrid_to_heapid_[arr_id2] = id1;
        }

        void push_down(int heap_index){
            int c1, c2;
            int next_index = heap_index;
            while(heap_index < size_){
                c1 = (heap_index + 1) * 2 - 1;
                c2 = (heap_index + 1) * 2 + 1 - 1;
                _Elem e, e1, e2;
                if(c1 >= size_) return;
                if(c2 >= size_){
                    e = arr_.at(heapid_to_arrid_.at(heap_index));
                    e1 = arr_.at(heapid_to_arrid_.at(c1));
                    if(!_Comp()(e, e1)){
                        swap_in_heap(heap_index, c1);
                        next_index = c1;
                    }
                } else {
                    e = arr_.at(heapid_to_arrid_.at(heap_index));
                    e1 = arr_.at(heapid_to_arrid_.at(c1));
                    e2 = arr_.at(heapid_to_arrid_.at(c2));
                    _Elem e_target = _Comp()(e1, e2) ? e1 : e2;
                    int c_target = _Comp()(e1, e2) ? c1 : c2;
                    if(!_Comp()(e, e_target)){
                        swap_in_heap(heap_index, c_target);
                        next_index = c_target;
                    }
                }
                if(heap_index == next_index) return;
                heap_index = next_index;
            }
        }

        void push_up(int heap_index){
            int parent_id = (heap_index + 1) / 2 - 1;
            int now_id = heap_index;
            while(true){
                if(parent_id < 0) return;
                _Elem parent_elem = arr_.at(heapid_to_arrid_.at(parent_id));
                _Elem now_elem = arr_.at(heapid_to_arrid_.at(now_id));
                if(_Comp()(parent_elem, now_elem)) return;
                swap_in_heap(parent_id, now_id);
                now_id = parent_id;
                parent_id = (now_id + 1) / 2 - 1;
            }
        }
};



// struct Comp{
//     bool operator()(int a, int b){
//         return a < b;
//     }
// };



// int main(){
//     int size;
//     std::vector<int> vec;
//     scanf("%d", &size);
//     for(int i = 0; i < size; i++){
//         int a;
//         scanf("%d", &a);
//         vec.emplace_back(a);
//     }
//     ArrayWithHeap<int, Comp> awh(vec);
//     for(int i = 0; i < size; i++){
//         printf("%d ", awh.get_top_elem());
//         awh.modify(awh.get_top_id(), INT_MAX);
//     }
//     printf("\n");
//     return 0;
// }
