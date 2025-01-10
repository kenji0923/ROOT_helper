add_library(DataSaver data_saver.cpp include/ROOT_helper/data_saver.h)
target_include_directories(DataSaver PUBLIC include)
target_link_libraries(DataSaver PUBLIC
    ROOT::Core
)


add_library(Graphics graphics.cpp include/ROOT_helper/graphics.h)
target_include_directories(Graphics PUBLIC include)
target_link_libraries(Graphics PUBLIC
    ROOT::Core
)


add_library(Container container.cpp include/ROOT_helper/container.h)
target_include_directories(Container PUBLIC include)
target_link_libraries(Container PUBLIC
    ROOT::Core
)


add_library(Analysis analysis.cpp include/ROOT_helper/analysis.h)
target_include_directories(Analysis PUBLIC include)
target_link_libraries(Analysis PUBLIC
    ROOT::Core
)
