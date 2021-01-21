to build with ceph's spawn library:
```
 g++ --std=c++17 -Wall -o test_spawn test_spawn.cpp -lboost_coroutine -lboost_context -lpthread -I ./spawn/include/
```

to build with boost's spawn library:
```
 g++ -DUSE_BOOST_SPAWN --std=c++17 -Wall -o test_spawn test_spawn.cpp -lboost_coroutine -lboost_context -lpthread -I ./spawn/include/
```

to make sure that the process crash on stack overflow (either for ceph or for boost spawn libraries):
```
 g++ -DTEST_CRASH --std=c++17 -Wall -o test_spawn test_spawn.cpp -lboost_coroutine -lboost_context -lpthread -I ./spawn/include/
```

