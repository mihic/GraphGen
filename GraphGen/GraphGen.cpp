#include "stdafx.h" //precompiled header

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
OutputFormat kOutFormat;
GeneratorType kGenType;
bool kDebug = false;

int main(int argc, const char* argv[]) {
  try {
    cxxopts::Options options("GraphGen", "Random Graph Generator");
    options.add_options()
      ("help", "Print help")
      ("d,debug", "Enable debugging")
      ("n,nodes", "Number of nodes, [int]", cxxopts::value<int>())
      ("f,format", "Output format type, [pajek,pmed]", cxxopts::value<std::string>())
      ("p,density", "Density of edges, [double(0-1)]", cxxopts::value<double>())
      ("t,type", "Graph type [random,grid,scalefree]", cxxopts::value<std::string>())
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
  }
  catch (const cxxopts::OptionException& e) {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

