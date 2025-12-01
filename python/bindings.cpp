/**
 * @file bindings.cpp
 * @brief Python bindings for brainmaze_mefd using pybind11
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <brainmaze_mefd/mef.hpp>

namespace py = pybind11;
using namespace brainmaze_mefd;

PYBIND11_MODULE(_brainmaze_mefd, m) {
    m.doc() = "Modern C++ MEF 3.0 library - Python bindings";
    
    // Version functions
    m.def("get_version", &get_version, "Get library version");
    m.def("get_mef_version", &get_mef_version, "Get MEF format version");
    
    // MefReader class
    py::class_<MefReader>(m, "MefReader", "MEF 3.0 session reader")
        .def(py::init<const std::string&, const std::string&>(),
             py::arg("path"),
             py::arg("password") = "",
             "Open a MEF session for reading")
        .def("is_valid", &MefReader::is_valid, "Check if session is valid")
        .def("get_path", &MefReader::get_path, "Get session path")
        .def("get_session_name", &MefReader::get_session_name, "Get session name")
        .def_property_readonly("channels", &MefReader::get_channels, "List of channel names")
        .def("get_channels", &MefReader::get_channels, "Get list of channel names")
        .def("get_time_series_channels", &MefReader::get_time_series_channels, 
             "Get list of time series channel names")
        .def("get_start_time", &MefReader::get_start_time, "Get session start time")
        .def("get_end_time", &MefReader::get_end_time, "Get session end time")
        .def("get_duration", &MefReader::get_duration, "Get recording duration in microseconds")
        .def("get_property", [](const MefReader& reader, const std::string& prop, 
                                const std::string& channel) {
            try {
                return py::cast(reader.get_numeric_property(prop, channel));
            } catch (...) {
                return py::cast(reader.get_string_property(prop, channel));
            }
        }, py::arg("property_name"), py::arg("channel_name") = "",
           "Get a property value")
        .def("get_data", [](const MefReader& reader, const std::string& channel,
                            py::object start, py::object end) {
            si8 start_time = 0, end_time = 0;
            si8* p_start = nullptr;
            si8* p_end = nullptr;
            
            if (!start.is_none()) {
                start_time = start.cast<si8>();
                p_start = &start_time;
            }
            if (!end.is_none()) {
                end_time = end.cast<si8>();
                p_end = &end_time;
            }
            
            auto data = reader.get_data(channel, p_start, p_end);
            
            // Convert to numpy array
            py::array_t<sf8> result(static_cast<py::ssize_t>(data.size()));
            auto buf = result.request();
            std::copy(data.begin(), data.end(), static_cast<sf8*>(buf.ptr));
            return result;
        }, py::arg("channel_name"), py::arg("start_time") = py::none(), 
           py::arg("end_time") = py::none(),
           "Read data from a channel");
    
    // MefWriter class
    py::class_<MefWriter>(m, "MefWriter", "MEF 3.0 session writer")
        .def(py::init<const std::string&, bool, const std::string&, const std::string&>(),
             py::arg("path"),
             py::arg("overwrite") = true,
             py::arg("password1") = "",
             py::arg("password2") = "",
             "Create a MEF session for writing")
        .def("is_valid", &MefWriter::is_valid, "Check if writer is valid")
        .def_property("mef_block_len", &MefWriter::get_mef_block_len, &MefWriter::set_mef_block_len,
                      "MEF block length (samples per block)")
        .def_property("max_nans_written", &MefWriter::get_max_nans_written, 
                      &MefWriter::set_max_nans_written,
                      "Maximum NaN samples to write")
        .def_property("data_units", &MefWriter::get_data_units, &MefWriter::set_data_units,
                      "Data units description")
        .def_property("units_conversion_factor", &MefWriter::get_units_conversion_factor,
                      &MefWriter::set_units_conversion_factor,
                      "Units conversion factor")
        .def_property("recording_time_offset", &MefWriter::get_recording_time_offset,
                      &MefWriter::set_recording_time_offset,
                      "Recording time offset")
        .def_property("gmt_offset", &MefWriter::get_gmt_offset, &MefWriter::set_gmt_offset,
                      "GMT offset in seconds")
        .def_property("subject_name", &MefWriter::get_subject_name, &MefWriter::set_subject_name,
                      "Subject name")
        .def_property("subject_id", &MefWriter::get_subject_id, &MefWriter::set_subject_id,
                      "Subject ID")
        .def_property("recording_location", &MefWriter::get_recording_location,
                      &MefWriter::set_recording_location,
                      "Recording location")
        .def_property("channel_description", &MefWriter::get_channel_description,
                      &MefWriter::set_channel_description,
                      "Channel description")
        .def_property("session_description", &MefWriter::get_session_description,
                      &MefWriter::set_session_description,
                      "Session description")
        .def("write_data", [](MefWriter& writer, py::array_t<sf8> data,
                              const std::string& channel, si8 start_uutc,
                              sf8 sampling_freq, si4 precision, bool new_segment) {
            auto buf = data.request();
            if (buf.ndim != 1) {
                throw std::runtime_error("Data must be 1-dimensional");
            }
            writer.write_data(static_cast<const sf8*>(buf.ptr), 
                              static_cast<size_t>(buf.size),
                              channel, start_uutc, sampling_freq, precision, new_segment);
        }, py::arg("data"), py::arg("channel_name"), py::arg("start_uutc"),
           py::arg("sampling_freq"), py::arg("precision") = -1, 
           py::arg("new_segment") = false,
           "Write data to a channel")
        .def("flush", &MefWriter::flush, "Flush data to disk")
        .def("close", &MefWriter::close, "Close the session");
    
    // Constants
    m.attr("MEF_VERSION_MAJOR") = MEF_VERSION_MAJOR;
    m.attr("MEF_VERSION_MINOR") = MEF_VERSION_MINOR;
    m.attr("UUTC_NO_ENTRY") = UUTC_NO_ENTRY;
    m.attr("RED_NAN") = RED_NAN;
}
