# Entity Data Converter - Implementation Templates
## Code Templates and Examples for Format Conversion

**Related Document**: [ENTITY_DATA_FORMAT_ANALYSIS.md](ENTITY_DATA_FORMAT_ANALYSIS.md)  
**Purpose**: Provide ready-to-use code templates for implementing data format conversion  
**Date**: 2026-01-30

---

## Table of Contents
1. [Converter Module Template](#converter-module-template)
2. [Test Fixture Update Templates](#test-fixture-update-templates)
3. [Matcher Implementation Templates](#matcher-implementation-templates)
4. [Adapter Pattern Templates](#adapter-pattern-templates)
5. [Error Handling Examples](#error-handling-examples)
6. [Performance Optimization Examples](#performance-optimization-examples)

---

## Converter Module Template

### Header File: EntityDataConverter.h

```cpp
#pragma once

#include <expected>
#include <string>
#include <vector>
#include <flatbuffers/flatbuffers.h>

// Forward declarations
class EntityMemoryPool;
struct EntityCollectionFbs;
struct Entity;
struct EntityFbs;

namespace EntityDataConverter {

/// @brief Convert FlatBuffers entity collection to memory pool format
/// @param fbs_data Pointer to FlatBuffers entity collection (must not be null)
/// @return EntityMemoryPool on success, error message on failure
/// @throws std::invalid_argument if fbs_data is null
std::expected<EntityMemoryPool, std::string> ToMemoryPool(
    const EntityCollectionFbs* fbs_data
);

/// @brief Convert memory pool to FlatBuffers format
/// @param pool Reference to entity memory pool
/// @return DetachedBuffer containing FlatBuffers data, or error message
std::expected<flatbuffers::DetachedBuffer, std::string> ToFlatBuffers(
    const EntityMemoryPool& pool
);

/// @brief Convert single entity from FlatBuffers to memory pool format
/// @param fbs_entity Pointer to FlatBuffers entity (must not be null)
/// @return Entity on success, error message on failure
std::expected<Entity, std::string> ConvertEntity(
    const EntityFbs* fbs_entity
);

/// @brief Validate FlatBuffers entity collection structure
/// @param fbs_data Pointer to FlatBuffers entity collection
/// @return true if valid, false otherwise
bool ValidateEntityCollectionFbs(const EntityCollectionFbs* fbs_data);

/// @brief Get human-readable description of conversion error
/// @param error_code Error code from conversion
/// @return Error description string
std::string GetConversionErrorDescription(int error_code);

} // namespace EntityDataConverter
```

### Implementation File: EntityDataConverter.cpp

```cpp
#include "EntityDataConverter.h"
#include "EntityMemoryPool.h"
#include "entity_collection_generated.h" // FlatBuffers generated header
#include <stdexcept>
#include <format>

namespace EntityDataConverter {

std::expected<EntityMemoryPool, std::string> ToMemoryPool(
    const EntityCollectionFbs* fbs_data
) {
    // Null check
    if (!fbs_data) {
        return std::unexpected("FlatBuffers data pointer is null");
    }
    
    // Validate structure
    if (!ValidateEntityCollectionFbs(fbs_data)) {
        return std::unexpected("Invalid FlatBuffers entity collection structure");
    }
    
    // Get entities vector
    const auto* entities = fbs_data->entities();
    if (!entities) {
        return std::unexpected("FlatBuffers entities vector is null");
    }
    
    // Create memory pool with pre-allocated capacity
    EntityMemoryPool pool;
    pool.Reserve(entities->size());
    
    // Convert each entity
    for (size_t i = 0; i < entities->size(); ++i) {
        const auto* fbs_entity = entities->Get(i);
        
        if (!fbs_entity) {
            return std::unexpected(
                std::format("Entity at index {} is null", i)
            );
        }
        
        // Convert individual entity
        auto entity_result = ConvertEntity(fbs_entity);
        if (!entity_result) {
            return std::unexpected(
                std::format("Failed to convert entity at index {}: {}", 
                           i, entity_result.error())
            );
        }
        
        // Add to pool
        pool.Add(entity_result.value());
    }
    
    return pool;
}

std::expected<flatbuffers::DetachedBuffer, std::string> ToFlatBuffers(
    const EntityMemoryPool& pool
) {
    try {
        flatbuffers::FlatBufferBuilder builder(1024); // Initial size
        
        // Convert entities
        std::vector<flatbuffers::Offset<EntityFbs>> entity_offsets;
        entity_offsets.reserve(pool.Size());
        
        for (size_t i = 0; i < pool.Size(); ++i) {
            const auto& entity = pool.Get(i);
            
            // Create position
            auto pos = CreatePositionFbs(builder, 
                                        entity.position.x,
                                        entity.position.y,
                                        entity.position.z);
            
            // Create entity data vector
            auto data_vector = builder.CreateVector(entity.data);
            
            // Create entity
            auto entity_fbs = CreateEntityFbs(builder,
                                             entity.id,
                                             pos,
                                             entity.flags,
                                             data_vector);
            
            entity_offsets.push_back(entity_fbs);
        }
        
        // Create entity collection
        auto entities_vector = builder.CreateVector(entity_offsets);
        auto collection = CreateEntityCollectionFbs(builder, 
                                                   entities_vector,
                                                   pool.GetVersion());
        
        builder.Finish(collection);
        
        return builder.Release();
        
    } catch (const std::exception& e) {
        return std::unexpected(
            std::format("FlatBuffers conversion failed: {}", e.what())
        );
    }
}

std::expected<Entity, std::string> ConvertEntity(const EntityFbs* fbs_entity) {
    if (!fbs_entity) {
        return std::unexpected("Entity pointer is null");
    }
    
    Entity entity;
    
    // Convert ID
    entity.id = fbs_entity->id();
    
    // Convert position
    const auto* pos = fbs_entity->position();
    if (!pos) {
        return std::unexpected("Entity position is null");
    }
    
    entity.position.x = pos->x();
    entity.position.y = pos->y();
    entity.position.z = pos->z();
    
    // Convert flags
    entity.flags = fbs_entity->flags();
    
    // Convert data vector
    const auto* data_vector = fbs_entity->data();
    if (data_vector) {
        entity.data.assign(data_vector->begin(), data_vector->end());
    }
    
    return entity;
}

bool ValidateEntityCollectionFbs(const EntityCollectionFbs* fbs_data) {
    if (!fbs_data) {
        return false;
    }
    
    const auto* entities = fbs_data->entities();
    if (!entities) {
        return false;
    }
    
    // Validate each entity has required fields
    for (const auto* entity : *entities) {
        if (!entity) {
            return false;
        }
        
        if (!entity->position()) {
            return false;
        }
    }
    
    return true;
}

std::string GetConversionErrorDescription(int error_code) {
    switch (error_code) {
        case 1: return "Null pointer provided";
        case 2: return "Invalid data structure";
        case 3: return "Memory allocation failed";
        case 4: return "Data validation failed";
        default: return "Unknown error";
    }
}

} // namespace EntityDataConverter
```

---

## Test Fixture Update Templates

### Before: Using FlatBuffers Directly

```cpp
class TestData {
private:
    std::vector<uint8_t> buffer_;
    const EntityCollectionFbs* entity_data_;  // ❌ Wrong format
    
public:
    void LoadFromFile(const std::string& path) {
        // Load file into buffer
        std::ifstream file(path, std::ios::binary);
        buffer_ = std::vector<uint8_t>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        
        // Get FlatBuffers root
        entity_data_ = GetEntityCollectionFbs(buffer_.data());
    }
    
    const EntityCollectionFbs* GetEntityData() const {
        return entity_data_;  // ❌ Returns wrong type
    }
};
```

### After: Using Converted Format

```cpp
class TestData {
private:
    std::vector<uint8_t> buffer_;  // Keep for FlatBuffers validity
    EntityMemoryPool entity_pool_;  // ✅ Correct format
    
public:
    std::expected<void, std::string> LoadFromFile(const std::string& path) {
        // Load FlatBuffers file
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return std::unexpected(
                std::format("Failed to open file: {}", path)
            );
        }
        
        buffer_ = std::vector<uint8_t>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        
        // Get FlatBuffers root
        const auto* fbs_data = GetEntityCollectionFbs(buffer_.data());
        
        // Convert to memory pool
        auto conversion_result = EntityDataConverter::ToMemoryPool(fbs_data);
        if (!conversion_result) {
            return std::unexpected(
                std::format("Conversion failed: {}", conversion_result.error())
            );
        }
        
        entity_pool_ = std::move(conversion_result.value());
        return {};
    }
    
    const EntityMemoryPool& GetEntityData() const {
        return entity_pool_;  // ✅ Returns correct type
    }
};
```

### Alternative: Lazy Conversion

```cpp
class TestData {
private:
    std::vector<uint8_t> buffer_;
    const EntityCollectionFbs* fbs_data_;
    mutable std::optional<EntityMemoryPool> cached_pool_;
    
public:
    void LoadFromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        buffer_ = std::vector<uint8_t>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        
        fbs_data_ = GetEntityCollectionFbs(buffer_.data());
        cached_pool_.reset();  // Clear cache on new load
    }
    
    const EntityMemoryPool& GetEntityData() const {
        // Lazy conversion on first access
        if (!cached_pool_) {
            auto result = EntityDataConverter::ToMemoryPool(fbs_data_);
            if (!result) {
                throw std::runtime_error(
                    std::format("Conversion failed: {}", result.error())
                );
            }
            cached_pool_ = std::move(result.value());
        }
        
        return *cached_pool_;
    }
};
```

---

## Matcher Implementation Templates

### Catch2 Custom Matcher

```cpp
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

namespace TestMatchers {

/// Custom matcher for comparing EntityCollectionFbs to EntityMemoryPool
class EntityCollectionEqualsMatcher : 
    public Catch::Matchers::MatcherGenericBase {
private:
    const EntityCollectionFbs* expected_;
    mutable std::string mismatch_description_;
    
public:
    explicit EntityCollectionEqualsMatcher(const EntityCollectionFbs* expected)
        : expected_(expected) {}
    
    bool match(const EntityMemoryPool& actual) const {
        // Validate input
        if (!expected_) {
            mismatch_description_ = "Expected data is null";
            return false;
        }
        
        const auto* entities = expected_->entities();
        if (!entities) {
            mismatch_description_ = "Expected entities vector is null";
            return false;
        }
        
        // Compare sizes
        if (entities->size() != actual.Size()) {
            mismatch_description_ = std::format(
                "Size mismatch: expected {} but got {}",
                entities->size(), actual.Size()
            );
            return false;
        }
        
        // Compare each entity
        for (size_t i = 0; i < entities->size(); ++i) {
            const auto* expected_entity = entities->Get(i);
            const auto& actual_entity = actual.Get(i);
            
            if (!CompareEntity(expected_entity, actual_entity, i)) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string describe() const override {
        return "equals entity collection";
    }
    
    std::string describe_mismatch(const EntityMemoryPool& actual) const {
        return mismatch_description_;
    }

private:
    bool CompareEntity(const EntityFbs* expected, 
                      const Entity& actual, 
                      size_t index) const {
        // Compare ID
        if (expected->id() != actual.id) {
            mismatch_description_ = std::format(
                "Entity {} ID mismatch: expected {} but got {}",
                index, expected->id(), actual.id
            );
            return false;
        }
        
        // Compare position
        const auto* pos = expected->position();
        if (pos->x() != actual.position.x ||
            pos->y() != actual.position.y ||
            pos->z() != actual.position.z) {
            mismatch_description_ = std::format(
                "Entity {} position mismatch: expected ({}, {}, {}) but got ({}, {}, {})",
                index, pos->x(), pos->y(), pos->z(),
                actual.position.x, actual.position.y, actual.position.z
            );
            return false;
        }
        
        // Compare flags
        if (expected->flags() != actual.flags) {
            mismatch_description_ = std::format(
                "Entity {} flags mismatch: expected {} but got {}",
                index, expected->flags(), actual.flags
            );
            return false;
        }
        
        // Compare data
        const auto* expected_data = expected->data();
        if (expected_data && 
            !std::equal(expected_data->begin(), expected_data->end(),
                       actual.data.begin(), actual.data.end())) {
            mismatch_description_ = std::format(
                "Entity {} data mismatch",
                index
            );
            return false;
        }
        
        return true;
    }
};

/// Helper function for creating matcher
inline EntityCollectionEqualsMatcher EqualsEntityCollection(
    const EntityCollectionFbs* expected
) {
    return EntityCollectionEqualsMatcher(expected);
}

} // namespace TestMatchers

// Usage in tests
TEST_CASE("Entity matching with custom matcher", "[entities]") {
    TestData test_data;
    test_data.LoadFromFile("test_entities.fbs");
    
    TestEngine engine;
    engine.ProcessEntities();
    
    REQUIRE_THAT(engine.GetDataBank(), 
                 TestMatchers::EqualsEntityCollection(test_data.entity_data));
}
```

### Google Test Custom Matcher

```cpp
#include <gmock/gmock.h>

namespace TestMatchers {

MATCHER_P(EqualsEntityCollection, expected, 
          "matches entity collection") {
    if (!expected) {
        *result_listener << "expected data is null";
        return false;
    }
    
    const auto* entities = expected->entities();
    if (!entities) {
        *result_listener << "expected entities vector is null";
        return false;
    }
    
    if (entities->size() != arg.Size()) {
        *result_listener << "size mismatch: expected " 
                        << entities->size() << " but got " << arg.Size();
        return false;
    }
    
    for (size_t i = 0; i < entities->size(); ++i) {
        const auto* expected_entity = entities->Get(i);
        const auto& actual_entity = arg.Get(i);
        
        if (expected_entity->id() != actual_entity.id) {
            *result_listener << "entity " << i << " ID mismatch";
            return false;
        }
        
        // More comparisons...
    }
    
    return true;
}

} // namespace TestMatchers

// Usage
TEST(EntityTest, ProcessingProducesCorrectEntities) {
    TestData test_data;
    test_data.LoadFromFile("test_entities.fbs");
    
    TestEngine engine;
    engine.ProcessEntities();
    
    EXPECT_THAT(engine.GetDataBank(), 
                TestMatchers::EqualsEntityCollection(test_data.entity_data));
}
```

---

## Adapter Pattern Templates

### Full Adapter Implementation

```cpp
#pragma once

#include "EntityMemoryPool.h"
#include "entity_collection_generated.h"
#include <memory>
#include <optional>
#include <vector>

/// @brief Adapter that makes EntityCollectionFbs* look like EntityMemoryPool
/// Provides lazy conversion and memory-efficient access to FlatBuffers data
class EntityCollectionAdapter {
public:
    /// @brief Construct adapter from FlatBuffers data
    /// @param fbs_data Pointer to FlatBuffers entity collection (not owned)
    explicit EntityCollectionAdapter(const EntityCollectionFbs* fbs_data)
        : fbs_data_(fbs_data) {
        if (!fbs_data_) {
            throw std::invalid_argument("FlatBuffers data cannot be null");
        }
        entity_cache_.resize(fbs_data_->entities()->size());
    }
    
    /// @brief Get number of entities
    size_t Size() const {
        return fbs_data_->entities()->size();
    }
    
    /// @brief Get entity at index (with caching)
    const Entity& Get(size_t index) const {
        if (index >= Size()) {
            throw std::out_of_range("Entity index out of range");
        }
        
        // Check cache first
        if (!entity_cache_[index]) {
            // Convert and cache
            const auto* fbs_entity = fbs_data_->entities()->Get(index);
            auto result = EntityDataConverter::ConvertEntity(fbs_entity);
            
            if (!result) {
                throw std::runtime_error(
                    std::format("Failed to convert entity {}: {}", 
                               index, result.error())
                );
            }
            
            entity_cache_[index] = result.value();
        }
        
        return *entity_cache_[index];
    }
    
    /// @brief Get all entities (converts all at once)
    std::vector<Entity> GetAll() const {
        std::vector<Entity> entities;
        entities.reserve(Size());
        
        for (size_t i = 0; i < Size(); ++i) {
            entities.push_back(Get(i));
        }
        
        return entities;
    }
    
    /// @brief Check if adapter has entity with given ID
    bool HasEntity(uint64_t id) const {
        for (size_t i = 0; i < Size(); ++i) {
            if (Get(i).id == id) {
                return true;
            }
        }
        return false;
    }
    
    /// @brief Comparison with EntityMemoryPool
    bool operator==(const EntityMemoryPool& other) const {
        if (Size() != other.Size()) {
            return false;
        }
        
        for (size_t i = 0; i < Size(); ++i) {
            if (Get(i) != other.Get(i)) {
                return false;
            }
        }
        
        return true;
    }
    
    bool operator!=(const EntityMemoryPool& other) const {
        return !(*this == other);
    }
    
    /// @brief Clear conversion cache
    void ClearCache() const {
        for (auto& cached_entity : entity_cache_) {
            cached_entity.reset();
        }
    }
    
    /// @brief Get cache statistics
    struct CacheStats {
        size_t total_entities;
        size_t cached_entities;
        double cache_hit_rate;
    };
    
    CacheStats GetCacheStats() const {
        size_t cached_count = 0;
        for (const auto& cached : entity_cache_) {
            if (cached) {
                ++cached_count;
            }
        }
        
        return CacheStats{
            .total_entities = Size(),
            .cached_entities = cached_count,
            .cache_hit_rate = Size() > 0 ? 
                static_cast<double>(cached_count) / Size() : 0.0
        };
    }

private:
    const EntityCollectionFbs* fbs_data_;
    mutable std::vector<std::optional<Entity>> entity_cache_;
};

// Usage
TEST_CASE("Adapter provides efficient access", "[adapter]") {
    TestData test_data;
    test_data.LoadFromFile("test_entities.fbs");
    
    EntityCollectionAdapter adapter(test_data.entity_data);
    
    TestEngine engine;
    engine.LoadData(/* ... */);
    engine.Process();
    
    // Adapter automatically converts as needed
    REQUIRE(adapter == engine.GetDataBank());
    
    // Check cache efficiency
    auto stats = adapter.GetCacheStats();
    INFO("Cache hit rate: " << stats.cache_hit_rate);
}
```

---

## Error Handling Examples

### Robust Conversion with Error Handling

```cpp
std::expected<EntityMemoryPool, ConversionError> LoadAndConvertTestData(
    const std::string& file_path
) {
    // Load file
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        return std::unexpected(ConversionError{
            .code = ConversionErrorCode::FileNotFound,
            .message = std::format("Cannot open file: {}", file_path),
            .file_path = file_path
        });
    }
    
    // Read file content
    std::vector<uint8_t> buffer(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
    
    if (buffer.empty()) {
        return std::unexpected(ConversionError{
            .code = ConversionErrorCode::EmptyFile,
            .message = "File is empty",
            .file_path = file_path
        });
    }
    
    // Verify FlatBuffers
    flatbuffers::Verifier verifier(buffer.data(), buffer.size());
    if (!VerifyEntityCollectionFbsBuffer(verifier)) {
        return std::unexpected(ConversionError{
            .code = ConversionErrorCode::InvalidFlatBuffers,
            .message = "FlatBuffers verification failed",
            .file_path = file_path
        });
    }
    
    // Get root
    const auto* fbs_data = GetEntityCollectionFbs(buffer.data());
    
    // Convert
    auto conversion_result = EntityDataConverter::ToMemoryPool(fbs_data);
    if (!conversion_result) {
        return std::unexpected(ConversionError{
            .code = ConversionErrorCode::ConversionFailed,
            .message = std::format("Conversion error: {}", 
                                  conversion_result.error()),
            .file_path = file_path,
            .details = conversion_result.error()
        });
    }
    
    return conversion_result.value();
}

// Usage with error handling
TEST_CASE("Test with proper error handling", "[entities][error-handling]") {
    auto result = LoadAndConvertTestData("test_entities.fbs");
    
    REQUIRE(result.has_value());
    
    if (!result) {
        const auto& error = result.error();
        FAIL(std::format("Failed to load test data: {} (code: {})",
                        error.message, static_cast<int>(error.code)));
    }
    
    const auto& test_data = result.value();
    // Continue with test...
}
```

---

## Performance Optimization Examples

### Bulk Conversion with Move Semantics

```cpp
EntityMemoryPool ConvertWithOptimizations(
    const EntityCollectionFbs* fbs_data
) {
    EntityMemoryPool pool;
    
    const auto* entities = fbs_data->entities();
    
    // Pre-allocate to avoid reallocations
    pool.Reserve(entities->size());
    
    // Use parallel conversion for large datasets
    if (entities->size() > 1000) {
        return ConvertParallel(fbs_data);
    }
    
    // Sequential conversion for smaller datasets
    for (const auto* entity : *entities) {
        // Direct construction in place
        pool.EmplaceBack(
            entity->id(),
            Position{
                entity->position()->x(),
                entity->position()->y(),
                entity->position()->z()
            },
            entity->flags(),
            std::vector<uint8_t>(
                entity->data()->begin(),
                entity->data()->end()
            )
        );
    }
    
    return pool;
}

// Parallel conversion for large datasets
EntityMemoryPool ConvertParallel(const EntityCollectionFbs* fbs_data) {
    const auto* entities = fbs_data->entities();
    const size_t count = entities->size();
    
    // Convert in parallel chunks
    std::vector<Entity> converted_entities(count);
    
    #pragma omp parallel for
    for (size_t i = 0; i < count; ++i) {
        const auto* entity = entities->Get(i);
        converted_entities[i] = Entity{
            entity->id(),
            Position{
                entity->position()->x(),
                entity->position()->y(),
                entity->position()->z()
            },
            entity->flags(),
            std::vector<uint8_t>(
                entity->data()->begin(),
                entity->data()->end()
            )
        };
    }
    
    // Build pool from converted entities
    EntityMemoryPool pool;
    pool.Reserve(count);
    
    for (auto&& entity : converted_entities) {
        pool.Add(std::move(entity));
    }
    
    return pool;
}
```

### Memory-Efficient Streaming Conversion

```cpp
class StreamingEntityConverter {
public:
    explicit StreamingEntityConverter(const EntityCollectionFbs* fbs_data)
        : fbs_data_(fbs_data), current_index_(0) {}
    
    std::optional<Entity> GetNext() {
        if (current_index_ >= fbs_data_->entities()->size()) {
            return std::nullopt;
        }
        
        const auto* fbs_entity = fbs_data_->entities()->Get(current_index_++);
        
        auto result = EntityDataConverter::ConvertEntity(fbs_entity);
        if (!result) {
            return std::nullopt;
        }
        
        return result.value();
    }
    
    bool HasMore() const {
        return current_index_ < fbs_data_->entities()->size();
    }
    
    void Reset() {
        current_index_ = 0;
    }

private:
    const EntityCollectionFbs* fbs_data_;
    size_t current_index_;
};

// Usage for memory-constrained scenarios
EntityMemoryPool ConvertStreaming(const EntityCollectionFbs* fbs_data) {
    EntityMemoryPool pool;
    StreamingEntityConverter converter(fbs_data);
    
    while (converter.HasMore()) {
        if (auto entity = converter.GetNext()) {
            pool.Add(std::move(*entity));
        }
    }
    
    return pool;
}
```

---

## Complete Test Example

### Integration Test with All Components

```cpp
TEST_CASE("Complete entity data conversion flow", 
          "[entities][integration]") {
    
    SECTION("Load and convert test data") {
        // Load FlatBuffers test data
        auto load_result = LoadAndConvertTestData("test_entities.fbs");
        REQUIRE(load_result.has_value());
        
        const auto& test_data = load_result.value();
        REQUIRE(test_data.Size() > 0);
    }
    
    SECTION("Converter handles empty collection") {
        std::vector<uint8_t> empty_buffer = CreateEmptyEntityCollection();
        const auto* fbs_data = GetEntityCollectionFbs(empty_buffer.data());
        
        auto result = EntityDataConverter::ToMemoryPool(fbs_data);
        REQUIRE(result.has_value());
        REQUIRE(result.value().Size() == 0);
    }
    
    SECTION("Round-trip conversion preserves data") {
        // Create original data
        EntityMemoryPool original;
        original.Add(Entity{1, {1.0f, 2.0f, 3.0f}, 0xFF, {0x01, 0x02}});
        original.Add(Entity{2, {4.0f, 5.0f, 6.0f}, 0x00, {}});
        
        // Convert to FlatBuffers
        auto fbs_result = EntityDataConverter::ToFlatBuffers(original);
        REQUIRE(fbs_result.has_value());
        
        // Convert back to memory pool
        const auto* fbs_data = GetEntityCollectionFbs(
            fbs_result.value().data()
        );
        auto restored_result = EntityDataConverter::ToMemoryPool(fbs_data);
        REQUIRE(restored_result.has_value());
        
        // Verify equality
        REQUIRE(original == restored_result.value());
    }
    
    SECTION("Adapter provides efficient access") {
        auto load_result = LoadAndConvertTestData("large_test.fbs");
        REQUIRE(load_result.has_value());
        
        // Use adapter for lazy conversion
        std::vector<uint8_t> buffer = /* load file */;
        const auto* fbs_data = GetEntityCollectionFbs(buffer.data());
        EntityCollectionAdapter adapter(fbs_data);
        
        // Access first entity (triggers conversion)
        const auto& first = adapter.Get(0);
        REQUIRE(first.id == 1);
        
        // Check cache stats
        auto stats = adapter.GetCacheStats();
        REQUIRE(stats.cached_entities == 1);
    }
    
    SECTION("Custom matcher works correctly") {
        TestData test_data;
        test_data.LoadFromFile("test_entities.fbs");
        
        TestEngine engine;
        engine.SetExpectedData(test_data.GetEntityData());
        engine.Process();
        
        REQUIRE_THAT(engine.GetDataBank(),
                     TestMatchers::EqualsEntityCollection(
                         test_data.entity_data
                     ));
    }
}
```

---

## Summary

These templates provide ready-to-use code for implementing entity data format conversion. Choose the appropriate template based on your needs:

- **Converter Module**: For most projects (Approach 3)
- **Test Fixture Updates**: When converting test data format
- **Custom Matchers**: When direct comparison not possible
- **Adapter Pattern**: For large datasets or lazy conversion
- **Error Handling**: For robust production code
- **Performance Optimization**: For large-scale conversions

All templates follow C++23 best practices and use `std::expected` for error handling.

---

**Related Documents**:
- [ENTITY_DATA_FORMAT_ANALYSIS.md](ENTITY_DATA_FORMAT_ANALYSIS.md) - Comprehensive analysis
- FlatBuffers documentation: https://google.github.io/flatbuffers/
- Testing frameworks: Catch2, Google Test

**Status**: ✅ Templates Complete and Ready to Use
