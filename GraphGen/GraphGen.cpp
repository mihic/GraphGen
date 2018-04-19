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
bool kDirected = true;
int kScaleFreeInitialNodes = 2;
int kScaleFreeMinDegree = 1;
double kScaleFreeOffsetExponent = 1.0;
int kMinCost = 1;
int kMaxCost = 100;

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
      ("k,centers", "Number of centers for pmed output, [int (1,n-1) ]", cxxopts::value<int>())
      ("u,undirected", "Generate undirected graphs")
      ("mincost", "Minimum cost of edges", cxxopts::value<int>())
      ("maxcost", "Maximum cost of edges", cxxopts::value<int>())
      ;
    options.add_options("scalefree")
      ("scalefree_initial_nodes", "Number of inital nodes in graph [int]", cxxopts::value<int>())
      ("scalefree_min_degree", "Minimum degree of new nodes [int]", cxxopts::value<int>())
      ("scalefree_offset_exponent", "Offset exponent applied to the probability of new edges [double]", cxxopts::value<double>())
      ;
    auto result = options.parse(argc, argv);

    //Print help
    if (result.count("help")) {
      std::cout << options.help({ "","scalefree" }) << std::endl;
      exit(0);
    }
    if (result.count("debug")) {
      kDebug = true;
      std::cerr << "debuging enabled" << std::endl;
    }

    //Number of nodes
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

    //Output format
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

    //Denstiy
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


    //Generator type
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

    //Seed 
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

    //Number of centers
    if (result.count("centers")) {
      kNumCenters = result["centers"].as<int>();
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


    //Undirected
    if (result.count("undirected")) {
      kDirected = false;
    }
    if (kDebug) {
      if (kDirected) {
        std::cerr << "undirected:false" << std::endl;
      }
      else {
        std::cerr << "undirected:true" << std::endl;
      }
    }

    //Cost of edges
    if (result.count("mincost")) {
      kMinCost = result["mincost"].as<int>();
    }
    if (result.count("maxcost")) {
      kMaxCost = result["maxcost"].as<int>();
    }
    if (kDebug) {
      std::cerr << "mincost: " << kMinCost << std::endl;
      std::cerr << "maxcost: " << kMaxCost << std::endl;
    }


    // ScaleFree paramaters
    if (kGenType == kScaleFree) {
      if (result.count("scalefree_initial_nodes")) {
        kScaleFreeInitialNodes = result["scalefree_initial_nodes"].as<int>();
        if (kScaleFreeInitialNodes >= kNumNodes || kScaleFreeInitialNodes<1) {
          std::cerr << "Number of initial nodes must be in range [1,n), input="<< kScaleFreeInitialNodes << std::endl;
          exit(2);
        }
      }
      if (result.count("scalefree_min_degree")) {
        kScaleFreeMinDegree = result["scalefree_min_degree"].as<int>();
        if (kScaleFreeMinDegree > kScaleFreeInitialNodes || kScaleFreeMinDegree<1) {
          std::cerr << "Minimum degree must be in range [1,scalefree_initial_nodes], input=" << kScaleFreeMinDegree << std::endl;
          exit(2);
        }
      }     
      if (result.count("scalefree_offset_exponent")) {
        kScaleFreeOffsetExponent = result["scalefree_offset_exponent"].as<double>();
      }
      if (kDebug) {
        std::cerr << "scalefree_initial_nodes: " << kScaleFreeInitialNodes << std::endl;
        std::cerr << "scalefree_min_degree: " << kScaleFreeMinDegree << std::endl;
        std::cerr << "scalefree_offset_exponent: " << kScaleFreeOffsetExponent << std::endl;
      }
    }

  }
  catch (const cxxopts::OptionException& e) {
    std::cerr << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }

  switch (kGenType) {
  case kRandom:
    generated_graph = RandomGraph(kNumNodes, kRandomEngine, kDirected, kDensity, kMinCost, kMaxCost);
    break;
  case kGrid:
    generated_graph = Random2DGridGraph(kNumNodes, kRandomEngine, kDirected, kDensity, kMinCost, kMaxCost);
    break;
  case kScaleFree:
    generated_graph = RandomScaleFreeGraph(kNumNodes, kRandomEngine, kScaleFreeInitialNodes, kScaleFreeOffsetExponent, kScaleFreeMinDegree, kMinCost, kMaxCost);
    break;
  }

  switch (kOutFormat) {
  case kPajek:
    generated_graph.output_pajek();
    break;
  case kPmed:
    generated_graph.output_pmed(kNumCenters);
  }
}

