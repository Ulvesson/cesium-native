// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "Library.h"
#include "NamedObject.h"

#include <cstdint>
#include <optional>
#include <string>

namespace CesiumGltf {
/**
 * @brief A buffer points to binary geometry, animation, or skins.
 */
struct CESIUMGLTF_API BufferSpec : public NamedObject {
  static inline constexpr const char* TypeName = "Buffer";

  /**
   * @brief The uri of the buffer.
   *
   * Relative paths are relative to the .gltf file.  Instead of referencing an
   * external file, the uri can also be a data-uri.
   */
  std::optional<std::string> uri;

  /**
   * @brief The length of the buffer in bytes.
   */
  int64_t byteLength = int64_t();

private:
  /**
   * @brief This class is not mean to be instantiated directly. Use {@link Buffer} instead.
   */
  BufferSpec() = default;
  friend struct Buffer;
};
} // namespace CesiumGltf
