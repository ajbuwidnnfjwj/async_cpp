#pragma once
#include <map>
#include <string>
#include <functional>

class Router {
private:
    static Router* r;

    Router();
    Router(Router&);
    Router(Router&&);

public:

    std::map<std::string, std::function<std::string(void)>> get;
    std::map<std::string, std::function<std::string(void)>> post;
    std::map<std::string, std::function<std::string(void)>> del;
    std::map<std::string, std::function<std::string(void)>> update;

public:
    static Router* get_instance() {
        if (r == nullptr) {
            r = new Router();
            return r;
        }
        else {
            return r;
        }
    }

    // Get 함수 선언
    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<std::string(void)>, T>::value>>
    void Get(const std::string path, const T&& callback) {
        get.emplace(path, std::function<void(void)>(callback));
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<std::string(void)>, T>::value>>
    void Post(std::string path, T callback) {
        post.emplace(path, std::function<void(void)>(callback));
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<std::string(void)>, T>::value>>
    void Delete(std::string path, T callback) {
        del.emplace(path, std::function<void(void)>(callback));
    }

    template <class T, typename = typename std::enable_if_t<std::is_constructible<std::function<std::string(void)>, T>::value>>
    void Update(std::string path, T callback) {
        update.emplace(path, std::function<void(void)>(callback));
    }

    ~Router() {
        delete r;
    }
};

Router* router = Router::get_instance();