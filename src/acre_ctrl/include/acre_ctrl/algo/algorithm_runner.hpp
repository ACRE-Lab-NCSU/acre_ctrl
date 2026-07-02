#pragma once
#include "algorithm_base.hpp"
#include "component_registry.hpp"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <filesystem>

namespace py = pybind11;

class AlgorithmRunner {
public:
    explicit AlgorithmRunner(const std::string& path)
    {
        if (!path.ends_with(".py"))
            throw std::runtime_error(
                "AlgorithmRunner only loads Python algorithms (.py). "
                "For C++ algorithms write your own node.");

        load_python(path);
    }

    void init() {
        instance_->attr("init")();
    }

    std::vector<std::string> required() const {
        return instance_->attr("required")().cast<std::vector<std::string>>();
    }

    ControlOutput compute(const ComponentRegistry& world) {
        return instance_->attr("compute")(world).cast<ControlOutput>();
    }

private:
    std::unique_ptr<py::object> instance_;

    void load_python(const std::string& path)
    {
        namespace fs = std::filesystem;

        if (!Py_IsInitialized()) {
            static py::scoped_interpreter interp{};
        }

        std::string dir  = fs::path(path).parent_path().string();
        std::string stem = fs::path(path).stem().string();

        py::module_ sys = py::module_::import("sys");
        sys.attr("path").attr("insert")(0, dir);

        py::module_ mod = py::module_::import(stem.c_str());
        py::object  base = py::module_::import("acre_ctrl_algorithm")
                               .attr("ControlSystem");

        // Find the ControlSystem subclass in the module
        for (auto item : mod.attr("__dict__").cast<py::dict>()) {
            py::object val = item.second.cast<py::object>();
            if (py::isinstance<py::type>(val)  &&
                val.attr("__name__").cast<std::string>() != "ControlSystem" &&
                PyObject_IsSubclass(val.ptr(), base.ptr()))
            {
                instance_ = std::make_unique<py::object>(val());
                return;
            }
        }

        throw std::runtime_error(
            "No ControlSystem subclass found in: " + path);
    }
};