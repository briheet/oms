env = Environment()

env.Append(LIBS=["curl"])

env.ParseConfig("pkg-config --cflags --libs libcurl")

Program("main", ["main.cpp", "src/p.cpp", "src/ordermanager.cpp", "src/token.cpp"])
