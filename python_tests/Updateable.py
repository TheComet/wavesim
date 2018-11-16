__author__ = 'thecomet'


class Updateable(object):
    def process_event(self, event):
        pass

    def update(self, time_step):
        pass

    def draw(self, draw):
        pass
