def distance(x, y, x2, y2):
    return abs(x - x2) + abs(y - y2)


if __name__ == "__main__":
    map = open("mapa.txt", "r")
    rows = map.readlines()
    map_matrix = []
    for i in rows:
        row = [x for x in i]
        row = list(filter(lambda x: x != "\n", row))
        map_matrix.append(row)
    rounds = 2000
    game_over = False
    while not game_over and rounds > 0:
        status = open("status.txt", "r")
        print(status.read())
        rozkazy = open("rozkazy.txt", "r")
        rozkazy = rozkazy.readlines()  # rozkaz linia po linii
        for i in rozkazy:
            rozkaz = i.split(" ")
            match rozkaz[0]:  # base and worker require different logic to combat units
                case "B":
                    break
                case "W":
                    break
                case _:
                    break

        game_over = True
