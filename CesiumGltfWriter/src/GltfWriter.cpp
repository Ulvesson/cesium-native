#include "CesiumGltfWriter/GltfWriter.h"

#include "ModelJsonWriter.h"
#include "registerExtensions.h"

#include <CesiumJsonWriter/JsonWriter.h>
#include <CesiumJsonWriter/PrettyJsonWriter.h>
#include <CesiumUtility/Tracing.h>

namespace CesiumGltfWriter {

namespace {

[[nodiscard]] size_t
getPadding(size_t byteCount, size_t byteAlignment) noexcept {
  assert(byteAlignment > 0);
  size_t remainder = byteCount % byteAlignment;
  size_t padding = remainder == 0 ? 0 : byteAlignment - remainder;
  return padding;
}

[[nodiscard]] std::vector<std::byte> writeGlbBuffer(
    const std::vector<std::byte>& jsonBuffer,
    const std::vector<std::byte>& binaryBuffer,
    size_t binaryChunkByteAlignment) {
  assert(binaryChunkByteAlignment > 0 && binaryChunkByteAlignment % 4 == 0);

  size_t headerSize = 12;
  size_t chunkHeaderSize = 8;

  size_t jsonPaddingSize =
      getPadding(headerSize + chunkHeaderSize + jsonBuffer.size(), 4);
  size_t jsonChunkDataSize = jsonBuffer.size() + jsonPaddingSize;
  size_t glbSize = headerSize + chunkHeaderSize + jsonChunkDataSize;

  size_t binaryPaddingSize = 0;
  size_t binaryChunkDataSize = 0;

  if (binaryBuffer.size() > 0) {
    size_t extraJsonPadding =
        getPadding(glbSize + chunkHeaderSize, binaryChunkByteAlignment);
    if (extraJsonPadding > 0) {
      jsonPaddingSize += extraJsonPadding;
      jsonChunkDataSize += extraJsonPadding;
      glbSize += extraJsonPadding;
    }

    binaryPaddingSize =
        getPadding(glbSize + chunkHeaderSize + binaryBuffer.size(), 4);
    binaryChunkDataSize = binaryBuffer.size() + binaryPaddingSize;
    glbSize += chunkHeaderSize + binaryChunkDataSize;
  }

  std::vector<std::byte> glb(glbSize);
  uint8_t* glb8 = reinterpret_cast<uint8_t*>(glb.data());
  uint32_t* glb32 = reinterpret_cast<uint32_t*>(glb.data());

  // GLB header
  size_t byteOffset = 0;
  glb8[byteOffset++] = 'g';
  glb8[byteOffset++] = 'l';
  glb8[byteOffset++] = 'T';
  glb8[byteOffset++] = 'F';
  glb32[byteOffset / 4] = 2;
  byteOffset += 4;
  glb32[byteOffset / 4] = static_cast<uint32_t>(glbSize);
  byteOffset += 4;

  // JSON chunk header
  glb32[byteOffset / 4] = static_cast<uint32_t>(jsonChunkDataSize);
  byteOffset += 4;
  glb8[byteOffset++] = 'J';
  glb8[byteOffset++] = 'S';
  glb8[byteOffset++] = 'O';
  glb8[byteOffset++] = 'N';

  // JSON chunk
  memcpy(glb8 + byteOffset, jsonBuffer.data(), jsonBuffer.size());
  byteOffset += jsonBuffer.size();

  // JSON chunk padding
  memset(glb8 + byteOffset, ' ', jsonPaddingSize);
  byteOffset += jsonPaddingSize;

  if (binaryBuffer.size() > 0) {
    // Binary chunk header
    glb32[byteOffset / 4] = static_cast<uint32_t>(binaryChunkDataSize);
    byteOffset += 4;
    glb8[byteOffset++] = 'B';
    glb8[byteOffset++] = 'I';
    glb8[byteOffset++] = 'N';
    glb8[byteOffset++] = 0;

    // Binary chunk
    memcpy(glb8 + byteOffset, binaryBuffer.data(), binaryBuffer.size());
    byteOffset += binaryBuffer.size();

    // Binary chunk padding
    memset(glb8 + byteOffset, 0, binaryPaddingSize);
  }

  return glb;
}
} // namespace

GltfWriter::GltfWriter() { registerExtensions(this->_context); }

CesiumJsonWriter::ExtensionWriterContext& GltfWriter::getExtensions() {
  return this->_context;
}

const CesiumJsonWriter::ExtensionWriterContext&
GltfWriter::getExtensions() const {
  return this->_context;
}

GltfWriterResult GltfWriter::writeGltf(
    const CesiumGltf::Model& model,
    const GltfWriterOptions& options) const {
  CESIUM_TRACE("GltfWriter::writeGltf");

  const CesiumJsonWriter::ExtensionWriterContext& context =
      this->getExtensions();

  GltfWriterResult result;
  std::unique_ptr<CesiumJsonWriter::JsonWriter> writer;

  if (options.prettyPrint) {
    writer = std::make_unique<CesiumJsonWriter::PrettyJsonWriter>();
  } else {
    writer = std::make_unique<CesiumJsonWriter::JsonWriter>();
  }

  ModelJsonWriter::write(model, *writer, context);
  result.gltfBytes = writer->toBytes();

  return result;
}

GltfWriterResult GltfWriter::writeGlb(
    const CesiumGltf::Model& model,
    const std::vector<std::byte>& bufferData,
    const GltfWriterOptions& options) const {
  CESIUM_TRACE("GltfWriter::writeGlb");

  const CesiumJsonWriter::ExtensionWriterContext& context =
      this->getExtensions();

  GltfWriterResult result;

  std::unique_ptr<CesiumJsonWriter::JsonWriter> writer;

  if (options.prettyPrint) {
    writer = std::make_unique<CesiumJsonWriter::PrettyJsonWriter>();
  } else {
    writer = std::make_unique<CesiumJsonWriter::JsonWriter>();
  }

  ModelJsonWriter::write(model, *writer, context);
  result.gltfBytes = writeGlbBuffer(
      writer->toBytes(),
      bufferData,
      options.binaryChunkByteAlignment);

  return result;
}

} // namespace CesiumGltfWriter
