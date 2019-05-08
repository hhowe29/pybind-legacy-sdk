import example

class EventListener(example.EventListener):
    def OnEvent(self, code, text):
        print('Py code got an event : %d : %s' % (code, text))


example.register_listener(EventListener())
example.fire()

#example.fire_async()
#input('press a key')