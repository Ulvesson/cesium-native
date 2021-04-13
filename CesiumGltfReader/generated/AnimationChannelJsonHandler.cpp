// This file was generated by generate-gltf-classes.
// DO NOT EDIT THIS FILE!
#include "AnimationChannelJsonHandler.h"
#include "CesiumGltf/AnimationChannel.h"

#include <cassert>
#include <string>

using namespace CesiumGltf;

AnimationChannelJsonHandler::AnimationChannelJsonHandler(
    const ReaderContext& context) noexcept
    : ExtensibleObjectJsonHandler(context), _sampler(), _target(context) {}

void AnimationChannelJsonHandler::reset(
    IJsonReader* pParentReader,
    AnimationChannel* pObject) {
  ExtensibleObjectJsonHandler::reset(pParentReader, pObject);
  this->_pObject = pObject;
}

AnimationChannel* AnimationChannelJsonHandler::getObject() {
  return this->_pObject;
}

void AnimationChannelJsonHandler::reportWarning(
    const std::string& warning,
    std::vector<std::string>&& context) {
  if (this->getCurrentKey()) {
    context.emplace_back(std::string(".") + this->getCurrentKey());
  }
  this->parent()->reportWarning(warning, std::move(context));
}

IJsonReader*
AnimationChannelJsonHandler::readObjectKey(const std::string_view& str) {
  assert(this->_pObject);
  return this->readObjectKeyAnimationChannel(
      AnimationChannel::TypeName,
      str,
      *this->_pObject);
}

IJsonReader* AnimationChannelJsonHandler::readObjectKeyAnimationChannel(
    const std::string& objectType,
    const std::string_view& str,
    AnimationChannel& o) {
  using namespace std::string_literals;

  if ("sampler"s == str)
    return property("sampler", this->_sampler, o.sampler);
  if ("target"s == str)
    return property("target", this->_target, o.target);

  return this->readObjectKeyExtensibleObject(objectType, str, *this->_pObject);
}
