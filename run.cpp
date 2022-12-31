#include <string>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <functional>
#include <vector>

struct Args {
  std::string input;
  std::string output;
  char op = 0; // 0-unknown, 1-serialize, 2-deserialize
};

struct Pair {
  uint32_t v; // vertex
  uint8_t w; // weight
};

bool operator<(const Pair &a, const Pair &b)
{
    return a.v != b.v ? a.v < b.v : a.w < b.w;
}

Args parseArguments(int argc, char **argv) {
    Args Args;
    if (argc != 6 || std::string(argv[2]) != "-i" || std::string(argv[4]) != "-o"){
      std::cout << "Bad command, template: ./run -[s|d] -i filename -o filename" << std::endl;
      exit(1);
    }
    if (std::string(argv[1]) == "-s"){
      Args.op = 1;
    } else if (std::string(argv[1]) == "-d") {
      Args.op = 2;
    } else {
      std::cout << "Bad type operations:" << argv[1] << std::endl;
      exit(1);
    }
    Args.input = std::string(argv[3]);
    Args.output = std::string(argv[5]);
    return Args;
}


typedef std::set<Pair> Pairs;
typedef std::map<uint32_t, Pairs> FlowGraph;

void serialize(const Args &Args) {
    FlowGraph g;
    uint32_t s, e;
    Pair p;
    uint8_t w;
    std::ifstream Input(Args.input);
    std::string edge;
    while (std::getline(Input, edge)) {
        std::sscanf(edge.c_str(), "%d\t%d\t%hhd", &s, &e, &w);
        auto key = std::min(s, e);
        auto ver = std::max(s, e);
        Pair p;
        p.v = ver;
        p.w = w;
        if (!g.count(key)){
          Pairs ps;
          g.insert(std::make_pair(key, ps));
        }
        std::map<uint32_t, Pairs>::iterator it = g.find(key); 
        if (it != g.end()){
          it->second.insert(p); 
        }
    }
    Input.close();
    std::ofstream Out(Args.output, std::ios::binary);
    uint32_t size = g.size();
    //std::cout << size << std::endl;
    Out.write(reinterpret_cast<const char *>(&(size)), sizeof(uint32_t));
    for (auto it = g.begin(); it != g.end(); it++) {
        //std::cout << it->first << ' ' << it->second.size() << std::endl;
        Out.write(reinterpret_cast<const char *>(&(it->first)), sizeof(uint32_t));
        uint32_t size = it->second.size();
        Out.write(reinterpret_cast<const char *>(&size), sizeof(uint32_t));
        for (auto sit = it->second.begin(); sit != it->second.end(); sit++) {
            Out.write(reinterpret_cast<const char *>(&(sit->v)), sizeof(uint32_t));
            Out.write(reinterpret_cast<const char *>(&(sit->w)), sizeof(uint8_t));
        }
    }
    Out.close();
}

void deserialize(const Args &Args) {
    std::ifstream In(Args.input, std::ios::binary);
    std::ofstream Out(Args.output);
    uint32_t size; 
    In.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
    //std::cout << size << std::endl; 
    for (int i = 0; i < size; i++) {
        uint32_t key, ssize;
        In.read(reinterpret_cast<char*>(&key), sizeof(uint32_t));
        In.read(reinterpret_cast<char*>(&ssize), sizeof(uint32_t));
        //std::cout << key << ' ' << ssize << std::endl;
        Pair p;
        for (int j = 0; j < ssize; j++) {
            In.read(reinterpret_cast<char*>(&(p.v)), sizeof(uint32_t));
            In.read(reinterpret_cast<char*>(&(p.w)), sizeof(uint8_t));
            if ((i == size - 1) && (j == ssize - 1) ){
              Out << key << "\t" << p.v << "\t" << int(p.w);
            } else {
              Out << key << "\t" << p.v << "\t" << int(p.w) << std::endl; 
            }
            //std::cout << key << "\t" << p.v << "\t" << ' ' << ssize << ' ' << int(p.w) << std::endl;
        }
    }
    Out.close();
    In.close();
}

int main(int argc, char **argv) {
  auto Args = parseArguments(argc, argv);
  if (Args.op == 1){
    serialize(Args);
  } else if (Args.op == 2){
    deserialize(Args);
  }
  return 0;
}