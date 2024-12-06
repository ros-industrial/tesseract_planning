/**
 * @file profile_dictionary.h
 * @brief This is a profile dictionary for storing all profiles
 *
 * @author Levi Armstrong
 * @date December 2, 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_MOTION_PLANNERS_PROFILE_DICTIONARY_H
#define TESSERACT_MOTION_PLANNERS_PROFILE_DICTIONARY_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <unordered_map>
#include <memory>
#include <shared_mutex>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/profile.h>
#include <tesseract_common/any_poly.h>

namespace tesseract_planning
{
/**
 * This used to store specific profile mapping with the request
 *
 * For example say you have a profile named Raster in your command language. Say you have multiple Raster profiles
 * for descartes {Raster, Raster1, Raster2}. This allows you to remap the meaning of Raster in the command language to
 * say Raster2 for the specific planner Descartes by Map<Descartes, Map<Raster, Raster1>>.
 */
using ProfileRemapping = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;

/**
 * @brief This class is used to store profiles used by various tasks
 * @details This is a thread safe class
 */
class ProfileDictionary
{
public:
  using Ptr = std::shared_ptr<ProfileDictionary>;
  using ConstPtr = std::shared_ptr<const ProfileDictionary>;

  /**
   * @brief Add a profile
   * @details If the profile entry does not exist it will create one
   * @param ns The profile namespace
   * @param profile_name The profile name
   * @param profile The profile to add
   */
  void addProfile(const std::string& ns, const std::string& profile_name, const Profile::ConstPtr& profile);

  /**
   * @brief Check if a profile exists
   * @details If profile entry does not exist it also returns false
   * @param key The profile key
   * @param ns The profile namespace
   * @param profile_name The profile name
   * @return True if profile exists, otherwise false
   */
  bool hasProfile(std::size_t key, const std::string& ns, const std::string& profile_name) const;

  /**
   * @brief Get a profile by name
   * @details Check if the profile exist before calling this function, if missing an exception is thrown
   * @param key The profile key
   * @param ns The profile namespace
   * @param profile_name The profile name
   * @return The profile
   */
  Profile::ConstPtr getProfile(std::size_t key, const std::string& ns, const std::string& profile_name) const;

  /**
   * @brief Remove a profile
   * @param key The profile key
   * @param ns The profile namespace
   * @param profile_name The profile to be removed
   */
  void removeProfile(std::size_t key, const std::string& ns, const std::string& profile_name);

  /**
   * @brief Check if a profile entry exists
   * @param key The profile key
   * @param ns The profile namespace
   * @return True if exists, otherwise false
   */
  bool hasProfileEntry(std::size_t key, const std::string& ns) const;

  /**
   * @brief Remove a profile entry
   * @param key The profile key
   * @param ns The profile namespace
   */
  void removeProfileEntry(std::size_t key, const std::string& ns);

  /**
   * @brief Get a profile entry
   * @param key The profile key
   * @param ns The profile namespace
   * @return The profile map associated with the profile entry
   */
  std::unordered_map<std::string, Profile::ConstPtr> getProfileEntry(std::size_t key, const std::string& ns) const;

  /** @brief Clear the dictionary */
  void clear();

protected:
  mutable std::shared_mutex mutex_;
  std::unordered_map<std::string, std::unordered_map<std::size_t, std::unordered_map<std::string, Profile::ConstPtr>>>
      profiles_;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version);  // NOLINT
};

}  // namespace tesseract_planning

BOOST_CLASS_EXPORT_KEY(tesseract_planning::ProfileDictionary)
TESSERACT_ANY_EXPORT_KEY(std::shared_ptr<tesseract_planning::ProfileDictionary>,
                         TesseractPlanningProfileDictionarySharedPtr)

#endif  // TESSERACT_MOTION_PLANNERS_PROFILE_DICTIONARY_H
