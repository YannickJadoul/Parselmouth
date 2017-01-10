#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_PLUGIN(parselmouth) {
    py::module m("parselmouth");

    return m.ptr();
}
