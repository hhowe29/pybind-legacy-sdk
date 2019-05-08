#include <pybind11/pybind11.h>
#include <iostream>
#include <thread>
#include <memory>

using namespace std;
namespace py = pybind11;

//--------------------------------------------------------
// From an SDK I can't control
struct EventListener
{
    virtual void OnEvent(int code, std::string &text)
    {
        cout << __PRETTY_FUNCTION__ << endl;
    };

    virtual ~EventListener() {}
};

EventListener *glistener = 0;
void register_listener(EventListener *listener)
{
    glistener = listener;
}
//--------------------------------------------------------

//--------------------------------------------------------
// My bindings for that SDK
struct PyEventListener : public EventListener {
    void OnEvent(int code, std::string &text)
    {
        cout << __PRETTY_FUNCTION__ << endl;
        py::gil_scoped_acquire acquire;
        PYBIND11_OVERLOAD(
          void,
          EventListener,
          OnEvent,
          code, text
        );
    }
};

std::shared_ptr<EventListener> listenerRef = {};

void register_pylistener(std::shared_ptr<EventListener> listener) {
    cout << __PRETTY_FUNCTION__ << endl;
    listenerRef = std::move(listener);
    register_listener(listenerRef.get());
}

void fire()
{
    cout << __PRETTY_FUNCTION__ << endl;
    std::string text = "alpha";
    glistener->OnEvent(29, text);

}

void fire_async()
{
    // In reality, events will fire asynchronously based on interaction with hardware
    cout << __PRETTY_FUNCTION__ << endl;
    auto func = []() {
        cout << __PRETTY_FUNCTION__ << endl;
        std::string text = "alpha";
        glistener->OnEvent(29, text);

        cout << "Exiting lambda" << endl;

    };
    auto task = thread(func);
    task.detach();
}


PYBIND11_MODULE(example, m)
{
    py::class_<EventListener, std::shared_ptr<EventListener>, PyEventListener>(m, "EventListener")
        .def(py::init<>())
        .def("OnEvent", &EventListener::OnEvent);

    m.def("register_listener", &register_pylistener, "Registers a listener with the SDK");
    m.def("fire", &fire, "Fire an event handler synchronously");
    m.def("fire_async", &fire_async, "Fire an event handler asynchronously");
}
