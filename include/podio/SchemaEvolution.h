#ifndef PODIO_SCHEMAEVOLUTION_H
#define PODIO_SCHEMAEVOLUTION_H

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace podio {

enum class Backend { ROOT, SIO };

/// The type used for schema versions throughout
using SchemaVersionT = uint32_t;

struct CollectionReadBuffers;

/**
 * The SchemaEvolution holds evolution functions that allow to transform
 * CollectionReadBuffers of known datatypes from a previous schema version to
 * the current schema version. From the evolved buffers it is then possible to
 * create collections.
 *
 * It is implemented as a singleton that is populated at the time shared
 * datamodel libraries (or their schema evolution libraries) are loaded. It is
 * assumed that this happens early on in the startup of any application, such
 * that the registration still happens on a single thread. After this
 * initialization evolutions can be done from multiple threads.
 */
class SchemaEvolution {
  /// The interface of any evolution function takes buffers and a version and
  /// returns buffers.
  using EvolutionFuncT = std::function<podio::CollectionReadBuffers(podio::CollectionReadBuffers, SchemaVersionT)>;
  /// Each datatype gets its own version "map" where the index defines the
  /// version from which the schema evolution has to start to end up in the
  /// current version
  using EvolFuncVersionMapT = std::vector<EvolutionFuncT>;

  /**
   * Helper struct combining the current schema version of each type and an
   * index into the schema evolution "map" below
   */
  struct MapIndex {
    SchemaVersionT currentVersion; ///< The current schema version for each type
    size_t index;                  ///< The index in the evolution function map
    /// Tombstone value indicating that no evolution function is available (yet)
    constexpr static size_t NoEvolutionAvailable = -1u;
  };

  /// The map that holds the current version for each type that is known to
  /// the schema evolution
  using VersionMapT = std::unordered_map<std::string, MapIndex>;
  /// The "map" that holds all evolution functions
  using EvolutionMapT = std::vector<EvolFuncVersionMapT>;

public:
  /**
   * Enum to make it possible to prioritize evolution functions during
   * registration, making AutoGenerated lower priority than UserDefined
   */
  enum class Priority { AutoGenerated = 0, UserDefined = 1 };

  /// The SchemaEvolution is a singleton so we disable all copy and move
  /// constructors explicitly
  SchemaEvolution(const SchemaEvolution&) = delete;
  SchemaEvolution& operator=(const SchemaEvolution&) = delete;
  SchemaEvolution(SchemaEvolution&&) = delete;
  SchemaEvolution& operator=(SchemaEvolution&&) = delete;
  ~SchemaEvolution() = default;

  /// Mutable instance only used for the initial registration of functions
  /// during library loading
  static SchemaEvolution& mutInstance();
  /// Get the instance for evolving buffers
  static SchemaEvolution const& instance();

  /**
   * Evolve the passed in buffers to the current version of the datatype that
   * can be constructed from them.
   *
   * Internally this will first check if the schema version of the buffers is
   * already the current one and in that case immediately return the passed in
   * buffers again as they do not need schema evolution. If that is not the case
   * it will look up the correct evolution function for the passed in version
   * and call that on the passed in buffers.
   *
   * @param oldBuffers The buffers to be evolved
   * @param fromVersion The schema version of the buffers
   * @param collType The fully qualified collection type
   *
   * @returns CollectionReadBuffers that have been evolved to the current
   * version. NOTE that these could also be the input buffers.
   */
  podio::CollectionReadBuffers evolveBuffers(const podio::CollectionReadBuffers& oldBuffers, SchemaVersionT fromVersion,
                                             const std::string& collType) const;

  /**
   * Register an evoution function for a given collection type and given
   * versions from where to where the evolution applies.
   *
   * Several assumptions are in place here:
   *
   * - The current version has to be the same for all invocations for a given
   *   datatype.
   * - An evolution function has to be registered for all possible versions from
   *   1 to N - 1, where N is the current version
   * - An evolution function can only be registerd once for a given datatype and
   *   fromVersion
   * - For auto generated code the passed in priority has to be AutoGenerated
   *   otherwise it might override user defined functions
   * - Even if a datatype does not require schema evolution it has to register
   *   an evolution function (e.g. the noOpSchemaEvolution below) in order to be
   *   known to the internal map.
   *
   * @param collType The fully qualified collection data type
   * @param fromVersion The version from which this evolution function should
   * apply
   * @param currentVersion The current schema version for the data type
   * @param evolutionFunc The evolution function that evolves passed in buffers
   * from fromVersion to currrentVersion
   * @param priority The priority of this evolution function. Defaults to
   * UserDefined which overrides auto generated functionality.
   */
  void registerEvolutionFunc(const std::string& collType, SchemaVersionT fromVersion, SchemaVersionT currentVersion,
                             const EvolutionFuncT& evolutionFunc, Priority priority = Priority::UserDefined);

  /**
   * A no-op schema evolution function that returns the buffers unchanged.
   *
   * This can be used for registering an evolution function for datatypes that
   * do not require schema evolution, but need to register themselves with
   * SchemaEvolution
   */
  static podio::CollectionReadBuffers noOpSchemaEvolution(podio::CollectionReadBuffers&& buffers, SchemaVersionT);

private:
  SchemaEvolution() = default;

  /// The map containing types and MapIndex structs
  VersionMapT m_versionMapIndices{};
  /// The "map" holding the evolution functions
  EvolutionMapT m_evolutionFuncs{};
};

} // namespace podio

#endif // PODIO_SCHEMAEVOLUTION_H
