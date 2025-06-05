#pragma once
#include "../../ext/alias.h"
#include "../../ext/autoSerialization.h"

namespace jamd::structs::api::instances
{
    /**
    * @struct LaunchResult
    * @brief 储存启动实例API的结果
    */
    struct LaunchResult
    {
        bool success;
        short code;
        String message;

        struct hana_accessors_impl
        {
            static constexpr auto apply()
            {
                struct member_names
                {
                    static constexpr auto get() { return hana::make_tuple("success", "code", "message"); }
                };
                return hana::make_tuple(
                    hana::make_pair(::boost::hana::struct_detail::prepare_member_name<0, member_names>(),
                                    ::boost::hana::struct_detail::member_ptr<
                                        decltype(&LaunchResult::success), &LaunchResult::success>{}),
                    hana::make_pair(::boost::hana::struct_detail::prepare_member_name<1, member_names>(),
                                    ::boost::hana::struct_detail::member_ptr<
                                        decltype(&LaunchResult::code), &LaunchResult::code>{}),
                    hana::make_pair(::boost::hana::struct_detail::prepare_member_name<2, member_names>(),
                                    ::boost::hana::struct_detail::member_ptr<
                                        decltype(&LaunchResult::message), &LaunchResult::message>{}));
            }
        };

        LaunchResult() = default;

        template <typename... Args>
        explicit LaunchResult(Args&&... args) { init_fields(std::forward<Args>(args)...); }

        static void serialize_fields(const LaunchResult& obj, Json::Value& json)
        {
            hana::for_each(hana::keys(obj), [&](auto key)
            {
                constexpr auto name = hana::to<char const*>(key);
                json[name] = json_serializer::toJson(hana::at_key(obj, key));
            });
        }

        static void deserialize_fields(const Json::Value& json, LaunchResult& obj)
        {
            hana::for_each(hana::keys(obj), [&](auto key)
            {
                constexpr auto name = hana::to<char const*>(key);
                if (json.isMember(name)) { json_serializer::fromJson(json[name], hana::at_key(obj, key)); }
            });
        }

        Json::Value toJson() const
        {
            Json::Value json;
            serialize_fields(*this, json);
            return json;
        }

        static LaunchResult fromJson(const Json::Value& json)
        {
            LaunchResult obj;
            deserialize_fields(json, obj);
            return obj;
        }

        operator HttpResponsePtr() const { return HttpResponse::newHttpJsonResponse(toJson()); }

    private:
        template <typename... Args>
        void init_fields(Args&&... args)
        {
            hana::for_each(hana::keys(*this), [&](auto key)
            {
                constexpr auto name = hana::to<char const*>(key);
                init_field_impl<decltype(key)>(name, std::forward<Args>(args)...);
            });
        }

        template <typename Key, typename... Args>
        void init_field_impl(const char* field_name, Args&&... args)
        {
            if constexpr (sizeof...(Args) > 0) { (init_single_field<Key>(field_name, std::forward<Args>(args)), ...); }
        }

        template <typename Key, typename T>
        void init_single_field(const char* field_name, T&& value)
        {
            if constexpr (std::is_convertible_v<T, std::remove_reference_t<decltype(hana::at_key(*this, Key{}))>>)
            {
                hana::at_key(*this, Key{}) = std::forward<T>(value);
            }
        }
    };
}
