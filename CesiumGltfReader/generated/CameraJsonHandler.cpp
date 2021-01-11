// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "CameraJsonHandler.h"
#include "CesiumGltf/Camera.h"

#include <cassert>
#include <string>

using namespace CesiumGltf;

void CameraJsonHandler::reset(IJsonHandler* pParent, Camera* pObject) {
  NamedObjectJsonHandler::reset(pParent, pObject);
  this->_pObject = pObject;
}

Camera* CameraJsonHandler::getObject() {
  return this->_pObject;
}

void CameraJsonHandler::reportWarning(const std::string& warning, std::vector<std::string>&& context) {
  if (this->getCurrentKey()) {
    context.emplace_back(std::string(".") + this->getCurrentKey());
  }
  this->parent()->reportWarning(warning, std::move(context));
}

IJsonHandler* CameraJsonHandler::Key(const char* str, size_t /*length*/, bool /*copy*/) {
  assert(this->_pObject);
  return this->CameraKey(str, *this->_pObject);
}

IJsonHandler* CameraJsonHandler::CameraKey(const char* str, Camera& o) {
  using namespace std::string_literals;

  if ("orthographic"s == str) return property("orthographic", this->_orthographic, o.orthographic);
  if ("perspective"s == str) return property("perspective", this->_perspective, o.perspective);
  if ("type"s == str) return property("type", this->_type, o.type);

  return this->NamedObjectKey(str, *this->_pObject);
}

void CameraJsonHandler::TypeJsonHandler::reset(IJsonHandler* pParent, Camera::Type* pEnum) {
  JsonHandler::reset(pParent);
  this->_pEnum = pEnum;
}

IJsonHandler* CameraJsonHandler::TypeJsonHandler::String(const char* str, size_t /*length*/, bool /*copy*/) {
  using namespace std::string_literals;

  assert(this->_pEnum);

  if ("perspective"s == str) *this->_pEnum = Camera::Type::perspective;
  else if ("orthographic"s == str) *this->_pEnum = Camera::Type::orthographic;
  else return nullptr;

  return this->parent();
}
