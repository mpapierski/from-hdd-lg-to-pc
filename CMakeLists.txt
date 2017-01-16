cmake_minimum_required (VERSION 2.6)
project (from_hdd_lg_to_pc CXX)

set (from_hdd_lg_to_pc_SOURCES
    from_hdd_lg_to_pc.cpp
    hdd_lg_config.cpp
    hdd_lg_copy.cpp
    hdd_lg_ctrl_fat.cpp
    hdd_lg_emulator.cpp
    hdd_lg_find_zero.cpp
    hdd_lg_func.cpp
    hdd_lg_func_1.cpp
    hdd_lg_func_folder.cpp
    hdd_lg_func_io.cpp
    hdd_lg_func_io_1.cpp
    hdd_lg_language.cpp
    hdd_lg_make_tree.cpp
    hdd_lg_map_hdd.cpp
    hdd_lg_mk_name.cpp
    hdd_lg_mme_db.cpp
    hdd_lg_out_test.cpp
    hdd_lg_read.cpp
    hdd_lg_read_io.cpp
    hdd_lg_remake_tree.cpp
    hdd_lg_sec_67_68_70_71.cpp
    hdd_lg_test0.cpp
    hdd_lg_test1.cpp
    hdd_lg_test2.cpp
    hdd_lg_test_idx.cpp
    hdd_lg_to_txt.cpp
    hdd_lg_write.cpp
    hdd_lg_write_1.cpp
    hdd_lg_write_2.cpp
    hdd_lg_write_io.cpp
    progress.cpp)

set (from_hdd_lg_to_pc_HEADERS
    defparam.h
    from_hdd_lg_to_pc.h
    from_hdd_lg_to_pc_def.h
    mme_db_head.h)

set (from_hdd_lg_to_pc_RESOURCES
    from_hdd_lg_to_pc_rc.rc)

add_executable (from_hdd_lg_to_pc WIN32
    ${from_hdd_lg_to_pc_SOURCES}
    ${from_hdd_lg_to_pc_HEADERS}
    ${from_hdd_lg_to_pc_RESOURCES})
target_link_libraries (from_hdd_lg_to_pc Comctl32)
