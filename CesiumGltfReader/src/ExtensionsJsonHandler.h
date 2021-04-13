#pragma once

#include "CesiumGltf/IExtensionJsonReader.h"
#include "CesiumJsonReader/ObjectJsonHandler.h"

namespace CesiumGltf {
struct ReaderContext;
struct ExtensibleObject;

class ExtensionsJsonHandler : public ObjectJsonHandler {
public:
  ExtensionsJsonHandler(const ReaderContext& context) noexcept
      : ObjectJsonHandler(),
        _context(context),
        _pObject(nullptr),
        _currentExtensionHandler() {}

  void reset(
      IJsonReader* pParent,
      ExtensibleObject* pObject,
      const std::string& objectType);

  virtual IJsonReader* readObjectKey(const std::string_view& str) override;

private:
  const ReaderContext& _context;
  ExtensibleObject* _pObject = nullptr;
  std::string _objectType;
  std::unique_ptr<IExtensionJsonReader> _currentExtensionHandler;
};

} // namespace CesiumGltf
