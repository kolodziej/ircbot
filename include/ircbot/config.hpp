#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

class Config {
 public:
  Config() = default;
  Config(const std::string& fname);

  void loadFile();
  void loadFile(const std::string& fname);

  void saveFile();
  void saveFile(const std::string& fname);

  pt::ptree& tree();

 private:
  std::string m_fname;
  pt::ptree m_pt;
};

#endif
