cmake_minimum_required(VERSION 2.8)


install(FILES ego-cli DESTINATION bin)
install(CODE
    "EXECUTE_PROCESS(COMMAND chmod +x ${CMAKE_INSTALL_PREFIX}/bin/ego-cli)"
)
