#pragma once
#include <bit>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <boost/unordered/unordered_flat_map.hpp>
#include <Luau/Compiler.h>
#include <v8.h>
#include <wasm.hh>

namespace Umi::Arguments {
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;
    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
    typedef int64_t i64;
    using std::move, std::vector, std::string, std::string_view, std::unique_ptr, std::make_unique, std::bit_cast, boost::unordered::unordered_flat_map;
    typedef unordered_flat_map<string, struct Argument> ufm;

    enum struct ArgumentType : u8 {
        SInt32, UInt32, SInt64, UInt64, Float32, Float64, Bool, String, Bytes, Func, Table, Handle,
        Count
    };

    struct Func {
        string name;
        vector<ArgumentType> expectedArgTypes;
    };

    struct Argument {
        union {
            i32 _i32;
            u32 _u32;
            i64 _i64;
            u64 _u64;
            float _f32;
            double _f64;
            bool _bool;
            string _str;
            vector<u8> _bytes;
            unique_ptr<Func> _func;
            ufm _table;
            void* _handle;
        };

        ArgumentType type{ArgumentType::Count};

        #pragma warning(suppress: 26495)
        [[nodiscard]] Argument() noexcept : type(ArgumentType::Count) {}
        [[nodiscard]] Argument(i32    data)            noexcept : type(ArgumentType::SInt32)  { new(&_i32)    i32(data);              }
        [[nodiscard]] Argument(u32    data)            noexcept : type(ArgumentType::UInt32)  { new(&_u32)    u32(data);              }
        [[nodiscard]] Argument(i64    data)            noexcept : type(ArgumentType::SInt64)  { new(&_i64)    i64(data);              }
        [[nodiscard]] Argument(u64    data)            noexcept : type(ArgumentType::UInt64)  { new(&_u64)    u64(data);              }
        [[nodiscard]] Argument(float  data)            noexcept : type(ArgumentType::Float32) { new(&_f32)    float(data);            }
        [[nodiscard]] Argument(double data)            noexcept : type(ArgumentType::Float64) { new(&_f64)    double(data);           }
        [[nodiscard]] Argument(bool   data)            noexcept : type(ArgumentType::Bool)    { new(&_bool)   bool(data);             }
        [[nodiscard]] Argument(const string& data)     noexcept : type(ArgumentType::String)  { new(&_str)    string(data);           }
        [[nodiscard]] Argument(string&& data)          noexcept : type(ArgumentType::String)  { new(&_str)    string(move(data));     }
        [[nodiscard]] Argument(const char* data)       noexcept : type(ArgumentType::String)  { new(&_str)    string(data);           }
        [[nodiscard]] Argument(const string_view data) noexcept : type(ArgumentType::String)  { new(&_str)    string(data);           }
        [[nodiscard]] Argument(const vector<u8>& data) noexcept : type(ArgumentType::Bytes)   { new(&_bytes)  vector<u8>(data);       }
        [[nodiscard]] Argument(vector<u8>&& data)      noexcept : type(ArgumentType::Bytes)   { new(&_bytes)  vector<u8>(move(data)); }
        //Prevent circular dependency
        [[nodiscard]] Argument(const Func& data)       noexcept;
        [[nodiscard]] Argument(Func&& data)            noexcept;
        [[nodiscard]] Argument(const ufm& data)        noexcept : type(ArgumentType::Table)   { new(&_table) ufm(data);               }
        [[nodiscard]] Argument(ufm&& data)             noexcept : type(ArgumentType::Table)   { new(&_table) ufm(move(data));         }
        [[nodiscard]] Argument(void* data)             noexcept : type(ArgumentType::Handle)  { new(&_handle) void*(data);            }

        //Prevent circular dependency
        [[nodiscard]] Argument           (const Argument& other) noexcept;
        [[nodiscard]] Argument           (Argument&& other)      noexcept;
        [[nodiscard]] Argument& operator=(const Argument& other) noexcept;
        [[nodiscard]] Argument& operator=(Argument&& other)      noexcept;

        ~Argument() {
            switch(type) {
                case ArgumentType::String: _str.~basic_string(); break;
                case ArgumentType::Bytes:  _bytes.~vector();     break;
                case ArgumentType::Func:   _func.~unique_ptr();  break;
                case ArgumentType::Table:  _table.~ufm();        break;
                default:                                         break;
            }
        }
    };

    struct FuncReturn {
        union {
            Argument ret;
            string err{"Undefined error"};
        };

        bool succeeded{false};
    };

    [[nodiscard]] inline Argument::Argument(const Argument& other) noexcept : type(other.type) {
        switch(type) {
            case ArgumentType::SInt32:  new(&_i32)    i32(other._i32);                          break;
            case ArgumentType::UInt32:  new(&_u32)    u32(other._u32);                          break;
            case ArgumentType::SInt64:  new(&_i64)    i64(other._i64);                          break;
            case ArgumentType::UInt64:  new(&_u64)    u64(other._u64);                          break;
            case ArgumentType::Float32: new(&_f32)    float(other._f32);                        break;
            case ArgumentType::Float64: new(&_f64)    double(other._f64);                       break;
            case ArgumentType::Bool:    new(&_bool)   bool(other._bool);                        break;
            case ArgumentType::String:  new(&_str)    string(other._str);                       break;
            case ArgumentType::Bytes:   new(&_bytes)  vector<u8>(other._bytes);                 break;
            case ArgumentType::Func:    new(&_func)   unique_ptr<Func>(new Func(*other._func)); break;
            case ArgumentType::Table:   new(&_table)  ufm(other._table);                        break;
            case ArgumentType::Handle:  new(&_handle) void*(other._handle);                     break;
            default:                                                                    break;
        }
    }

    [[nodiscard]] inline Argument::Argument(Argument&& other) noexcept : type(other.type) {
        switch(type) {
            case ArgumentType::SInt32:  new(&_i32)    i32(other._i32);                            break;
            case ArgumentType::UInt32:  new(&_u32)    u32(other._u32);                            break;
            case ArgumentType::SInt64:  new(&_i64)    i64(other._i64);                            break;
            case ArgumentType::UInt64:  new(&_u64)    u64(other._u64);                            break;
            case ArgumentType::Float32: new(&_f32)    float(other._f32);                          break;
            case ArgumentType::Float64: new(&_f64)    double(other._f64);                         break;
            case ArgumentType::Bool:    new(&_bool)   bool(other._bool);                          break;
            case ArgumentType::String:  new(&_str)    string(move(other._str));                   break;
            case ArgumentType::Bytes:   new(&_bytes)  vector<u8>(move(other._bytes));             break;
            case ArgumentType::Func:    new(&_func)   unique_ptr<Func>(move(other._func));        break;
            case ArgumentType::Table:   new(&_table)  ufm(move(other._table));                    break;
            case ArgumentType::Handle:  new(&_handle) void*(other._handle);                       break;
            default:                                                                      break;
        }
        other.type = ArgumentType::Count;
    }

    [[nodiscard]] inline Argument& Argument::operator=(const Argument& other) noexcept {
        if (this != &other) {
            this->~Argument();
            new(this) Argument(other);
        }
        return *this;
    }

    [[nodiscard]] inline Argument& Argument::operator=(Argument&& other) noexcept {
        if (this != &other) {
            this->~Argument();
            new(this) Argument(move(other));
        }
        return *this;
    }

    [[nodiscard]] inline Argument::Argument(const Func& data) noexcept : type(ArgumentType::Func) { new(&_func) unique_ptr<Func>(new Func(data));       }
    [[nodiscard]] inline Argument::Argument(Func&& data)      noexcept : type(ArgumentType::Func) { new(&_func) unique_ptr<Func>(new Func(move(data))); }
}