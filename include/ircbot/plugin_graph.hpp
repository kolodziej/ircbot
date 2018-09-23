#ifndef _IRCBOT_PLUGIN_GRAPH_HPP
#define _IRCBOT_PLUGIN_GRAPH_HPP

#include <map>
#include <memory>
#include <vector>

#include "ircbot/core.hpp"

namespace ircbot {

/* \class PluginGraph
 *
 * Container for all plugins in ircbot.
 */

class PluginGraph : private std::enable_shared_from_this<PluginGraph> {
 public:
  PluginGraph(std::shared_ptr<Core> core, Config config);

  /** Loads plugin from file and adds it to PluginGraph
   *
   * Uses id to load plugin. Where plugin is not accessible, throws an exception
   *
   * \param id unique id in format: type://resource; eg. py://hello.py,
   * tcp://reported_name, so://clipboard/clipboard.so
   *
   * \return shared pointer to loaded plugin
   */
  std::shared_ptr<Plugin> loadPlugin(const std::string& id);

  /** Adds plugin to Graph
   *
   * This function runs dependency check and loads dependencies if they are
   * needed. In case it is not possible to load some depenedencies function will
   * raise an exception
   *
   * \param id unique id in format: type://resource; eg. py://hello.py,
   * tcp://reported_name, so://clipboard/clipboard.so
   * \param plugin shared pointer to plugin
   */
  void addPlugin(const std::string& id, std::shared_ptr<Plugin> plugin);

  /** Remove plugin from Graph
   *
   * This function removes plugin from graph.
   */
  void removePlugin(const std::string& id);

 private:
  /** pointer to Core */
  std::shared_ptr<Core> m_core;

  /** graph roots - inputs to plugins' graph */
  std::vector<std::shared_ptr<Plugin>> m_roots;

  /** all plugins registered in graph */
  std::vector<std::shared_ptr<Plugin>> m_plugins;

 private:
  /** splits plugin's ID into type and path part */
  std::pair<std::string, std::string> splitId(const std::string& id);

  /** load python plugin
   *
   * \param path path of file with plugin
   */
  std::shared_ptr<Plugin> loadPyPlugin(const std::string& path);

  /** load so plugin
   *
   * \param path path of file with plugin
   */
  std::shared_ptr<Plugin> loadSoPlugin(const std::string& path);
};

}  // namespace ircbot

#endif
