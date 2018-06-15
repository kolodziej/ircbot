#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace ircbot {

/** \class Config
 *
 * \brief Proxy class for boost::property_tree::ptree
 *
 * This class plays a role of simple proxy for boost::property_tree::ptree
 * object which keeps configuration loaded from json file. Config can handle
 * configuration from already existing ptree or from saved file. It also allows
 * to save modified tree back to file.
 */

class Config {
 public:
  /** Defualt constructor - creates empty object */
  Config() = default;

  /** Constructor loading configuration from file
   *
   * \param fname path to json configuration file
   */
  Config(const std::string& fname);

  /** Constructor handling existing ptree
   *
   * ptree object will be copied into Config instance.
   *
   * \param pt boost::property_tree:ptree object
   */
  Config(const pt::ptree& pt);

  /** Load configuration from file given in constructor */
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

  /** Returns a reference to ptree object */
  pt::ptree& tree();

 private:
  /** path of configuration file */
  std::string m_fname;

  /** property tree handling configuration */
  pt::ptree m_pt;
};

}  // namespace ircbot

#endif
