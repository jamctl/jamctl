#pragma once
#include <deque>
#include <list>
#include <map>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <boost/asio.hpp>
#include <mutex>
#include <regex>
#include <drogon/orm/Criteria.h>

// stdfuns

template <typename T> using Function = std::function<T>;
using String = std::string;
using StringView = std::string_view;
using Path = std::filesystem::path;
using Regex = std::regex;
constexpr auto OptimizeRegex = std::regex::optimize;

// smart ptrs
template <typename T> using UniquePtr = std::unique_ptr<T>;
template <typename T> using SharedPtr = std::shared_ptr<T>;
template <typename T> using WeakPtr = std::weak_ptr<T>;

// STL

template <typename T> using Vector = std::vector<T>;
template <typename T> using List = const std::list<T>;
template <typename T> using MutableList = std::list<T>;
template <typename T> using Deque = const std::deque<T>;
template <typename T> using MutableDeque = std::deque<T>;
template <typename K, typename V,
          typename C = std::less<K>,
          typename A = std::pair<const K, V>> using Map = const std::map<K, V, C, A>;
template <typename K, typename V,
          typename C = std::less<K>,
          typename A = std::pair<const K, V>> using MutableMap = std::map<K, V, C, A>;
template <typename K, typename V,
          typename H = std::hash<V>,
          typename P = std::equal_to<V>,
          typename A = std::pair<const K, V>> using UnorderedMap = std::unordered_map<K, V, H, P, A>;
template <typename K, typename V> using Pair = std::pair<K, V>;
template <typename T> using Optional = std::optional<T>;
template <typename T> using RefWrapper = std::reference_wrapper<T>;

// asio
using Mutex = std::mutex;
using IOContext = boost::asio::io_context;
using WritablePipe = boost::asio::writable_pipe;
using ReadablePipe = boost::asio::readable_pipe;
using Thread = std::thread;


// drogon

using Request = drogon::HttpRequestPtr;
using Response = drogon::HttpResponsePtr;
using RouteCallback = Function<void(const Response&)>;

using where = drogon::orm::Criteria;
