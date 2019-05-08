# Exposing a legacy C++ SDK to python with pybind

## Background

Suppose you have a legacy C++ SDK that fires events via overridden virtual functions on an interface. 

```cpp
struct EventListener
{
    virtual void OnEvent(int code, const std::string &text) = 0;
};
```

You register a custom listener through a free standing function provided by the SDK.

```cpp
void register_listener(EventListener *listener)
```

The SDK holds a reference to your listener for the life of the program (ignore ownership details for now)


## Naive attempt to pybind the SDK over to python

We could pybind the interface and registration routing straight over to python like this
```
struct PyEventListener : public EventListener {
    void OnEvent(int code, std::string &text)
    {
        py::gil_scoped_acquire acquire;
        PYBIND11_OVERLOAD( ... );
    }
};

void register_pylistener(PyEventListener *listener) {
    register_listener(static_cast<EventListener*>(listener));
}


PYBIND11_MODULE(example, m)
{
    py::class_<EventListener, PyEventListener>(m, "EventListener")
        .def(py::init<>())
        .def("OnEvent", &EventListener::OnEvent);

    m.def("register_listener", &register_pylistener, "Registers a listener with the SDK");
}
```

On the python side, we can create a listener and register it with the SDK:

```python
import example

class EventListener(example.EventListener):
    def OnEvent(self, code, text):
        print('Py code got an event : %d : %s' % (code, text))

listener  = EventListener()
example.register_listener(listener)
# do something that fires events
```
 
Unfortunately, this creates a fragile situation in the python world. 
The python consumer must be careful to keep the lister alive for as long as the SDK holds a reference to it
This code above works. The code below SEGFAULTS:

```python
example.register_listener(EventListener())
# do something that fires events
```

## Solutions


### Wrapper class + pybind's keep-alive policy 

See the [`sdk-wrapper-class` branch](https://github.com/hhowe29/pybind-legacy-sdk/tree/sdk-wrapper-class)

### Use pyobject 

This suggestion was discussed in [a gitter thread](https://gitter.im/pybind/Lobby?at=5cd2d464da34620ff906fc66)  


### Use `shared_ptr` (failure)     

See the [`shared_ptr` branch](https://github.com/hhowe29/pybind-legacy-sdk/tree/shared-ptr). 
This fixes the seg fault, but unfortunately, results in slicing off the python part of the event listener. 
Virtual dispatching to the python side does not work. More details are posted in the gitter thread. 

