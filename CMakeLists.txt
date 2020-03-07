cmake_minimum_required(VERSION 3.10)
project(457ASG3 C)

set(CMAKE_C_STANDARD 11)

add_executable(457ASG3 countPrimes.c)

#pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(457ASG3 Threads::Threads)

#math
target_link_libraries(457ASG3 m)


