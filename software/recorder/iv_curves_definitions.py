import json


class IvCurve:
    number_of_points_in_curve = 40

    def __init__(self, _curve_number):
        self.curve_number = _curve_number
        self.curve_points_list = []

    def add_point_to_curve(self, _curve_point):
        self.curve_points_list.append(_curve_point)

    def encode_to_json(self):
        curve_dict = {}

        curve_dict["curve_number"] = self.curve_number

        encoded_curve_point_list = []
        for i in self.curve_points_list:
            encoded_curve_point_list.append(i.asdict())

        curve_dict["curve_points"] = encoded_curve_point_list

        print(json.dumps(curve_dict, ensure_ascii=False))

    def print(self):
        for i in self.curve_points_list:
            print("V: " + str(i.x) + ", I: " + str(i.y))


class CurvePoint:
    def __init__(self, _sequence_number, _x, _y):
        self.sequence_number = _sequence_number
        self.x = _x
        self.y = _y

    def asdict(self):
        return {"seq_nr": self.sequence_number, "volt": self.x, "micro_ampere": self.y}


class HarvestingCondition:
    def __init__(self, _indoor_or_outdoor: str, _light_intensity: str, _weather_condition: str, _country: str,
                 _city: str):
        self.indoor_or_outdoor = _indoor_or_outdoor
        self.light_intensity = _light_intensity
        self.weather_condition = _weather_condition
        self.country = _country
        self.city = _city
