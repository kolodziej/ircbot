#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include "yaml-cpp/yaml.h"

namespace ircbot {

/** \class Config
 *
 * \brief Proxy class for yaml-cpp
 *
 * This class plays a role of simple proxy for yaml-cpp object which keeps
 * configuration loaded from yaml file. Config can handle configuration from
 * already existing ptree or from saved file. It also allows to save modified
 * tree back to file.
 */

class Config : public YAML::Node {
 public:
  /** Defualt constructor - creates empty object */
  Config() = default;
  Config(const Config&) = default;
  Config(Config&&) = default;
  Config(const YAML::Node& node) : YAML::Node{node} {}
  Config(YAML::Node&& node) : YAML::Node{std::move(node)} {}

  Config& operator=(const Config&) = default;
  Config& operator=(Config&&) = default;

  /** Constructor loading configuration from file
   *
   * \param fname path to yaml configuration file
   */
  Config(const std::string& fname);

  /** Load file */
  void loadFile();

  /** Load configuration from given file
   *
   * \param fname path to json configuration file
   */
  void loadFile(const std::string& fname);

  /** Save configuration to file given in constructor */
  void saveFile();

  /** Save configuration to given file
   *
   * \param fname path to json configuration file
   */
  void saveFile(const std::string& fname);

 private:
  /** path of configuration file */
  std::string m_fname;
};

}  // namespace ircbot

#endif
