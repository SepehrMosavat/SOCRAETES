class IvCurve:
    number_of_points_in_curve = 40

    def __init__(self, _curve_number):
        self.curve_number = _curve_number
        self.curve_points_list = []

    def add_point_to_curve(self, _curve_point):
        self.curve_points_list.append(_curve_point)

    def print(self):
        for i in self.curve_points_list:
            print("V: " + str(i.x) + ", I: " + str(i.y))


class CurvePoint:
    def __init__(self, _sequence_number, _x, _y):
        self.sequence_number = _sequence_number
        self.x = _x
        self.y = _y