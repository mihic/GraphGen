#include "stdafx.h" //precompiled header
#include "Graph.hpp"

enum OutputFormat {
  kPajek,
  kPmed,
};

enum GeneratorType {
  kRandom,
  kGrid,
  kScaleFree
};

static std::map<std::string, GeneratorType> kGeneratorTypeMap{
  {"random",    kRandom},
  {"grid",      kGrid},
  {"scalefree", kScaleFree}
};

static std::map<std::string, OutputFormat> kOutputFormatTypeMap{
  {"pajek", kPajek },
  {"pmed",  kPmed }
};



int kNumNodes;
double kDensity;
int kNumCenters;
OutputFormat kOutFormat;
GeneratorType kGenType;
std::default_random_engine kRandomEngine;
Graph generated_graph;
bool kDebug = false;

int main(int argc, const char* argv[]) {
  try {
    cxxopts::Options options("GraphGen", "Random Graph Generator");
    options.add_options()
      ("help", "Print help")
      ("d,debug", "Enable debugging")
      ("n,nodes", "Number of nodes, [int]", cxxopts::value<int>())
      ("f,format", "Output format type, [pajek,pmed]", cxxopts::value<std::string>())
      ("p,density", "Density of edges, [double (0,1] ]", cxxopts::value<double>())
      ("t,type", "Graph type [random,grid,scalefree]", cxxopts::value<std::string>())
      ("s,seed", "Random generator seed, [int]", cxxopts::value<int>())
      ("k,centers", "Number of centers for pmed output, [int (1,n-1) ]")
      ;

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
      std::cout << options.help({""}) << std::endl;
      exit(0);
    }
    if (result.count("debug")) {
      kDebug = true;
      std::cerr << "debuging enabled" << std::endl;
    }

    if (result.count("nodes")) {
      kNumNodes = result["nodes"].as<int>();
      if (kNumNodes <= 0) {
        std::cerr << "Invalid number of nodes: " << kNumNodes << std::endl;
        exit(2);
      }
      if (kDebug) {
        std::cerr << "Nodes: " << kNumNodes << std::endl;
      }
    }
    else {
      std::cerr << "Number of nodes not defined!" << std::endl;
      exit(2);
    }

    if (result.count("format")) {
      std::string format = result["format"].as<std::string>();
      if (kOutputFormatTypeMap.count(format)) {
        kOutFormat = kOutputFormatTypeMap[format];
        if (kDebug) {
          std::cerr << "Fromat type: " << format << std::endl;
        }
      }
      else {
        std::cerr << "Unknown format type:" << format << std::endl;
        exit(2);
      }
    }
    else {
      std::cerr << "Output format type not defined!" << std::endl;
      exit(2);
    }

    if (result.count("density")) {
      kDensity = result["density"].as<double>();
      if (kDensity <= 0 || kDensity > 1) {
        std::cerr << "Density must be a value in range (0,1], input=" << kDensity << std::endl;
        exit(2);
      }
      if (kDebug) {
        std::cerr << "Density: " << kDensity << std::endl;
      }
    }
    else {
      std::cerr << "Density not defined!" << std::endl;
      exit(2);
    }

    if (result.count("type")) {
      std::string type = result["type"].as<std::string>();
      if (kGeneratorTypeMap.count(type)) {
        kGenType = kGeneratorTypeMap[type];
        if (kDebug) {
          std::cerr << "Generator type: " << type << std::endl;
        }
      }
      else {
        std::cerr << "Unknown generator type:" << type << std::endl;
        exit(2);
      }
    }
    else {
      std::cerr << "Generator type not defined!" << std::endl;
      exit(2);
    }

    if (result.count("seed")) {
      int seed = result["seed"].as<int>();
      kRandomEngine = std::default_random_engine(seed);
      if (kDebug) {
        std::cerr << "Seed: " << seed << std::endl;
      }
    }
    else {
      std::random_device r;
      auto random_seed = r();
      kRandomEngine = std::default_random_engine(random_seed);
      if (kDebug) {
        std::cerr << "Seed: " << random_seed << std::endl;
      }
    }
    if (result.count("centers")) {
      int kNumCenters = result["centers"].as<int>();
      if (kNumCenters < 1 || kNumCenters >= kNumNodes) {
        std::cerr << "centers must be a value in range (1,nodes-1) input=" << kNumCenters << std::endl;
        exit(2);
      }
    }
    else {
      kNumCenters = kNumNodes / 3;
    }
    if (kDebug) {
      std::cerr << "centers: " << kNumCenters << std::endl;
    }
  }
  catch (const cxxopts::OptionException& e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }

  switch (kGenType) {
  case kRandom:
    generated_graph = RandomGraph(kNumNodes, kRandomEngine, kDensity,1,100);
    break;
  case kGrid:
    generated_graph = Random2DGridGraph(kNumNodes, kRandomEngine, kDensity, 1, 100);
  }

  switch (kOutFormat) {
  case kPajek:
    generated_graph.output_pajek();
    break;
  case kPmed:
    generated_graph.output_pmed(kNumCenters);
  }
}

