#pragma once
#include <stdexcept>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>

#include "../../debug/loggers.hpp"
#include "../../util/RuntimeBijection.hpp"

namespace World::Record {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::invalid_argument, std::vector, boost::unordered_flat_map, Util::RuntimeBijection;

    enum struct RecordType : u8 {
    //Fixed sized types. These types store the actual data.
        Record,
        Int8, Int16, Int32, Int64,
        Uint8, Uint16, Uint32, Uint64,
        Float, Double,
        Bool,

    //Dynamic sized types. These types store a handle to the actual data.
        Array, //Dynamic array (vector). Use plain field combination for fixed-size ones.
        String,

        Count
    };

    struct RecordSchema {
        using enum RecordType;

        RecordType type;
        union {
            struct Record {
                vector<RecordSchema*> schema;
            } record;
            struct Array {
                RecordSchema* elementType;
            } array;
        };

        RecordSchema() noexcept : type(Count) {}
        template <RecordType type>
        [[nodiscard]] RecordSchema() noexcept : type(type) {
            static_assert(type != Record && type != Array, "Use the respective constructors.");
        }
        [[nodiscard]] RecordSchema(const vector<RecordSchema*>& schema) noexcept : type(Record), record{schema} {}
        [[nodiscard]] RecordSchema(RecordSchema* elementType) noexcept : type(Array), array{elementType} {}

        void destroyUnion() noexcept {
            switch (type) {
            case Record:
                record.schema.~vector();
                break;
            default:
                break;
            }
        }

        [[nodiscard]] u64 size() const noexcept {
            switch (type) {
                case Record: {
                    u64 sizeSum = 0;
                    for (u64 i = 0; i < record.schema.size(); i++) sizeSum += record.schema[i]->size();
                    return sizeSum;
                }
                case Int8: return 1;
                case Int16: return 2;
                case Int32: return 4;
                case Int64: return 8;
                case Uint8: return 1;
                case Uint16: return 2;
                case Uint32: return 4;
                case Uint64: return 8;
                case Float: return 4;
                case Double: return 8;
                case Bool: return 1;
                case Array:
                case String:
                    //todo: This is a placeholder for handle size. Handle is not implemented. REMEMBER TO CHANGE THIS.
                    return 8;
                case Count:
                    lout << "`RecordSchema::size()` called on a `RecordSchema` with type `Count`." << nlaf;
                    return 0;
            }
        }

        [[nodiscard]] bool isFixedSize() const noexcept {
            switch (type) {
                case Record:
                    for (u64 i = 0; i < record.schema.size(); i++) if (!record.schema[i]->isFixedSize()) return false;
                    return true;
                case Int8: case Int16: case Int32: case Int64:
                case Uint8: case Uint16: case Uint32: case Uint64:
                case Float: case Double:
                case Bool:
                case Array: case String:
                    return false;
                case Count:
                    lout << "`RecordSchema::isFixedSize()` called on a `RecordSchema` with type `Count`." << nlaf;
                    return false;
            }
        }
    };

    //todo: RecordSchema <-> Storage to implement versions on records, so packs don't register a new one and reinterpret cast all the data to nonsense (or to attack the game).
}