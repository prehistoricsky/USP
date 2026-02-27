#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<random>
#include<algorithm>
#include<queue>
#include<unordered_map>
#include<limits.h>
#include<time.h>

#include"ArrayWithHeap.cpp"
#include"GraphScanner.cpp"
#include"PartitionWriter.cpp"
#include"MemoryGeter.cpp"


#pragma once



class USPPartitioner {
    public:
        USPPartitioner(GraphScannerWithInAndOutDegree *gswd, StreamEdgePartitionWriter *sepw, int k, double imbalanceFactor){
            gswd_ = gswd;
            sepw_ = sepw;
            k_ = k;
            max_load_ = 2 * imbalanceFactor * gswd_->edgesCnt() / k_;
        }


        void clusteringProcess(){
            auto start = clock();
            belong_.resize(gswd_->verticesCnt());
            belong_stack_.resize(gswd_->verticesCnt());
            cluster_target_.resize(gswd_->verticesCnt());
            in_degree_size_.resize(gswd_->verticesCnt());
            out_degree_size_.resize(gswd_->verticesCnt());
            match_count_.resize(gswd_->verticesCnt());
            tree_.resize(gswd_->verticesCnt());
            tree_edges_cnt_ = 0;

            for(int i = 0; i < gswd_->verticesCnt(); i++){
                belong_[i] = i;
                cluster_target_[i] = -1;
                in_degree_size_[i] = gswd_->inDegree(i);
                out_degree_size_[i] = gswd_->outDegree(i);
            }

            double mean_degree = (double)2 * gswd_->edgesCnt() / gswd_->verticesCnt();
            int source, target;
            int source_parent, target_parent;
            int remain_clusters = gswd_->verticesCnt();
            long long edges = 0;
            while(gswd_->next(source, target)){
                edges++;
                source_parent = searchInUnion(source);
                target_parent = searchInUnion(target);
                if(source_parent == target_parent) continue;
                if(true){
                // if(source_size + target_size <= max_load_){
                    int smaller_degree = std::min(gswd_->degree(source_parent), gswd_->degree(target_parent));
                    double edge_expectation = (double)in_degree_size_[source_parent] * out_degree_size_[target_parent] / gswd_->edgesCnt();
                    edge_expectation += (double)in_degree_size_[target_parent] * out_degree_size_[source_parent] / gswd_->edgesCnt();
                    double border_factor = smaller_degree / mean_degree;
                    double remain_p = border_factor / edge_expectation;
                    if(randomChoice(1 - remain_p)){
                        tryMatch(source_parent, target_parent);
                        tryMatch(target_parent, source_parent);
                        continue;
                    }
                    remain_clusters--;
                    if(gswd_->degree(target_parent) < gswd_->degree(source_parent)){
                        int temp = source_parent;
                        source_parent = target_parent;
                        target_parent = temp;
                    }
                    belong_[target_parent] = source_parent;
                    in_degree_size_[source_parent] += in_degree_size_[target_parent];
                    out_degree_size_[source_parent] += out_degree_size_[target_parent];
                    in_degree_size_[target_parent] = 0;
                    out_degree_size_[target_parent] = 0;
                    tree_[tree_edges_cnt_].first = source;
                    tree_[tree_edges_cnt_].second = target;
                    tree_edges_cnt_++;
                    unionMatch(source_parent, target_parent);
                } else {
                    tryMatch(source_parent, target_parent);
                    tryMatch(target_parent, source_parent);
                }
            }
            auto end = clock();
            printf("clusteringProcess = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
            printf("remainclusters: %d\n", remain_clusters);
            printf("edges = %lld\n", edges);

            for(int i = 0; i < gswd_->verticesCnt(); i++){
                in_degree_size_[i] += out_degree_size_[i];
            }
            cluster_size_.swap(in_degree_size_);

            printf("clusteringProcess memcost = %dkb\n", getMemoryCost());
            std::vector<long long>().swap(match_count_);
            std::vector<long long>().swap(out_degree_size_);
        }


        void unionProcess(){
            auto start = clock();

            std::pair<int, long long> p;

            ArrayWithHeap<long long, LongLongComp> cluster_size_with_heap{cluster_size_};
            int non_target_cnt = 0;
            for(int i = 0; i < gswd_->verticesCnt() - k_; i++){
                int source = cluster_size_with_heap.get_top_id();
                if(cluster_size_with_heap.at(source) == 0){
                    cluster_size_with_heap.modify(source, LONG_LONG_MAX);
                    continue;
                }
                int source_parent = searchInUnion(source);
                long long source_size = cluster_size_with_heap.at(source_parent);
                cluster_size_with_heap.modify(source_parent, LONG_LONG_MAX);
                int target = cluster_target_[source];
                if(target == -1 || searchInUnion(target) == source_parent){
                    if(target != -1 && searchInUnion(target) == source_parent) non_target_cnt++;
                    // target = bigger_clusters.get_top_elem().first;
                    target = cluster_size_with_heap.get_top_id();
                }
                int target_parent = searchInUnion(target);
                long long target_size = cluster_size_with_heap.at(target_parent);
                belong_[source_parent] = target_parent;
                target_size += source_size;
                cluster_size_with_heap.modify(target_parent, target_size);
                // if(cluster_in_bigger[target_parent] != -1){
                //     p.first = target_parent;
                //     p.second = target_size;
                //     bigger_clusters.modify(cluster_in_bigger[target_parent], p);
                // } else if(target_size > bigger_clusters.get_top_elem().second){
                //     p.first = target_parent;
                //     p.second = target_size;
                //     cluster_in_bigger[target_parent] = bigger_clusters.get_top_id();
                //     cluster_in_bigger[bigger_clusters.get_top_elem().first] = -1;
                //     bigger_clusters.modify(bigger_clusters.get_top_id(), p);
                // }
                tree_[tree_edges_cnt_].first = source;
                tree_[tree_edges_cnt_].second = target;
                tree_edges_cnt_++;
            }
            printf("union complete\n");
            final_clusters_.resize(k_);
            final_clusters_size_.resize(k_);
            // for(int i = 0; i < k_; i++){
            //     p = bigger_clusters.get_top_elem();
            //     final_clusters_[i] = p.first;
            //     final_clusters_size_[i] = p.second;
            //     p.second = LONG_LONG_MAX;
            //     bigger_clusters.modify(bigger_clusters.get_top_id(), p);
            // }
            for(int i = 0; i < k_; i++){
                final_clusters_[i] = cluster_size_with_heap.get_top_id();
                final_clusters_size_[i] = cluster_size_with_heap.at(final_clusters_[i]);
                cluster_size_with_heap.modify(final_clusters_[i], LONG_LONG_MAX);
            }
            for(int i = 0; i < gswd_->verticesCnt(); i++){
                searchInUnion(i);
            }
            auto end = clock();
            printf("unionProcess time = %f\n", (double)(end - start) / CLOCKS_PER_SEC);
            printf("non_target_cnt = %d\n", non_target_cnt);
            printf("unionProcess memcost = %dkb\n", getMemoryCost());

            std::vector<long long>().swap(cluster_size_);
            std::vector<int>().swap(cluster_target_);
            std::vector<int>().swap(belong_stack_);
        }


        void balanceProcess(){
            auto start = clock();
            std::vector<std::pair<int, int>> tree_edges;
            tree_edges.resize(2 * tree_edges_cnt_);
            std::pair<int, int> e;
            for(int i = 0; i < tree_edges_cnt_; i++){
                e = tree_[i];
                tree_edges[i] = e;
                int temp = e.first;
                e.first = e.second;
                e.second = temp;
                tree_edges[i + tree_edges_cnt_] = e;
            }
            printf("blanceProcess memcost = %dkb\n", getMemoryCost());
            std::vector<std::pair<int, int>>().swap(tree_);
            std::sort(tree_edges.begin(), tree_edges.end(), PairFirstComp());
            int pr = 0;
            std::vector<int> tree_start;
            tree_start.resize(gswd_->verticesCnt() + 1);
            for(int i = 0; i < gswd_->verticesCnt(); i++){
                while(pr < 2 * tree_edges_cnt_ && tree_edges[pr].first < i){
                    pr++;
                }
                tree_start[i] = pr;
            }
            tree_start[gswd_->verticesCnt()] = 2 * tree_edges_cnt_;

            std::vector<char> rec;
            rec.resize(gswd_->verticesCnt());
            int left = 0;
            int right = k_ - 1;
            int left_elem = final_clusters_[left];
            int right_elem = final_clusters_[right];
            int astar_elem;
            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, PairSecondCompReverse> queue;
            queue.emplace(right_elem, gswd_->degree(right_elem));
            rec[right_elem] = true;
            int points_cnt = 0;
            int queue_max_size = 0;
            while(left < right){
                if(queue.empty() || final_clusters_size_[right] <= max_load_){
                    right--;
                    right_elem = final_clusters_[right];
                    while(!queue.empty()) queue.pop();
                    queue.emplace(right_elem, gswd_->degree(right_elem));
                    rec[right_elem] = true;
                    continue;
                }
                astar_elem = queue.top().first;
                queue.pop();
                points_cnt++;
                for(int i = tree_start[astar_elem]; i < tree_start[astar_elem + 1]; i++){
                    int expand_elem = tree_edges[i].second;
                    if(!rec[expand_elem]){
                        queue.emplace(expand_elem, gswd_->degree(expand_elem));
                        rec[expand_elem] = true;
                        if(queue_max_size < queue.size()) queue_max_size = queue.size();
                    }
                }
                if(final_clusters_size_[left] >= max_load_){
                    left++;
                    left_elem = final_clusters_[left];
                }
                belong_[astar_elem] = left_elem;
                final_clusters_size_[left] += gswd_->degree(astar_elem);
                final_clusters_size_[right] -= gswd_->degree(astar_elem);
            }
            auto end = clock();
            printf("balanceProcess time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);

            printf("moved points cnt = %d\n", points_cnt);
            // printf("queue_max_size = %d\n", queue_max_size);

            long total_size = 0;
            for(int i = 0; i < k_; i++) total_size += final_clusters_size_[i];

            printf("blanceProcess memcost = %dkb\n", getMemoryCost());

            std::vector<int>().swap(final_clusters_);
            std::vector<long long>().swap(final_clusters_size_);
        }


        void partitionProcess(){
            auto start = clock();
            std::unordered_map<int, int> map;
            std::vector<long long> res_size;
            res_size.resize(k_);
            int cnt = 0;
            std::pair<int, int> p;
            for(int i = 0; i < gswd_->verticesCnt(); i++){
                if(gswd_->degree(i) == 0) continue;
                if(map.find(belong_[i]) == map.end()){
                    map.emplace(belong_[i], cnt++);
                }
                belong_[i] = map.at(belong_[i]);
            }
            printf("res_cnt = %d\n", cnt);

            gswd_->resetStream();
            int source, target;
            long long edges = 0;
            while(gswd_->next(source, target)){
                edges++;
                int c1 = belong_[source];
                int c2 = belong_[target];
                int c;
                if(res_size[c1] >= max_load_ / 2 && res_size[c2] >= max_load_ / 2){
                    int min_c = 0;
                    long min_c_size = res_size[0];
                    for(int i = 0; i < k_; i++){
                        if(res_size[i] < min_c_size){
                            min_c = i;
                            min_c_size = res_size[i];
                        }
                    }
                    c = min_c;
                } else if(res_size[c1] >= max_load_ / 2){
                    c = c2;
                } else if(res_size[c2] >= max_load_ / 2){
                    c = c1;
                } else {
                    if(gswd_->degree(source) < gswd_->degree(target)){
                        c = c1;
                    } else if(gswd_->degree(source) == gswd_->degree(target)){
                        c = source < target ? c1 : c2;
                    } else {
                        c = c2;
                    }
                }
                p.first = source;
                p.second = target;
                sepw_->next(p, c);
                res_size[c]++;
            }
            auto end = clock();
            printf("partitionProcess time = %fs\n", (double)(end - start) / CLOCKS_PER_SEC);
            printf("edges = %lld\n", edges);

            printf("partitionProcess memcost = %dkb\n", getMemoryCost());
        }


        void run(){
            clusteringProcess();
            unionProcess();
            balanceProcess();
            partitionProcess();
        }



    private:
        GraphScannerWithInAndOutDegree *gswd_;
        StreamEdgePartitionWriter *sepw_;
        std::vector<int> belong_;
        std::vector<int> belong_stack_;
        std::vector<int> cluster_target_;
        std::vector<long long> cluster_size_;
        std::vector<long long> in_degree_size_;
        std::vector<long long> out_degree_size_;
        std::vector<long long> match_count_;
        std::vector<std::pair<int, int>> tree_;
        std::vector<int> final_clusters_;
        std::vector<long long> final_clusters_size_;
        int tree_edges_cnt_;
        double max_load_;
        int k_;
        int max_stack_size_;
        std::mt19937_64 random_engine_;
        std::uniform_real_distribution<double> double_generator_;

        struct PairSecondComp{
            bool operator()(const std::pair<int, long long> &a, const std::pair<int, long long> &b){
                return a.second <= b.second;
            }
        };

        struct LongLongComp{
            bool operator()(long long a, long long b){
                return a <= b;
            }
        };

        struct PairFirstComp{
            bool operator()(const std::pair<int, int> &a, const std::pair<int, int> &b){
                return a.first < b.first;
            }
        };

        struct PairSecondCompReverse{
            bool operator()(const std::pair<int, int> &a, const std::pair<int, int> &b){
                return a.second >= b.second;
            }
        };


        int searchInUnion(int point){
            int size = 0;
            while(point != belong_[point]){
                belong_stack_[size++] = point;
                point = belong_[point];
            }
            for(int i = 0; i < size; i++){
                belong_[belong_stack_[i]] = point;
            }
            if(size > max_stack_size_) max_stack_size_ = size;
            return point;
        }


        bool randomChoice(double true_pos){
            // return true;
            return double_generator_(random_engine_) < true_pos;
        }


        void tryMatch(int source, int target){
            if(match_count_[source] == 0){
                cluster_target_[source] = target;
                match_count_[source] = 1;
                return;
            }
            match_count_[source]++;
            double p_match = (double)1 / match_count_[source];
            if(randomChoice(p_match)){
                cluster_target_[source] = target;
            }
        }


        void unionMatch(int source, int target){
            if(match_count_[target] == 0) return;
            if(match_count_[source] == 0){
                cluster_target_[source] = cluster_target_[target];
                match_count_[source] = match_count_[target];
                return;
            }
            double p_choose_source = (double)match_count_[source] / (match_count_[source] + match_count_[target]);
            match_count_[source] += match_count_[target];
            // if(searchInUnion(cluster_target_[target]) == source) return;
            // if(searchInUnion(cluster_target_[source]) == source){
            //     cluster_target_[source] = cluster_target_[target];
            //     return;
            // }
            if(!randomChoice(p_choose_source)){
                cluster_target_[source] = cluster_target_[target];
            }
        }
};


