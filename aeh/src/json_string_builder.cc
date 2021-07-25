#include "json_string_builder.hh"
#include <cassert>

using namespace std::string_view_literals;

namespace aeh::json
{

    static void remove_last_comma(std::string & s)
    {
        if (s.size() > 0 && s.back() == ',')
            s.pop_back();
    }
    
    static void write_value_string(std::string & json_string, std::string_view str)
    {
        json_string += str;
        json_string += ',';
    }
    
    static void append_json_escaped_string(std::string & builder, std::string_view str)
    {
        builder += '"';
        for (char c : str)
        {
            switch (c)
            {
            case '"':
            case '\\':
            {
                builder += '\\';
                builder += c;
                break;
            }
            case '\b':
            {
                builder += '\\';
                builder += 'b';
                break;
            }
            case '\f':
            {
                builder += '\\';
                builder += 'f';
                break;
            }
            case '\n':
            {
                builder += '\\';
                builder += 'n';
                break;
            }
            case '\r':
            {
                builder += '\\';
                builder += 'r';
                break;
            }
            case '\t':
            {
                builder += '\\';
                builder += 't';
                break;
            }
            default:
            {
                builder += c;
                break;
            }
            }
        }
        builder += '"';
    }
    
    //***********************************************************************************************************

    void ValueBuilder::operator << (std::nullptr_t)
    {
        debug_check_write_value();
        write_value_string(json_string, "null"sv);
    }

    void ValueBuilder::operator << (int32_t value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (uint32_t value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (int64_t value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (uint64_t value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (float value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (double value)
    {
        debug_check_write_value();
        write_value_string(json_string, std::to_string(value));
    }

    void ValueBuilder::operator << (std::string_view value)
    {
        debug_check_write_value();
        append_json_escaped_string(json_string, value);
        json_string += ',';
    }

    auto ValueBuilder::object() -> ObjectBuilder
    {
        #ifdef NDEBUG
           return ObjectBuilder(json_string);
        #else
            debug_check_write_value();
            return ObjectBuilder(json_string, what_to_pass_to_child_as_parent);
        #endif
    }
    
    auto ValueBuilder::array() -> ArrayBuilder
    {
        #ifdef NDEBUG
            return ArrayBuilder(json_string);
        #else
            debug_check_write_value();
            return ArrayBuilder(json_string, what_to_pass_to_child_as_parent);
        #endif
    }

    //***********************************************************************************************************
    
    #ifndef NDEBUG
    ObjectMemberBuilder::~ObjectMemberBuilder()
    {
        assert(single_value_written);
    }
    #endif
    
    #ifndef NDEBUG
    void ObjectMemberBuilder::debug_check_write_value()
    {
        assert(single_value_written == false);
        single_value_written = true;
    }
    #endif
    
    //***********************************************************************************************************
    
    #ifdef NDEBUG
    ObjectBuilder::ObjectBuilder(std::string & s) 
        : json_string(s) 
    {
        json_string += '{'; 
    }
    #else
    ObjectBuilder::ObjectBuilder(std::string & s, bool & parent) 
        : json_string(s)
        , parent_parsing_child(parent)
    {
        json_string += '{';
        parent_parsing_child = true;
    }
    #endif
    
    ObjectBuilder::~ObjectBuilder()
    {
        remove_last_comma(json_string); 
        json_string += "},"; 
    
        #ifndef NDEBUG
            assert(parsing_child == false);
            parent_parsing_child = false;
        #endif
    }
    
    auto ObjectBuilder::operator[] (std::string_view key) -> ObjectMemberBuilder
    {
        append_json_escaped_string(json_string, key);
        json_string += ":";
    
    #ifdef NDEBUG
        return ObjectMemberBuilder(json_string);
    #else
        assert(parsing_child == false);
        return ObjectMemberBuilder(json_string, parsing_child);
    #endif
    }
    
    //***********************************************************************************************************
    
    #ifdef NDEBUG
    ArrayBuilder::ArrayBuilder(std::string & s)
        : ValueBuilder(s)
    {
        json_string += '[';
    }
    #else
    ArrayBuilder::ArrayBuilder(std::string & s, bool & parent)
        : ValueBuilder(s, parsing_child)
        , parent_parsing_child(parent)
    {
        json_string += '[';
        parent_parsing_child = true;
    }
    #endif
    
    ArrayBuilder::~ArrayBuilder()
    {
        remove_last_comma(json_string);
        json_string += "],";
    
        #ifndef NDEBUG
            assert(parsing_child == false);
            parent_parsing_child = false;
        #endif
    }

    #ifndef NDEBUG
    void ArrayBuilder::debug_check_write_value()
    {
        assert(parsing_child == false);
    }
    #endif
    
    //***********************************************************************************************************
    
    StringBuilder::StringBuilder(std::string & s) 
#ifdef NDEBUG
        : ObjectMemberBuilder(s) 
#else
        : ObjectMemberBuilder(s, parsing_child)
#endif
    {}

    StringBuilder::~StringBuilder()
    {
        #ifndef NDEBUG
            assert(parsing_child == false);
        #endif
    
        remove_last_comma(json_string);
    }
    
    //***********************************************************************************************************
    
    auto MultipleStringBuilder::next() -> StringBuilder
    {
        if (!json_string.empty())
            json_string += separator;
    
        return StringBuilder(json_string);
    }

} // namespace json
