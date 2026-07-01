#pragma once
#include "algorithm_base.hpp"
#include <pybind11/embed.h>
#include <dlfcn.h>
#include <stdexcept>
#include <functional>
#include <string>

namespace py = pybind11;

class AlgorithmRunner {
public:
    explicit AlgorithmRunner(const std::string& path)
    {
        if (path.ends_with(".so"))      load_cpp(path);
        else if (path.ends_with(".py")) load_python(path);
        else throw std::runtime_error("Unsupported file type: " + path);
    }

    ~AlgorithmRunner() {
        if (destroy_fn_ && instance_) destroy_fn_(instance_);
        if (handle_) dlclose(handle_);
    }

    AlgorithmInputType input_type() const { return input_type_; }

    ControlOutput compute(const void* input) {
        return compute_fn_(instance_, input);
    }

private:
    void*              handle_   = nullptr;
    void*              instance_ = nullptr;
    AlgorithmInputType input_type_;

    std::function<void(void*)>                    destroy_fn_;
    std::function<ControlOutput(void*, const void*)> compute_fn_;

    void load_cpp(const std::string& path) {
        handle_ = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle_) throw std::runtime_error(dlerror());

        auto create   = (void*(*)())            dlsym(handle_, "algo_create");
        auto destroy  = (void(*)(void*))        dlsym(handle_, "algo_destroy");
        auto init     = (void(*)(void*))        dlsym(handle_, "algo_init");
        auto compute  = (ControlOutput(*)(void*, const void*))
                            dlsym(handle_, "algo_compute");
        auto get_type = (uint32_t(*)())         dlsym(handle_, "algo_input_type");

        if (!create || !destroy || !compute || !get_type)
            throw std::runtime_error("Missing required symbols in: " + path);

        instance_   = create();
        input_type_ = static_cast<AlgorithmInputType>(get_type());
        destroy_fn_ = destroy;
        compute_fn_ = [compute](void* p, const void* in) {
            return compute(p, in);
        };
        if (init) init(instance_);
    }

    void load_python(const std::string& path) {
        /**
        py::scoped_interpreter guard{}; // Start the Python Interprtor
        py::module_ sys = py::module_::import("sys");
        py::modeule_ algo = py::module_::import(path); // import the algorithm file at the given path
        instance_ = algo.attr("init"); // Initlize the instance 
        input_type_ = static_cast<AlgorithmInputType>(algo.attr("type"));
        compute_fn_ = [compute](void* p, const void* in) {
            return algo.attr("compute")(p, in);
        };
        */


    }
};