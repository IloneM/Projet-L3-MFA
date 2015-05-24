DEBUG = 0
CPPFLAGS = -std=gnu++11 -fopenmp -I/usr/include/eigen3/ -I/usr/local/include/libcmaes/
LDFLAGS = -L/usr/local/lib/ -lcmaes -ljsoncpp
HPP_EXTENSIONS += h
EXEC = test-rpcma-special

