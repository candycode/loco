cmake_minimum_required(VERSION 2.8)

project("loco")

add_subdirectory( locoplay )
#enable only if ssl installed
add_subdirectory( ssltest )