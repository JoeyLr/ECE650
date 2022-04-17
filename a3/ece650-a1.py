#!/usr/bin/env python
import math
import re
import sys


class Parser():
    def __init__(self, line):
        self.line = line
        self.parse_dict = {}
        self.coor_lst = []

    def parseLine(self):
        if self.line == "gg":
            self.parse_dict["cmd"] = "gg"
            return self.parse_dict
        elif self.line[0:2] == "rm":
            command = '(^[a-z]+) "([a-zA-Z ]+)"$'
            res = re.match(command, self.line)
            if res:
                self.parse_dict["cmd"] = res.group(1)
                self.parse_dict["name"] = res.group(2).lower()
            return self.parse_dict
        elif self.line[0:3] == "add" or "mod":
            command = r'(^[admorg]+)(?: +?"([a-zA-Z ]+?)" *?)?((?:\([-]?[0-9]+?,[-]?[0-9]+?\) *?)*)?$'
            res = re.match(command, self.line)
            if res:
                self.parse_dict["cmd"] = res.group(1)
                self.parse_dict["name"] = res.group(2).lower()
                coordinates = res.group(3)
                command_coordinate = '\([-]?[0-9]+,[-]?[0-9]+\)'
                res_coordinates = re.findall(command_coordinate, coordinates)
                command_x = '\(([-]?[0-9]+),'
                command_y = '([-]?[0-9]+)\)'
                for idx in range(0, len(res_coordinates)):
                    x = int(re.findall(command_x, res_coordinates[idx])[0])
                    y = int(re.findall(command_y, res_coordinates[idx])[0])
                    self.coor_lst.append([float(x), float(y)])
                self.parse_dict["coordinates"] = self.coor_lst
                return self.parse_dict


class StreetDatabase:
    def __init__(self):
        self.street_dict = {}  # {"street":{"points":[], "line segment": { 1: {"point_start":"", "point_end":"", "gradient":""}}}

    def calc_gradient(self, x_1, y_1, x_2, y_2):
        if x_1 - x_2 == 0:
            return "inf"
        else:
            grad = (y_1 - y_2) / (x_1 - x_2)
            return grad

    def add(self, street_name, coordinates):
        if street_name in self.street_dict.keys():
            print("Error: street currently exists.")
        if len(coordinates) < 2:
            print("Error: A street needs at least two coordinates")
        else:
            self.street_dict[street_name] = {}
            self.street_dict[street_name]["points"] = []
            self.street_dict[street_name]["line segment"] = {}
            for idx in range(0, len(coordinates)):
                self.street_dict[street_name]["points"].append(tuple(coordinates[idx]))

            for idx in range(0, len(coordinates) - 1):
                self.street_dict[street_name]["line segment"]["{0}".format(idx)] = {}
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["point_start"] = coordinates[idx]
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["point_end"] = coordinates[idx + 1]
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["gradient"] = self.calc_gradient(
                    coordinates[idx][0], coordinates[idx][1], coordinates[idx + 1][0], coordinates[idx + 1][1])

    def mod(self, street_name, coordinates):
        if street_name not in self.street_dict.keys():
            print("Error: 'mod' or 'rm' specified for a street that does not exist.")
        if len(coordinates) < 2:
            print("Error: A street needs at least two coordinates")
        else:
            self.street_dict[street_name] = {}
            self.street_dict[street_name]["points"] = []
            self.street_dict[street_name]["line segment"] = {}
            for idx in range(0, len(coordinates)):
                self.street_dict[street_name]["points"].append(tuple(coordinates[idx]))

            for idx in range(0, len(coordinates) - 1):
                self.street_dict[street_name]["line segment"]["{0}".format(idx)] = {}
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["point_start"] = coordinates[idx]
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["point_end"] = coordinates[idx + 1]
                self.street_dict[street_name]["line segment"]["{0}".format(idx)]["gradient"] = self.calc_gradient(
                    coordinates[idx][0], coordinates[idx][1], coordinates[idx + 1][0], coordinates[idx + 1][1])

    def rm(self, street_name):
        if street_name not in self.street_dict.keys():
            print("Error: 'mod' or 'rm' specified for a street that does not exist.")
        else:
            self.street_dict.pop(street_name)

    def check(self, street_name):
        return self.street_dict[street_name]

    def get(self):
        return self.street_dict


class Graph:
    def __init__(self):
        self.edges = []
        self.vertices = []


class GraphGenerator:

    def __init__(self, st_dict):
        self.st_dict = st_dict
        self.lines = []
        self.gradients = []
        self.intersections_dict = {}
        self.graph = Graph()

    def calc_dist(self, x1, y1, x2, y2):
        dist = math.sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2))
        return dist

    def is_parallel(self, g1, g2):
        if g1 != g2:
            return False
        else:
            return True

    def store_lines(self):
        for street in self.st_dict.values():
            for idx in range(len(street["points"]) - 1):
                self.lines.append([street["points"][idx], street["points"][idx + 1]])

    def store_gradients(self):
        for street in self.st_dict.values():
            for line_idx in street["line segment"].keys():
                self.gradients.append((street["line segment"][line_idx]['gradient']))

    def calc_coordinate_axis_intersection(self, l, g):
        intersection = -(l[0][0] * g) + l[0][1]
        return intersection

    def calc_intersections(self, l1, l2, g1, g2):
        if self.is_parallel(l1, l2):
            return None
        else:
            if g1 != "inf" and g2 != "inf":
                x = (1. / (g1 - g2)) * (
                        self.calc_coordinate_axis_intersection(l2, g2) - self.calc_coordinate_axis_intersection(l1, g1))
                y = (g1 * x) + self.calc_coordinate_axis_intersection(l1, g1)
            else:
                if g1 == "inf":
                    x = l1[0][0]
                    y = (g2 * x) + self.calc_coordinate_axis_intersection(l2, g2)
                elif g2 == "inf":
                    x = l2[0][0]
                    y = (g1 * x) + self.calc_coordinate_axis_intersection(l1, g1)
            x = float(round(x, 2))
            y = float(round(y, 2))
            return x, y

    def add_intersection_point(self, point, l):
        self.intersections_dict.setdefault(tuple(l), [])
        if point not in self.intersections_dict[tuple(l)]:
            self.intersections_dict[tuple(l)].append(point)

    def get_key_x(self, item):
        return item[0]

    def get_key_y(self, item):
        return item[1]

    def overlap_point(self, l1, l2):
        if l1[0][0] == l2[0][0] and l1[0][0] == l2[1][0]:
            if l1[0][1] >= sorted(l2, key=self.get_key_y)[0][1] and l1[0][1] <= sorted(l2, key=self.get_key_y)[1][1]:
                self.add_intersection_point(l1[0], l2)
        elif l1[0][0] >= sorted(l2, key=self.get_key_x)[0][0] and l1[0][0] <= sorted(l2, key=self.get_key_x)[1][0]:
            self.add_intersection_point(l1[0], l2)

        if l1[1][0] == l2[0][0] and l1[1][0] == l2[1][0]:
            if l1[1][1] >= sorted(l2, key=self.get_key_y)[0][1] and l1[1][1] <= sorted(l2, key=self.get_key_y)[1][1]:
                self.add_intersection_point(l1[1], l2)
        elif l1[1][0] >= sorted(l2, key=self.get_key_x)[0][0] and l1[1][0] <= sorted(l2, key=self.get_key_x)[1][0]:
            self.add_intersection_point(l1[1], l2)

    def is_intersect(self, l1, l2, g1, g2):
        if self.is_parallel(g1, g2) is not True:
            x, y = self.calc_intersections(l1, l2, g1, g2)
            range_1 = max(min(l1[0][0], l1[1][0]), min(l2[0][0], l2[1][0]))
            range_2 = min(max(l1[0][0], l1[1][0]), max(l2[0][0], l2[1][0]))
            range_l = min(range_1, range_2)
            range_h = max(range_1, range_2)

            if range_l <= x <= range_h:
                dist = self.calc_dist(l1[0][0], l1[0][1], l1[1][0], l1[1][1])
                dist_1 = self.calc_dist(l1[0][0], l1[0][1], x, y)
                dist_2 = self.calc_dist(x, y, l1[1][0], l1[1][1])

                if dist_1 <= dist and dist_2 <= dist:
                    if (x, y) not in l1 or (x, y) not in l2:
                        self.add_intersection_point((x, y), l1)
                        self.add_intersection_point((x, y), l2)
                    # if (x, y) not in l1 and (x, y) in l2:
                    #     self.add_intersection_point((x, y), l2)
                    # if (x, y) not in l2 and (x, y) in l1:
                    #     self.add_intersection_point((x, y), l1)
        else:
            if g1 == "inf" and g2 == "inf":
                k1 = g1
                k2 = g2
                b1 = 0
                b2 = 0
            else:
                b1 = self.calc_coordinate_axis_intersection(l1, g1)
                k1 = g1

                b2 = self.calc_coordinate_axis_intersection(l2, g2)
                k2 = g2

            if k1 == k2 and b1 == b2:
                if l1[0] == l2[0] and l1[1] == l2[1] or l1[0] == l2[1] and l1[1] == l2[0]:
                    self.add_intersection_point(l2[0], l1)
                    self.add_intersection_point(l2[1], l1)
                    self.add_intersection_point(l1[0], l2)
                    self.add_intersection_point(l1[1], l2)
                else:
                    self.overlap_point(l1, l2)
                    self.overlap_point(l2, l1)

    def add_edges(self, k, v, idx_1, idx_2):
        if k[idx_1] != v[idx_2] and (k[idx_1], v[idx_2]) not in self.graph.edges and (
                v[idx_2], k[idx_1]) not in self.graph.edges:
            self.graph.edges.append((k[idx_1], v[idx_2]))

    def gen_graph(self):
        self.store_lines()
        self.store_gradients()
        for idx in range(len(self.lines)):
            for idx_1 in range(idx + 1, len(self.lines)):
                self.is_intersect(self.lines[idx], self.lines[idx_1], self.gradients[idx], self.gradients[idx_1])

        for k, v in self.intersections_dict.items():
            self.add_edges(k, v, 0, 0)

            for idx in range(0, len(v) - 1):
                self.add_edges(v, v, idx, idx + 1)

            self.add_edges(k, v, 1, len(v) - 1)

        for k, v in self.intersections_dict.items():
            self.graph.vertices.append(k[0])
            self.graph.vertices.append(k[1])
            for item in v:
                self.graph.vertices.append(item)

        vertices_set = set(self.graph.vertices)
        self.graph.vertices = list(vertices_set)

        return self.graph.edges, self.graph.vertices


class GraphPrinter:
    def __init__(self, edges, vertices):
        self.edges = edges
        self.vertices = vertices
        self.vertices_dict = {}
        self.final_edges = []

    def print_graph(self):
        # print("as;dflkajdsf;j;lj")
        for idx in range(len(self.vertices)):
            self.vertices_dict[idx + 1] = self.vertices[idx]
        reversed_dict = {v: k for k, v in self.vertices_dict.items()}
        for idx in range(len(self.edges)):
            self.final_edges.append([reversed_dict[self.edges[idx][0]], reversed_dict[self.edges[idx][1]]])

        print("V", len(self.vertices_dict.keys()))
        # for k, v in self.vertices_dict.items():
        #     print("{0}:  ({1:.2f},{2:.2f})".format(k, v[0], v[1]))
        # print("}")

        sys.stdout.write("E {")
        for idx, item in enumerate(self.final_edges):
            # sys.stdout.write("<{0},{1}>,".format(item[0], item[1]))
            # if idx == len(self.final_edges) - 1:
            #     sys.stdout.write("<{0},{1}>".format(item[0], item[1]))
            comma = "" if (idx == len(self.final_edges) - 1) else ","
            sys.stdout.write("<{0},{1}>".format(item[0], item[1]) + comma)
        print("}")


if __name__ == '__main__':
    dataBase = StreetDatabase()
    while True:
        try:
            line = input()
            # inp =dummy_in[a]
        except EOFError:
            sys.exit(0)
        if line == '':
            sys.exit(0)
        parser = Parser(line)
        parse_dict = parser.parseLine()
        # print("hahaha " + str(parse_dict))
        if parse_dict is None or parse_dict == {}:
            print("Error: Incorrect input format")
            continue

        if parse_dict["cmd"] == "add":
            dataBase.add(parse_dict["name"], parse_dict["coordinates"])
            # print(dataBase.get())
        elif parse_dict["cmd"] == "mod":
            dataBase.mod(parse_dict["name"], parse_dict["coordinates"])
            # print(dataBase.get())
        elif parse_dict["cmd"] == "rm":
            dataBase.rm(parse_dict["name"])
            # print(dataBase.get())
        elif parse_dict["cmd"] == "gg":
            # print("V 15\nE {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}")
            graphGenerator = GraphGenerator(dataBase.get())
            edges, vertices = graphGenerator.gen_graph()
            graphPrinter = GraphPrinter(edges, vertices)
            graphPrinter.print_graph()
        else:
            print("Error: Incorrect command")
