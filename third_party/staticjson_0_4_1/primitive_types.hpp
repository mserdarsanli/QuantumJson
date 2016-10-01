// The MIT License (MIT)
//
// Copyright (c) 2014 Siyuan Ren (netheril96@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <staticjson/basic.hpp>

#include <limits>
#include <string>
#include <type_traits>

namespace staticjson
{

template <class IntType>
class IntegerHandler : public BaseHandler
{
    static_assert(std::is_arithmetic<IntType>::value, "Only arithmetic types are allowed");

protected:
    IntType* m_value;

    template <class AnotherIntType>
    bool is_out_of_range(AnotherIntType a)
    {
        typedef typename std::common_type<IntType, AnotherIntType>::type CommonType;
        bool this_signed = std::numeric_limits<IntType>::is_signed,
             that_signed = std::numeric_limits<AnotherIntType>::is_signed;

        if (this_signed == that_signed)
            return CommonType(a) < CommonType(std::numeric_limits<IntType>::min())
                || CommonType(a) > CommonType(std::numeric_limits<IntType>::max());

        if (this_signed)
            return CommonType(a) > CommonType(std::numeric_limits<IntType>::max());

        return a < 0 || CommonType(a) > CommonType(std::numeric_limits<IntType>::max());
    }

public:
    explicit IntegerHandler(IntType* value) : m_value(value) {}

    bool Int(int i) override
    {
        if (is_out_of_range(i))
            return set_out_of_range("int");
        *m_value = static_cast<IntType>(i);
        this->parsed = true;
        return true;
    }

    bool Uint(unsigned i) override
    {
        if (is_out_of_range(i))
            return set_out_of_range("unsigned int");
        *m_value = static_cast<IntType>(i);
        this->parsed = true;
        return true;
    }

    bool Int64(std::int64_t i) override
    {
        if (is_out_of_range(i))
            return set_out_of_range("std::int64_t");
        *m_value = static_cast<IntType>(i);
        this->parsed = true;
        return true;
    }

    bool Uint64(std::uint64_t i) override
    {
        if (is_out_of_range(i))
            return set_out_of_range("std::uint64_t");
        *m_value = static_cast<IntType>(i);
        this->parsed = true;
        return true;
    }

    bool Double(double d) override
    {
        *m_value = static_cast<IntType>(d);
        if (static_cast<double>(*m_value) != d)
            return set_out_of_range("double");
        this->parsed = true;
        return true;
    }

    bool write(IHandler* output) const override
    {
        if (std::numeric_limits<IntType>::is_signed)
        {
            return output->Int64(*m_value);
        }
        else
        {
            return output->Uint64(*m_value);
        }
    }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("integer"), alloc);
        Value minimum, maximum;
        if (std::numeric_limits<IntType>::is_signed)
        {
            minimum.SetInt64(std::numeric_limits<IntType>::min());
            maximum.SetInt64(std::numeric_limits<IntType>::max());
        }
        else
        {
            minimum.SetUint64(std::numeric_limits<IntType>::min());
            maximum.SetUint64(std::numeric_limits<IntType>::max());
        }
        output.AddMember(rapidjson::StringRef("minimum"), minimum, alloc);
        output.AddMember(rapidjson::StringRef("maximum"), maximum, alloc);
    }
};

template <>
class Handler<std::nullptr_t> : public BaseHandler
{
public:
    explicit Handler(std::nullptr_t*) {}

    bool Null() override { return true; }

    std::string type_name() const override { return "null"; }

    bool write(IHandler* output) const override { return output->Null(); }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("null"), alloc);
    }
};

template <>
class Handler<bool> : public BaseHandler
{
private:
    bool* m_value;

public:
    explicit Handler(bool* value) : m_value(value) {}

    bool Bool(bool v) override
    {
        *m_value = v;
        this->parsed = true;
        return true;
    }

    std::string type_name() const override { return "bool"; }

    bool write(IHandler* output) const override { return output->Bool(*m_value); }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("boolean"), alloc);
    }
};

template <>
class Handler<int> : public IntegerHandler<int>
{
public:
    explicit Handler(int* i) : IntegerHandler<int>(i) {}

    std::string type_name() const override { return "int"; }

    bool write(IHandler* output) const override { return output->Int(*m_value); }
};

template <>
class Handler<unsigned int> : public IntegerHandler<unsigned int>
{
public:
    explicit Handler(unsigned* i) : IntegerHandler<unsigned int>(i) {}

    std::string type_name() const override { return "unsigned int"; }

    bool write(IHandler* output) const override { return output->Uint(*m_value); }
};

template <>
class Handler<long> : public IntegerHandler<long>
{
public:
    explicit Handler(long* i) : IntegerHandler<long>(i) {}

    std::string type_name() const override { return "long"; }
};

template <>
class Handler<unsigned long> : public IntegerHandler<unsigned long>
{
public:
    explicit Handler(unsigned long* i) : IntegerHandler<unsigned long>(i) {}

    std::string type_name() const override { return "unsigned long"; }
};

template <>
class Handler<long long> : public IntegerHandler<long long>
{
public:
    explicit Handler(long long* i) : IntegerHandler<long long>(i) {}

    std::string type_name() const override { return "long long"; }
};

template <>
class Handler<unsigned long long> : public IntegerHandler<unsigned long long>
{
public:
    explicit Handler(unsigned long long* i) : IntegerHandler<unsigned long long>(i) {}

    std::string type_name() const override { return "unsigned long long"; }
};

// char is an alias for bool to work around the stupid `std::vector<bool>`
template <>
class Handler<char> : public BaseHandler
{
private:
    char* m_value;

public:
    explicit Handler(char* i) : m_value(i) {}

    std::string type_name() const override { return "bool"; }

    bool Bool(bool v) override
    {
        *this->m_value = v;
        this->parsed = true;
        return true;
    }

    bool write(IHandler* out) const override { return out->Bool(*m_value); }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("boolean"), alloc);
    }
};

template <>
class Handler<double> : public BaseHandler
{
private:
    double* m_value;

public:
    explicit Handler(double* v) : m_value(v) {}

    bool Int(int i) override
    {
        *m_value = i;
        this->parsed = true;
        return true;
    }

    bool Uint(unsigned i) override
    {
        *m_value = i;
        this->parsed = true;
        return true;
    }

    bool Int64(std::int64_t i) override
    {
        *m_value = static_cast<double>(i);
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("std::int64_t");
        this->parsed = true;
        return true;
    }

    bool Uint64(std::uint64_t i) override
    {
        *m_value = static_cast<double>(i);
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("std::uint64_t");
        this->parsed = true;
        return true;
    }

    bool Double(double d) override
    {
        *m_value = d;
        this->parsed = true;
        return true;
    }

    std::string type_name() const override { return "double"; }

    bool write(IHandler* out) const override { return out->Double(*m_value); }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("number"), alloc);
    }
};

template <>
class Handler<float> : public BaseHandler
{
private:
    float* m_value;

public:
    explicit Handler(float* v) : m_value(v) {}

    bool Int(int i) override
    {
        *m_value = i;
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("int");
        this->parsed = true;
        return true;
    }

    bool Uint(unsigned i) override
    {
        *m_value = i;
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("unsigned int");
        this->parsed = true;
        return true;
    }

    bool Int64(std::int64_t i) override
    {
        *m_value = static_cast<float>(i);
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("std::int64_t");
        this->parsed = true;
        return true;
    }

    bool Uint64(std::uint64_t i) override
    {
        *m_value = static_cast<float>(i);
        if (static_cast<decltype(i)>(*m_value) != i)
            return set_out_of_range("std::uint64_t");
        this->parsed = true;
        return true;
    }

    bool Double(double d) override
    {
        *m_value = d;
        if (static_cast<decltype(d)>(*m_value) != d)
            return set_out_of_range("double");
        this->parsed = true;
        return true;
    }

    std::string type_name() const override { return "float"; }

    bool write(IHandler* out) const override { return out->Double(*m_value); }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("number"), alloc);
    }
};

template <>
class Handler<std::string> : public BaseHandler
{
private:
    std::string* m_value;

public:
    explicit Handler(std::string* v) : m_value(v) {}

    bool String(const char* str, SizeType length, bool) override
    {
        m_value->assign(str, length);
        this->parsed = true;
        return true;
    }

    std::string type_name() const override { return "string"; }

    bool write(IHandler* out) const override
    {
        return out->String(m_value->data(), SizeType(m_value->size()), true);
    }

    void generate_schema(Value& output, MemoryPoolAllocator& alloc) const override
    {
        output.SetObject();
        output.AddMember(rapidjson::StringRef("type"), rapidjson::StringRef("string"), alloc);
    }
};
}
