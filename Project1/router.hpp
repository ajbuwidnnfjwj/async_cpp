#pragma once
#include <map>
#include <string>
#include <functional>

class router {
public:
    std::map<std::string, std::function<void(void)>> get;
    std::map<std::string, std::function<void(void)>> post;
    std::map<std::string, std::function<void(void)>> del;
    std::map<std::string, std::function<void(void)>> update;

    // Get 함수 선언
    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    void Get(const std::string path, const T&& callback) {
        get.emplace(path, std::function<void(void)>(callback));
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    void Post(std::string path, T callback) {
        post[path] = callback;
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    void Delete(std::string path, T callback) {
        del[path] = callback;
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<void(void)>, T>::value>>
    void Update(std::string path, T callback) {
        update[path] = callback;
    }
};