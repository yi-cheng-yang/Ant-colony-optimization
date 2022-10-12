#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <cfloat>
#include <cmath>
#include <algorithm>
using namespace std;
template <typename T>
using vec1D = vector<T>;
template <typename T>
using vec2D = vector<vec1D<T> >;
template <typename T, typename U> T RandomReal_with_uniform(T BMin, T BMax, U &RG)
{
    uniform_real_distribution<T> R(BMin, BMax);
    return R(RG);
}

template <typename T, typename U> T RandomInt_with_uniform(T BMin, T BMax, U &RG)
{
    uniform_int_distribution<T> R(BMin, BMax);
    return R(RG);
}

template <typename T> double EuclideanDistance(const T* a, const T* b, const int dimension){
    double distance = 0.0;
    for(int i = 0; i < dimension; ++i){
        distance += (b[i] - a[i]) * (b[i] - a[i]);
    }
    distance = sqrt(distance);
    return distance;
}

typedef struct solution{
    vec1D<int> path;
    double length;
}ANT;


#define INITP 1e-6

class ACO{
    public:
        ACO(double v_alpha, double v_beta, double v_Q, double decrease): alpha(v_alpha), beta(v_beta), Q_(v_Q), P_decrease(decrease){
            MAP.clear();
            PathLength.clear();
            P_table.clear();
        }
        void ALG_run(int run, int iter, string FileName, int ant_num){
            random_device rd;
            generator.seed(rd());
            LoadFile(FileName);
            int city_num = MAP.size();

            for(int rc = 0; rc < run; ++rc){
                min_dis = DBL_MAX;
                shortest_path.assign(city_num, 0);
                P_table.assign(city_num, vec1D<double>(city_num, INITP));
                for(int ic = 0; ic < iter; ++ic){
                    ant_colony.clear();
                    for(int ac = 0; ac < ant_num; ++ac){
                        construct_path(city_num);
                    }

                    for(size_t i = 0; i < ant_colony.size(); ++i){
                        if(ant_colony[i].length < min_dis){
                            min_dis = ant_colony[i].length;
                            shortest_path = ant_colony[i].path;
                        }
                    }
                    update_ptable();

                    // double eup_rate = 0.8 - 0.6 * (double(ic) / iter);
                    // if(RandomReal_with_uniform(0.0, 1.0, generator) < eup_rate){
                    //     elite_update();
                    // }
                }
                cout << min_dis << endl;
                // cout << "distance: " << min_dis << endl;
                // for(size_t i = 0; i < shortest_path.size(); ++i){
                //     cout << shortest_path[i] << endl;
                    // cout << MAP[shortest_path[i] - 1][1] << " " << MAP[shortest_path[i] - 1][2] << endl;
                // }
                // cout << MAP[shortest_path[0] - 1][1] << " " << MAP[shortest_path[0] - 1][2] << endl;
            }
        }

    private:
        vec2D<int> MAP;
        vec2D<double> PathLength;
        vec2D<double> P_table;
        mt19937_64 generator;

        double alpha;
        double beta;
        double Q_;
        double P_decrease;

        double min_dis;
        vec1D<int> shortest_path;
        vec1D<ANT> ant_colony;



        void LoadFile(string FileName){
            ifstream input(FileName, ios::in);
            int val;
            int count = 0;
            vec1D<int> point(3, 0); 
            while(input >> val){
                point[count] = val;
                ++count;
                if(count == 3){
                    MAP.push_back(point);
                    point.assign(3, 0);
                    count = 0;
                }
            }
            input.close();

            for(size_t i = 0; i < MAP.size(); ++i){
                vec1D<double> distance;
                for(size_t j = 0; j < MAP.size(); ++j){
                    if(i != j){
                        double d = EuclideanDistance(&MAP[i][1], &MAP[j][1], 2);
                        distance.push_back(d);
                    }
                    else{
                        distance.push_back(0.0);
                    }
                }
                PathLength.push_back(distance);
            }
        }
        void construct_path(int city_num){
            vec1D<int> remain_num;
            for(int i = 1; i <= city_num; ++i){
                remain_num.push_back(i);
            }

            ANT temp_ant;
            int rand_index = RandomInt_with_uniform(0, int(remain_num.size() - 1), generator);
            temp_ant.path.push_back(remain_num[rand_index]);
            remain_num.erase(remain_num.begin() + rand_index);
            while(remain_num.size() > 0){
                int new_city = select_city(*(temp_ant.path.end() - 1), remain_num);
                temp_ant.path.push_back(new_city);
            }

            temp_ant.length = distance_calculate(temp_ant.path);
            ant_colony.push_back(temp_ant);
        }
        int select_city(int last_city, vec1D<int> &remain){
            vec1D<double> prob(remain.size(), 0.0);
            double prob_sum = 0.0;


            for(size_t i = 0; i < remain.size(); ++i){
                double d_ = 1.0 / PathLength[last_city - 1][remain[i] - 1];
                double p_ = P_table[last_city - 1][remain[i] - 1];
                prob[i] = pow(d_, beta) * pow(p_, alpha);
                prob_sum += prob[i];
            }

            double rnd = RandomReal_with_uniform(0.0, prob_sum, generator);
            int return_city;
            for(size_t i = 0; i < prob.size(); ++i){
                if(rnd < prob[i]){
                    return_city = remain[i];
                    remain.erase(remain.begin() + i);
                    break;
                }
                else{
                    rnd -= prob[i];
                }
            }
            return return_city;
        }
        double distance_calculate(const vec1D<int> &path){
            double dis = 0.0;
            for(size_t i = 0; i < path.size() - 1; ++i){
                dis += PathLength[ path[i] - 1 ][ path[i + 1] - 1 ];
            }
            dis += PathLength[ *(path.end() - 1) - 1 ][ *(path.begin()) - 1 ];
            return dis;
        }
        void update_ptable(){
            for(size_t i = 0; i < P_table.size(); ++i){
                for(size_t j = 0; j < P_table[i].size(); ++j){
                    P_table[i][j] = max(P_table[i][j] * P_decrease, INITP);
                }
            }

            for(size_t ac = 0; ac < ant_colony.size(); ++ac){
                double increase = Q_ / ant_colony[ac].length;
                int start, end;
                for(size_t cc = 0; cc < ant_colony[ac].path.size() - 1; ++cc){
                    start = ant_colony[ac].path[cc] - 1;
                    end = ant_colony[ac].path[cc + 1] - 1;
                    P_table[start][end] += increase;
                    P_table[end][start] += increase;
                }
                start = *(ant_colony[ac].path.end() - 1) - 1;
                end = *(ant_colony[ac].path.begin()) - 1;
                P_table[start][end] += increase;
                P_table[end][start] += increase;
            }
        }
        void elite_update(){
            double best_dis = DBL_MAX;
            int best_ant = -1;
            for(size_t ac = 0; ac < ant_colony.size(); ++ac){
                if(ant_colony[ac].length < best_dis){
                    best_dis = ant_colony[ac].length;
                    best_ant = ac;
                }
            }
            double increase = Q_ / ant_colony[best_ant].length;
            int start, end;
            for(size_t cc = 0; cc < ant_colony[best_ant].path.size() - 1; ++cc){
                start = ant_colony[best_ant].path[cc] - 1;
                end = ant_colony[best_ant].path[cc + 1] - 1;
                P_table[start][end] += increase;
                P_table[end][start] += increase;
            }
            start = *(ant_colony[best_ant].path.end() - 1) - 1;
            end = *(ant_colony[best_ant].path.begin()) - 1;
            P_table[start][end] += increase;
            P_table[end][start] += increase;
        }
};

